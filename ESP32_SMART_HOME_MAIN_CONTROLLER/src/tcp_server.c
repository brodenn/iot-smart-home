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

#define PORT 8080
static const char *TAG = "TCP_SERVER";

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