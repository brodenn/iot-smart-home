/**
 * @file json_parser.c
 * @brief This file contains the implementation of JSON parsing functions for the ESP32 Smart Home Main Controller project.
 *
 * The functions provided in this file allow for parsing incoming JSON data to extract sensor readings and control commands.
 *
 * The main functionalities provided by this file include:
 * - Parsing JSON data to extract temperature, humidity, and light intensity readings.
 * - Parsing JSON data to extract control commands for the heater and dehumidifier.
 * - Parsing JSON data to extract setpoints for temperature and humidity.
 *
 * Dependencies:
 * - esp_http_server.h: ESP32 HTTP server functions.
 * - esp_log.h: ESP32 logging functions.
 * - cJSON.h: JSON parsing library.
 * - globals.h: Global variables and definitions.
 * - esp_err.h: ESP32 error handling functions.
 * - tcp_server.h: TCP server function declarations.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "globals.h"
#include "esp_err.h"
#include "tcp_server.h"

static const char *TAG = "JSON_PARSER";

/**
 * @brief Parses the incoming JSON data.
 *
 * This function parses the incoming JSON data to extract sensor readings and control commands.
 * It updates the global variables with the extracted values.
 *
 * @param json_data The incoming JSON data.
 */
void parse_json(const char* json_data) {
    cJSON *root = cJSON_Parse(json_data);
    if (root == NULL) {
        ESP_LOGE(TAG, "Error parsing JSON");
        return;
    }

    cJSON *temp = cJSON_GetObjectItem(root, "temperature");
    if (cJSON_IsNumber(temp)) {
        temperature = temp->valuedouble;
    }

    cJSON *hum = cJSON_GetObjectItem(root, "humidity");
    if (cJSON_IsNumber(hum)) {
        humidity = hum->valuedouble;
    }

    cJSON *lux_item = cJSON_GetObjectItem(root, "lux");
    if (cJSON_IsNumber(lux_item)) {
        lux = lux_item->valueint;
    }

    cJSON *heater_item = cJSON_GetObjectItem(root, "heater");
    if (cJSON_IsBool(heater_item)) {
        heater = heater_item->valueint;
    }

    cJSON *dehumidifier_item = cJSON_GetObjectItem(root, "dehumidifier");
    if (cJSON_IsBool(dehumidifier_item)) {
        dehumidifier = dehumidifier_item->valueint;
    }

    cJSON *sp_temp_item = cJSON_GetObjectItem(root, "sp_temperature");
    if (cJSON_IsNumber(sp_temp_item)) {
        SP_TEMP = (int16_t)(sp_temp_item->valuedouble * 100);
    }

    cJSON *sp_hum_item = cJSON_GetObjectItem(root, "sp_humidity");
    if (cJSON_IsNumber(sp_hum_item)) {
        SP_HUM = (int16_t)(sp_hum_item->valuedouble * 100);
    }

    cJSON_Delete(root); // Clean up the JSON object
}