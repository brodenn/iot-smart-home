#include "esp_http_server.h"
#include "json_parser.h"
#include "globals.h"
#include "esp_log.h"

// Handler to serve the main web page
esp_err_t get_data_handler(httpd_req_t *req) {
    char response[512];
    snprintf(response, sizeof(response),
             "<html><head><meta charset=\"UTF-8\"></head><body><h1>ESP32 Smart Home</h1>"
             "<p>Temperature: %.2f &#176;C</p>"
             "<p>Humidity: %.2f %%</p>"
             "<p>Lux: %d</p>"
             "<p>Heater: %s</p>"
             "<p>Dehumidifier: %s</p>"
             "</body></html>",
             temperature, humidity, lux,
             heater ? "ON" : "OFF", dehumidifier ? "ON" : "OFF");

    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// Function to start the HTTP server
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

        // Register the URI handler with the server
        httpd_register_uri_handler(server, &data_uri);
    } else {
        ESP_LOGE("HTTP", "Failed to start HTTP server");
    }
}
