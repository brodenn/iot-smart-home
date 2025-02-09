#include "handshake.h"
#include "tcp_server.h"
#include "globals.h"
#include "esp_log.h"
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

static const char *TAG = "HANDSHAKE";

void performHandshake() {
    handshake_done = false;
    int handshakeRetries = 0;
    const int maxHandshakeRetries = 5;

    while (!handshake_done && handshakeRetries < maxHandshakeRetries) {
        // Wait for handshake message from Arduino
        char ack_buffer[32];
        int ack_len = recv(client_sock, ack_buffer, sizeof(ack_buffer) - 1, 0);
        if (ack_len > 0) {
            ack_buffer[ack_len] = '\0';
            ESP_LOGI(TAG, "Received handshake message: %s", ack_buffer);
            if (strncmp(ack_buffer, "HANDSHAKE:ARDUINO_READY\n", 24) == 0) {
                ESP_LOGI(TAG, "Handshake received from Arduino. Sending response...");
                send_tcp_message("HANDSHAKE:ESP32_READY\n");
                handshake_done = true;
            } else {
                ESP_LOGE(TAG, "Unexpected handshake message: %s", ack_buffer);
                send_tcp_message("ERROR:HANDSHAKE_FAILED\n");
            }
        } else {
            ESP_LOGE(TAG, "Failed to receive handshake message. Retrying...");
        }
        handshakeRetries++;
    }

    if (!handshake_done) {
        ESP_LOGE(TAG, "Failed to perform handshake after retries");
        send_tcp_message("ERROR:HANDSHAKE_FAILED\n");
    }
}