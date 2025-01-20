#include "cJSON.h"
#include "esp_log.h"
#include "globals.h" 

static const char *TAG = "JSON_PARSER";

// Function to parse the incoming JSON data
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

    cJSON_Delete(root); // Clean up the JSON object
}
