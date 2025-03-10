/**
 * @file main.c
 * @brief This file contains the main entry point for the ESP32 Smart Home Main Controller project.
 *
 * The main function initializes the Wi-Fi interface, starts the TCP server, and starts the HTTP server.
 *
 * Dependencies:
 * - wifi.h: Wi-Fi initialization and event handling functions.
 * - tcp_server.h: TCP server function declarations.
 * - http_server.h: HTTP server function declarations.
 * - json_parser.h: JSON parsing helper functions.
 * - esp_log.h: ESP32 logging functions.
 * - freertos/FreeRTOS.h: FreeRTOS functions.
 * - freertos/task.h: FreeRTOS task functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "wifi.h"
#include "tcp_server.h"
#include "http_server.h"
#include "json_parser.h"
#include <stddef.h>  // For NULL
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief Main entry point for the ESP32 Smart Home Main Controller project.
 *
 * This function initializes the Wi-Fi interface, starts the TCP server, and starts the HTTP server.
 */
void app_main(void) {
    ESP_LOGI("MAIN", "Starting Wi-Fi...");
    wifi_init();

    ESP_LOGI("MAIN", "Starting TCP server...");
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Starting HTTP server...");
    start_http_server();
}
