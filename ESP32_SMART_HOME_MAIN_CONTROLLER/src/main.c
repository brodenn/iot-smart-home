#include "wifi.h"
#include "tcp_server.h"
#include "http_server.h"
#include "json_parser.h"
#include <stddef.h>  // For NULL
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    ESP_LOGI("MAIN", "Starting Wi-Fi...");
    wifi_init();

    ESP_LOGI("MAIN", "Starting TCP server...");
    xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Starting HTTP server...");
    start_http_server();
}
