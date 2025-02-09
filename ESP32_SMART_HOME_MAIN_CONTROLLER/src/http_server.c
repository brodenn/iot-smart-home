#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "globals.h"
#include "json_parser.h" // Include the header for json_parser
#include "tcp_server.h" // Include this header
#include "shelly_control.h"

static const char *TAG = "HTTP_SERVER";

esp_err_t get_data_handler(httpd_req_t *req) {
    char buffer[128]; // Buffer for smaller chunks

    // Send HTML headers with character encoding
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "utf-8");

    // Send HTML content
    httpd_resp_send_chunk(req, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>ESP32 Smart Home</title></head><body><h1>ESP32 Smart Home</h1>", HTTPD_RESP_USE_STRLEN);

    // Temperature
    snprintf(buffer, sizeof(buffer), "<p>Temperature: %.2f &#8451;</p>", temperature);
    httpd_resp_send_chunk(req, buffer, HTTPD_RESP_USE_STRLEN);

    // Humidity
    snprintf(buffer, sizeof(buffer), "<p>Humidity: %.2f %%</p>", humidity);
    httpd_resp_send_chunk(req, buffer, HTTPD_RESP_USE_STRLEN);

    // Lux
    snprintf(buffer, sizeof(buffer), "<p>Lux: %d</p>", lux);
    httpd_resp_send_chunk(req, buffer, HTTPD_RESP_USE_STRLEN);

    // Heater and Dehumidifier states
    snprintf(buffer, sizeof(buffer), "<p>Heater: %s</p>", heater ? "ON" : "OFF");
    httpd_resp_send_chunk(req, buffer, HTTPD_RESP_USE_STRLEN);

    snprintf(buffer, sizeof(buffer), "<p>Dehumidifier: %s</p>", dehumidifier ? "ON" : "OFF");
    httpd_resp_send_chunk(req, buffer, HTTPD_RESP_USE_STRLEN);

    // Sliders and JavaScript
    httpd_resp_send_chunk(req,
        "<br><br>"
        "<label for=\"tempSlider\">Set Temperature (&#8451;): </label>"
        "<input type=\"range\" id=\"tempSlider\" min=\"0\" max=\"50\" step=\"0.1\" "
        "value=\"0\" oninput=\"updateTempValue(this.value)\">"
        "<span id=\"tempValue\">0</span><br><br>"
        "<label for=\"humiditySlider\">Set Humidity (%): </label>"
        "<input type=\"range\" id=\"humiditySlider\" min=\"0\" max=\"100\" step=\"0.1\" "
        "value=\"0\" oninput=\"updateHumidityValue(this.value)\">"
        "<span id=\"humidityValue\">0</span><br><br>"
        "<button onclick=\"sendData()\">Update Setpoints</button>"
        "<script>"
        "function updateTempValue(val) {"
        "    document.getElementById('tempValue').innerText = val;"
        "}"
        "function updateHumidityValue(val) {"
        "    document.getElementById('humidityValue').innerText = val;"
        "}"
        "function sendData() {"
        "    var temp = document.getElementById('tempSlider').value;"
        "    var humidity = document.getElementById('humiditySlider').value;"
        "    var xhr = new XMLHttpRequest();"
        "    xhr.open('POST', '/update', true);"
        "    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
        "    xhr.send('temp=' + temp + '&humidity=' + humidity);"
        "}"
        "</script>"
        "</body></html>", HTTPD_RESP_USE_STRLEN);

    // End the response
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
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

        // Send setpoints to Arduino over TCP and wait for acknowledgment
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "temp=%.2f&humidity=%.2f\n", set_temp, set_humidity);
        ESP_LOGI("SETPOINTS", "Sending TCP message: %s", tcp_message);
        send_setpoints_with_ack(tcp_message);

        // Control Shelly devices based on setpoints
        if (set_temp > 25.0) { // Example condition to turn on the heater
            send_http_request(heaterIP, true);
        } else {
            send_http_request(heaterIP, false);
        }

        if (set_humidity > 50.0) { // Example condition to turn on the dehumidifier
            send_http_request(humidifierIP, true);
        } else {
            send_http_request(humidifierIP, false);
        }
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid data");
    }

    return ESP_OK;
}

void start_http_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Starting HTTP server...");

        httpd_uri_t data_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = get_data_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &data_uri);

        httpd_uri_t setpoints_uri = {
            .uri = "/update",
            .method = HTTP_POST,
            .handler = setpoints_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &setpoints_uri);
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}

