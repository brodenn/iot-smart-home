/**
 * @file handshake.c
 * @brief This file contains the implementation of the handshake functions for the ESP32 Smart Home Main Controller project.
 *
 * The functions provided in this file allow for performing a handshake with the Arduino to establish a reliable connection.
 *
 * The main functionalities provided by this file include:
 * - Sanitizing handshake input to remove extra characters.
 * - Performing the handshake process with the Arduino.
 *
 * Dependencies:
 * - tcp_server.h: TCP server function declarations.
 * - globals.h: Global variables and definitions.
 * - esp_log.h: ESP32 logging functions.
 * - sys/socket.h: Socket functions.
 * - errno.h: Error handling functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "handshake.h"
#include "tcp_server.h"
#include "globals.h"
#include "esp_log.h"
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

static const char *TAG = "HANDSHAKE";

/**
 * @brief Sanitizes handshake input to remove extra characters.
 *
 * This function removes newline characters and unwanted "AT" characters from the handshake input.
 *
 * @param input The input string to sanitize.
 */
void sanitize_handshake(char *input) {
    char *newline = strchr(input, '\n');
    if (newline) *newline = '\0';  

    char *extra = strstr(input, "AT");  // Remove unwanted AT characters
    if (extra) *extra = '\0';  
}

/**
 * @brief Performs the handshake process with the Arduino.
 *
 * This function attempts to perform a handshake with the Arduino by sending and receiving handshake messages.
 * It retries the handshake process up to a maximum number of retries if the handshake is not successful.
 */
void performHandshake() {
    if (handshake_done) {
        ESP_LOGW(TAG, "⚠️ Handshake already completed. Skipping...");
        return;
    }

    handshake_done = false;
    int handshakeRetries = 0;
    const int maxRetries = 6;  

    ESP_LOGI(TAG, "🕐 Waiting 3s before first handshake attempt...");
    vTaskDelay(pdMS_TO_TICKS(3000));

    while (!handshake_done && handshakeRetries < maxRetries) {
        char ack_buffer[128] = {0};  
        int total_received = 0;

        struct timeval timeout = {.tv_sec = 10, .tv_usec = 0};
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        ESP_LOGI(TAG, "📨 Waiting for handshake response...");

        while (total_received < sizeof(ack_buffer) - 1) {
            int bytes_received = recv(client_sock, ack_buffer + total_received, sizeof(ack_buffer) - total_received - 1, 0);
            if (bytes_received <= 0) {
                break;
            }
            total_received += bytes_received;

            if (ack_buffer[total_received - 1] == '\n') {
                break;  // Stop if full handshake message received
            }
        }
        ack_buffer[total_received] = '\0';

        if (total_received > 0) {
            sanitize_handshake(ack_buffer);

            ESP_LOGI(TAG, "📩 Received clean handshake: '%s'", ack_buffer);

            if (strcmp(ack_buffer, "HANDSHAKE:ARDUINO_READY") == 0) {
                ESP_LOGI(TAG, "✅ Handshake received from Arduino. Sending response...");
                send_tcp_message("HANDSHAKE:ESP32_READY\n");

                handshake_done = true;
                ESP_LOGI(TAG, "🎉 Handshake completed! Connection is ready.");
                return;
            } else {
                ESP_LOGE(TAG, "❌ Unexpected handshake message: '%s'", ack_buffer);
            }
        } else {
            ESP_LOGW(TAG, "⚠️ No valid handshake received. Retrying...");
        }

        handshakeRetries++;
        ESP_LOGW(TAG, "🔁 Retrying handshake in 3s... (%d/%d)", handshakeRetries, maxRetries);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

    ESP_LOGE(TAG, "🚨 Handshake FAILED after max retries!");
    send_tcp_message("ERROR:HANDSHAKE_FAILED\n");
}
