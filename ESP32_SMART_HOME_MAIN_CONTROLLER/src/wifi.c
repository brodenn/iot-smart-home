/**
 * @file wifi.c
 * @brief This file contains the implementation of Wi-Fi initialization and event handling functions for the ESP32 module.
 *
 * The functions provided in this file allow for initializing the Wi-Fi interface, connecting to a Wi-Fi network,
 * and handling Wi-Fi events such as connection and disconnection.
 *
 * The main functionalities provided by this file include:
 * - Initializing the Wi-Fi interface.
 * - Connecting to a Wi-Fi network.
 * - Handling Wi-Fi events.
 *
 * Dependencies:
 * - esp_wifi.h: ESP32 Wi-Fi functions.
 * - esp_event.h: ESP32 event handling functions.
 * - esp_log.h: ESP32 logging functions.
 * - nvs_flash.h: ESP32 non-volatile storage functions.
 * - lwip/inet.h: LwIP internet address functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/inet.h"

#define WIFI_SSID "TN_24GHz_F3908D"
#define WIFI_PASS "UP7ADFCFXJ"

static const char *TAG = "WIFI";

/**
 * @brief Wi-Fi event handler.
 *
 * This function handles Wi-Fi events such as station start, disconnection, and IP acquisition.
 *
 * @param arg User-defined argument.
 * @param event_base Event base.
 * @param event_id Event ID.
 * @param event_data Event data.
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected from Wi-Fi, retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: %s", ip4addr_ntoa((const ip4_addr_t*)&event->ip_info.ip));
    }
}

/**
 * @brief Initializes the Wi-Fi interface.
 *
 * This function initializes the non-volatile storage, network interface, event loop, and Wi-Fi interface.
 * It then configures the Wi-Fi interface to connect to the specified SSID and password.
 */
void wifi_init(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_config_t sta_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
}
