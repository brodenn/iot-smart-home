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

const char* heaterIP = "192.168.10.199"; // IP address of your heater Shelly Plus Plug S
const char* humidifierIP = "192.168.10.201"; // IP address of your humidifier Shelly Plus Plug S

void send_http_request(const char* deviceIP, bool turnOn) {
    char url[128];
    snprintf(url, sizeof(url), "http://%s/rpc/Switch.Set?id=0&on=%s", deviceIP, turnOn ? "true" : "false");

    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = 5000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

void send_tcp_message(const char *message) {
    if (client_sock < 0) {
        ESP_LOGE(TAG, "No client connected");
        return;
    }

    int len = strlen(message);
    int to_write = len;
    while (to_write > 0) {
        int written = send(client_sock, message + (len - to_write), to_write, 0);
        if (written < 0) {
            ESP_LOGE(TAG, "Send failed: errno %d", errno);
            break;
        }
        to_write -= written;
    }
    ESP_LOGI(TAG, "Sent message: %s", message);
}

void send_tcp_message_with_ack(const char *message) {
    const int maxRetries = 3;
    int retries = 0;
    bool ackReceived = false;

    while (retries < maxRetries && !ackReceived) {
        send_tcp_message(message);

        char ack_buffer[32];
        int ack_len = recv(client_sock, ack_buffer, sizeof(ack_buffer) - 1, 0);
        if (ack_len > 0) {
            ack_buffer[ack_len] = '\0';
            if (strncmp(ack_buffer, "ACK\n", 4) == 0) {
                ackReceived = true;
                ESP_LOGI(TAG, "Acknowledgment received");
            } else {
                ESP_LOGE(TAG, "Unexpected response: %s", ack_buffer);
            }
        } else {
            ESP_LOGE(TAG, "Failed to receive acknowledgment. Retrying...");
        }
        retries++;
    }

    if (!ackReceived) {
        ESP_LOGE(TAG, "Failed to receive acknowledgment after retries");
    }
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

void handle_received_data(const char* data) {
    cJSON *json = cJSON_Parse(data);
    if (json == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return;
    }

    cJSON *temperature_item = cJSON_GetObjectItem(json, "temperature");
    cJSON *humidity_item = cJSON_GetObjectItem(json, "humidity");
    cJSON *lux_item = cJSON_GetObjectItem(json, "lux");
    cJSON *heater_item = cJSON_GetObjectItem(json, "heater");
    cJSON *dehumidifier_item = cJSON_GetObjectItem(json, "dehumidifier");

    if (cJSON_IsNumber(temperature_item) && cJSON_IsNumber(humidity_item) && cJSON_IsNumber(lux_item) && cJSON_IsBool(heater_item) && cJSON_IsBool(dehumidifier_item)) {
        temperature = temperature_item->valuedouble;
        humidity = humidity_item->valuedouble;
        lux = lux_item->valueint;
        heater = heater_item->valueint;
        dehumidifier = dehumidifier_item->valueint;

        ESP_LOGI(TAG, "Parsed data: temperature=%.2f, humidity=%.2f, lux=%d, heater=%s, dehumidifier=%s",
                 temperature, humidity, lux,
                 heater ? "true" : "false",
                 dehumidifier ? "true" : "false");

        // Control Shelly devices based on the parsed data
        send_http_request(heaterIP, heater);
        send_http_request(humidifierIP, dehumidifier);

        // Update web server with the data
        char web_server_message[256];
        snprintf(web_server_message, sizeof(web_server_message), 
                 "{\"temperature\":%.2f,\"humidity\":%.2f,\"lux\":%d,\"heater\":%s,\"dehumidifier\":%s}",
                 temperature, humidity, lux,
                 heater ? "true" : "false",
                 dehumidifier ? "true" : "false");
        send_data_to_web_server(web_server_message);
    } else {
        ESP_LOGE(TAG, "Invalid data format");
    }

    cJSON_Delete(json);
}

// Function to initialize the TCP server
void tcp_server_task(void *pvParameters) {
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = addr_family;
    server_addr.sin_port = htons(PORT);

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }
    ESP_LOGI(TAG, "Socket listening");

    while (1) {
        ESP_LOGI(TAG, "Waiting for a connection...");
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        inet_ntoa_r(client_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
        ESP_LOGI(TAG, "Client connected: %s", addr_str);

        performHandshake();

        char rx_buffer[128];
        while (1) {
            int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (len < 0) {
                ESP_LOGE(TAG, "Receive failed: errno %d", errno);
                break;
            } else if (len == 0) {
                ESP_LOGI(TAG, "Client disconnected");
                break;
            }

            rx_buffer[len] = '\0';
            ESP_LOGI(TAG, "Received: %s", rx_buffer);

            // Check if the message is a data message
            if (strncmp(rx_buffer, "DATA:", 5) == 0) {
                ESP_LOGI(TAG, "Received data: %s", rx_buffer + 5);
                handle_received_data(rx_buffer + 5);
                // Send acknowledgment
                send_tcp_message("ACK\n");
            } else if (strncmp(rx_buffer, "HANDSHAKE:", 10) == 0) {
                ESP_LOGI(TAG, "Received handshake message: %s", rx_buffer);
                if (strncmp(rx_buffer, "HANDSHAKE:ARDUINO_READY\n", 24) == 0) {
                    ESP_LOGI(TAG, "Handshake received from Arduino. Sending response...");
                    send_tcp_message("HANDSHAKE:ESP32_READY\n");
                    handshake_done = true;
                } else {
                    ESP_LOGE(TAG, "Unexpected handshake message: %s", rx_buffer);
                    send_tcp_message("ERROR:HANDSHAKE_FAILED\n");
                }
            } else if (strncmp(rx_buffer, "SETPOINTS_ACK\n", 14) == 0) {
                ESP_LOGI(TAG, "Setpoints acknowledgment received from Arduino");
            } else {
                ESP_LOGE(TAG, "Unexpected message: %s", rx_buffer);
                send_tcp_message("ERROR:UNEXPECTED_MESSAGE\n");
            }
        }

        close(client_sock);
        client_sock = -1;
        handshake_done = false;
        ESP_LOGI(TAG, "Client connection closed");
    }

    close(listen_sock);
    vTaskDelete(NULL);
}