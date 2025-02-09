#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "globals.h"
#include "esp_err.h"
#include "tcp_server.h"

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

esp_err_t setpoints_handler(httpd_req_t *req) {
    char content[128]; // Buffer to hold the URL-encoded payload
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    content[ret] = '\0'; // Null-terminate the received data

    ESP_LOGI("SETPOINTS", "Received data: %s", content);

    // Parse the URL-encoded form data
    char *temp_str = strstr(content, "temp=");
    char *humidity_str = strstr(content, "&humidity=");
    if (temp_str && humidity_str) {
        temp_str += 5; // Move past "temp="
        humidity_str += 10; // Move past "&humidity="

        // Extract temperature and humidity values
        char *temp_end = strchr(temp_str, '&');
        if (temp_end) {
            *temp_end = '\0';
        }
        float set_temp = strtof(temp_str, NULL);

        char *humidity_end = strchr(humidity_str, '&');
        if (humidity_end) {
            *humidity_end = '\0';
        }
        float set_humidity = strtof(humidity_str, NULL);

        ESP_LOGI("SETPOINTS", "Set Temperature: %.2f, Set Humidity: %.2f", set_temp, set_humidity);

        // Send response
        httpd_resp_sendstr(req, "Setpoints updated successfully");

        // Send setpoints to Arduino over TCP
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "temp=%.2f&humidity=%.2f\n", set_temp, set_humidity);
        ESP_LOGI("SETPOINTS", "Sending TCP message: %s", tcp_message);
        send_tcp_message(tcp_message);
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid data");
    }

    return ESP_OK;
}
