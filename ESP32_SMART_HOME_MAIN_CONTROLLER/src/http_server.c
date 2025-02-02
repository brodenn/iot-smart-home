#include "esp_http_server.h"
#include "globals.h"
#include "esp_log.h"
#include "cJSON.h"

#define TAG "HTTP"


float temperature = 0.0;
float humidity = 0.0;
uint16_t lux = 0;  // Use `uint16_t` to match `globals.h`
bool heater = false;
bool dehumidifier = false;


esp_err_t get_data_handler(httpd_req_t *req) {
    char buffer[128]; // Buffer for smaller chunks

    // Send HTML headers
    httpd_resp_send_chunk(req, "<html><body><h1>ESP32 Smart Home</h1>", HTTPD_RESP_USE_STRLEN);

    // Temperature
    snprintf(buffer, sizeof(buffer), "<p>Temperature: %.2f °C</p>", temperature);
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
        "<label for=\"tempSlider\">Set Temperature (°C): </label>"
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
        "    fetch('/setpoints', {"
        "        method: 'POST',"
        "        headers: { 'Content-Type': 'application/json' },"
        "        body: JSON.stringify({ temperature: temp, humidity: humidity })"
        "    }).then(response => response.json())"
        "      .then(data => { alert('Setpoints updated!'); });"
        "}"
        "</script>"
        "</body></html>",
        HTTPD_RESP_USE_STRLEN);

    // End of response
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

esp_err_t setpoints_handler(httpd_req_t *req) {
    char content[128]; // Buffer to hold the JSON payload
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret <= 0) { // Error or client closed connection
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req); // Send timeout response
        }
        return ESP_FAIL;
    }
    content[ret] = '\0'; // Null-terminate the received data

    ESP_LOGI("SETPOINTS", "Received JSON: %s", content);  // Debug log

    // Parse JSON data
    cJSON *json = cJSON_Parse(content);
    if (!json) {
        ESP_LOGE("SETPOINTS", "Failed to parse JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    // Extract temperature and humidity values
    cJSON *temp_item = cJSON_GetObjectItem(json, "temperature");
    cJSON *humidity_item = cJSON_GetObjectItem(json, "humidity");

    if (temp_item && humidity_item) {
        ESP_LOGI("SETPOINTS", "Parsed values -> Temperature: %.2f, Humidity: %.2f",
                 temp_item->valuedouble, humidity_item->valuedouble);

        // Update global variables
        temperature = temp_item->valuedouble;
        humidity = humidity_item->valuedouble;

        ESP_LOGI("SETPOINTS", "Updated -> Temperature: %.2f, Humidity: %.2f", temperature, humidity);

        // Send success response
        httpd_resp_set_type(req, "application/json");
        httpd_resp_sendstr(req, "{\"status\":\"success\"}");
    } else {
        ESP_LOGE("SETPOINTS", "Missing JSON fields");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing fields");
    }

    cJSON_Delete(json); // Clean up JSON object
    return ESP_OK;
}


void start_http_server(void) {
    httpd_handle_t server = NULL;

    // Configure the HTTP server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI("HTTP", "Starting HTTP server...");

        // Register the URI handler for the root path "/"
        httpd_uri_t data_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = get_data_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &data_uri);

        // Register the URI handler for "/setpoints"
        httpd_uri_t setpoints_uri = {
            .uri = "/setpoints",
            .method = HTTP_POST,
            .handler = setpoints_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &setpoints_uri);
    } else {
        ESP_LOGE("HTTP", "Failed to start HTTP server");
    }
}
