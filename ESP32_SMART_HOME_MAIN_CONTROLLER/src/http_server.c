#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "globals.h"
#include "json_parser.h" // Include the header for json_parser

static const char *TAG = "HTTP_SERVER";

esp_err_t get_data_handler(httpd_req_t *req) {
    char buffer[128]; // Buffer for smaller chunks

    // Send HTML headers
    httpd_resp_send_chunk(req, "<html><body><h1>ESP32 Smart Home</h1>", HTTPD_RESP_USE_STRLEN);

    // Temperature
    snprintf(buffer, sizeof(buffer), "<p>Temperature: %.2f \u00b0C</p>", temperature);
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
        "<label for=\"tempSlider\">Set Temperature (\u00b0C): </label>"
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

