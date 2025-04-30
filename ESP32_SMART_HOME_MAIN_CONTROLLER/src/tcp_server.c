/**
 * @file tcp_server.c
 * @brief This file contains the implementation of the TCP server and related functions for the ESP32 module.
 *
 * The functions provided in this file allow for initializing and running a TCP server, handling incoming TCP connections,
 * sending and receiving TCP messages, and controlling devices such as the heater and humidifier via HTTP requests.
 *
 * The main functionalities provided by this file include:
 * - Initializing and running a TCP server.
 * - Handling incoming TCP connections and messages.
 * - Sending and receiving TCP messages.
 * - Controlling devices via HTTP requests.
 *
 * Dependencies:
 * - esp_log.h: ESP32 logging functions.
 * - esp_http_client.h: ESP32 HTTP client functions.
 * - cJSON.h: JSON parsing library.
 * - wifi.h: Wi-Fi initialization and event handling functions.
 * - json_parser.h: JSON parsing helper functions.
 * - globals.h: Global variables and definitions.
 * - handshake.h: Handshake functions.
 * - tcp_server.h: TCP server function declarations.
 * - shelly_control.h: Shelly device control functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "cJSON.h"
#include "wifi.h"
#include "json_parser.h"
#include "globals.h"
#include "handshake.h"
#include "tcp_server.h"
#include "esp_http_client.h"
#include "shelly_control.h"

#define PORT 8080
static const char *TAG = "TCP_SERVER";

const char* heaterIP = "192.168.10.199";
const char* humidifierIP = "192.168.10.201";


void sanitize_input(char *input) {
    size_t len = strlen(input);
    while (len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r' || input[len - 1] == '}')) {
        input[--len] = '\0'; 
    }
}


void send_http_request(const char* deviceIP, bool turnOn) {
    char url[128];
    snprintf(url, sizeof(url), "http://%s/rpc/Switch.Set?id=0&on=%s", deviceIP, turnOn ? "true" : "false");

    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "✅ HTTP GET successful: %s", url);
    } else {
        ESP_LOGE(TAG, "❌ HTTP GET failed: %s", esp_err_to_name(err));
    }
}


void send_tcp_message(const char *message) {
    if (client_sock < 0) return;

    int to_write = strlen(message);
    while (to_write > 0) {
        int written = send(client_sock, message + (strlen(message) - to_write), to_write, 0);
        if (written < 0) {
            ESP_LOGE(TAG, "❌ Send failed: errno %d", errno);
            return;
        }
        to_write -= written;
    }
}


void send_tcp_message_with_ack(const char *message, bool *ackReceived) {
    const int maxRetries = 3;
    *ackReceived = false;

    for (int retries = 0; retries < maxRetries; retries++) {
        send_tcp_message(message);

        char ack_buffer[32];
        int ack_len = recv(client_sock, ack_buffer, sizeof(ack_buffer) - 1, 0);
        if (ack_len > 0) {
            ack_buffer[ack_len] = '\0';
            if (strncmp(ack_buffer, "ACK\n", 4) == 0) {
                *ackReceived = true;
                return;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


void handle_received_data(const char* data) {
    static char json_buffer[512] = {0};  
    static size_t json_index = 0;

    size_t data_len = strlen(data);
    if (json_index + data_len >= sizeof(json_buffer)) {
        ESP_LOGE(TAG, "⚠️ JSON buffer overflow! Resetting.");
        json_index = 0;
    }

    strncat(json_buffer, data, sizeof(json_buffer) - json_index - 1);
    json_index += data_len;

    // Check if complete JSON is received
    if (json_buffer[0] != '{' || json_buffer[json_index - 1] != '}') {
        ESP_LOGW(TAG, "⚠️ Waiting for full JSON...");
        return;
    }

    ESP_LOGI(TAG, "📥 Full JSON received: %s", json_buffer);

    cJSON *json = cJSON_Parse(json_buffer);
    if (!json) {
        ESP_LOGE(TAG, "❌ JSON parsing failed!");
        json_index = 0;
        return;
    }

    json_index = 0;

    cJSON *temp = cJSON_GetObjectItem(json, "temperature");
    cJSON *hum = cJSON_GetObjectItem(json, "humidity");
    cJSON *lux = cJSON_GetObjectItem(json, "lux");
    cJSON *heater = cJSON_GetObjectItem(json, "heater");
    cJSON *dehumidifier = cJSON_GetObjectItem(json, "dehumidifier");

    if (cJSON_IsNumber(temp) && cJSON_IsNumber(hum) &&
        cJSON_IsNumber(lux) && cJSON_IsBool(heater) && cJSON_IsBool(dehumidifier)) {
        
        temperature = temp->valuedouble;
        humidity = hum->valuedouble;
        lux = lux->valueint;

        ESP_LOGI(TAG, "🌡 Temp: %.2f°C, 💧 Humidity: %.2f%%, ☀️ Lux: %d, 🔥 Heater: %s, ❄️ Dehumidifier: %s",
                 temperature, humidity, lux,
                 heater->valueint ? "ON" : "OFF",
                 dehumidifier->valueint ? "ON" : "OFF");

        send_http_request(heaterIP, heater->valueint);
        send_http_request(humidifierIP, dehumidifier->valueint);
    }

    cJSON_Delete(json);
}


void tcp_server_task(void *pvParameters) {
    struct sockaddr_in server_addr = {
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
    };

    int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "❌ Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "✅ Socket created");

    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        ESP_LOGE(TAG, "❌ Bind failed: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
    }

    if (listen(listen_sock, 1) != 0) {
        ESP_LOGE(TAG, "❌ Listen failed: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "🎧 Listening on port %d", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "❌ Accept failed: errno %d", errno);
            continue;
        }
        ESP_LOGI(TAG, "✅ Client connected");
        performHandshake();

        char rx_buffer[128];
        while (1) {
            int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len <= 0) break;

            rx_buffer[len] = '\0';
            sanitize_input(rx_buffer);

            if (strncmp(rx_buffer, "DATA:", 5) == 0) {
                handle_received_data(rx_buffer + 5);
                send_tcp_message("ACK\n");
            }
        }

        close(client_sock);
        client_sock = -1;
    }

    close(listen_sock);
    vTaskDelete(NULL);
}


void send_setpoints_with_ack(const char *setpoints) {
    const int maxRetries = 5;
    int retries = 0;
    bool ackReceived = false;

    while (retries < maxRetries && !ackReceived) {
        send_tcp_message(setpoints);

        char ack_buffer[32];
        int ack_len = recv(client_sock, ack_buffer, sizeof(ack_buffer) - 1, 0);
        if (ack_len > 0) {
            ack_buffer[ack_len] = '\0';
            if (strncmp(ack_buffer, "SETPOINTS_ACK\n", 14) == 0) {
                ackReceived = true;
                ESP_LOGI(TAG, "Setpoints acknowledgment received");
            } else {
                ESP_LOGE(TAG, "Unexpected response: %s", ack_buffer);
            }
        } else {
            ESP_LOGE(TAG, "Failed to receive setpoints acknowledgment. Retrying...");
        }
        retries++;
    }

    if (!ackReceived) {
        ESP_LOGE(TAG, "Failed to receive setpoints acknowledgment after retries");
    }
}


void send_data_to_web_server(const char* data) {
    esp_http_client_config_t config = {
        .url = "http://192.168.10.206/update", // Replace with your actual web server URL or IP address
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, data, strlen(data));
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
