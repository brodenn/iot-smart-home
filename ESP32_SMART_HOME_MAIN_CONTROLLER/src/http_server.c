/**
 * @file http_server.c
 * @brief This file contains the implementation of the HTTP server functions for the ESP32 Smart Home Main Controller project.
 *
 * The functions provided in this file allow for serving a web interface to control and monitor the smart home system,
 * handling HTTP GET and POST requests, and updating setpoints for temperature and humidity.
 *
 * The main functionalities provided by this file include:
 * - Serving a web interface with real-time data updates.
 * - Handling HTTP GET requests to provide sensor data.
 * - Handling HTTP POST requests to update setpoints.
 * - Sending setpoints to the Arduino over TCP and controlling Shelly devices via HTTP requests.
 *
 * Dependencies:
 * - esp_http_server.h: ESP32 HTTP server functions.
 * - esp_log.h: ESP32 logging functions.
 * - cJSON.h: JSON parsing library.
 * - globals.h: Global variables and definitions.
 * - json_parser.h: JSON parsing helper functions.
 * - tcp_server.h: TCP server function declarations.
 * - shelly_control.h: Shelly device control functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "esp_http_server.h"
#include "esp_log.h"
#include "cJSON.h"
#include "globals.h"
#include "json_parser.h" // Include the header for json_parser
#include "tcp_server.h" // Include this header
#include "shelly_control.h"

static const char *TAG = "HTTP_SERVER";

extern int16_t SP_TEMP; // Declare the external variables
extern int16_t SP_HUM;

esp_err_t get_data_handler(httpd_req_t *req) {
    char buffer[1536]; // Buffer size

    // Send HTML headers with character encoding
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "utf-8");

    // Send HTML content in smaller chunks
    snprintf(buffer, sizeof(buffer),
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset=\"UTF-8\">"
        "<title>ESP32 Smart Home</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; }");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        ".container { max-width: 800px; padding: 20px; background-color: #fff; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); border-radius: 10px; }"
        "h1 { text-align: center; color: #333; }"
        "p { font-size: 18px; color: #666; }"
        ".slider-container { margin: 20px 0; }"
        ".slider-label { display: block; margin-bottom: 10px; font-weight: bold; }"
        ".slider { width: 100%; }");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        ".button { display: block; width: 100%; padding: 10px; background-color: #007bff; color: #fff; text-align: center; border: none; border-radius: 5px; cursor: pointer; }"
        ".button:hover { background-color: #0056b3; }"
        ".status { display: flex; justify-content: space-between; margin: 10px 0; }"
        ".status div { flex: 1; text-align: center; padding: 10px; border-radius: 5px; }"
        ".status .on { background-color: #28a745; color: #fff; }"
        ".status .off { background-color: #dc3545; color: #fff; }"
        ".loading { text-align: center; font-size: 18px; color: #666; display: none; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class=\"container\">"
        "<h1>ESP32 Smart Home</h1>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Temperature
    snprintf(buffer, sizeof(buffer), "<p>Temperature: %.2f &#8451; (SP: <span id=\"spTempValue\">%.2f</span> &#8451;)</p>", temperature, SP_TEMP / 100.0);
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Humidity
    snprintf(buffer, sizeof(buffer), "<p>Humidity: %.2f %% (SP: <span id=\"spHumidityValue\">%.2f</span> %%)</p>", humidity, SP_HUM / 100.0);
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Lux
    snprintf(buffer, sizeof(buffer), "<p>Lux: %d</p>", lux);
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Heater and Dehumidifier states
    snprintf(buffer, sizeof(buffer), 
        "<div class=\"status\">"
        "<div id=\"heaterStatus\" class=\"%s\">Heater: %s</div>"
        "<div id=\"dehumidifierStatus\" class=\"%s\">Dehumidifier: %s</div>"
        "</div>",
        heater ? "on" : "off", heater ? "ON" : "OFF",
        dehumidifier ? "on" : "off", dehumidifier ? "ON" : "OFF");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Sliders
    snprintf(buffer, sizeof(buffer),
        "<div class=\"slider-container\">"
        "<label for=\"tempSlider\" class=\"slider-label\">Set Temperature (&#8451;): </label>"
        "<input type=\"range\" id=\"tempSlider\" class=\"slider\" min=\"0\" max=\"50\" step=\"0.1\" value=\"%.2f\" oninput=\"updateTempValue(this.value)\">"
        "<span id=\"tempValue\">%.2f</span>"
        "</div>", SP_TEMP / 100.0, SP_TEMP / 100.0);
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "<div class=\"slider-container\">"
        "<label for=\"humiditySlider\" class=\"slider-label\">Set Humidity (&#37;): </label>"
        "<input type=\"range\" id=\"humiditySlider\" class=\"slider\" min=\"0\" max=\"100\" step=\"0.1\" value=\"%.2f\" oninput=\"updateHumidityValue(this.value)\">"
        "<span id=\"humidityValue\">%.2f</span>"
        "</div>", SP_HUM / 100.0, SP_HUM / 100.0);
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Button and JavaScript
    snprintf(buffer, sizeof(buffer),
        "<button class=\"button\" onclick=\"sendData()\">Update Setpoints</button>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // Loading message
    snprintf(buffer, sizeof(buffer),
        "<div id=\"loading\" class=\"loading\">Please wait...</div>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "<script>"
        "function updateTempValue(val) {"
        "    document.getElementById('tempValue').innerText = val;"
        "}"
        "function updateHumidityValue(val) {"
        "    document.getElementById('humidityValue').innerText = val;"
        "}");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "function sendData() {"
        "    document.getElementById('loading').style.display = 'block';"
        "    var temp = document.getElementById('tempSlider').value;"
        "    var humidity = document.getElementById('humiditySlider').value;"
        "    var xhr = new XMLHttpRequest();"
        "    xhr.open('POST', '/update', true);"
        "    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');"
        "    xhr.send('temp=' + temp + '&humidity=' + humidity);");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "    xhr.onload = function() {"
        "        if (xhr.status == 200) {"
        "            fetchData();"
        "        }"
        "    };"
        "}");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "</script>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "<script>"
        "function fetchData() {"
        "    var xhr = new XMLHttpRequest();"
        "    xhr.open('GET', '/data', true);"
        "    xhr.onload = function() {"
        "        document.getElementById('loading').style.display = 'none';"
        "        if (xhr.status == 200) {"
        "            var response = JSON.parse(xhr.responseText);"
        "            document.getElementById('spTempValue').innerText = response.sp_temperature.toFixed(2);"
        "            document.getElementById('spHumidityValue').innerText = response.sp_humidity.toFixed(2);"
        "            document.getElementById('heaterStatus').className = response.heater ? 'on' : 'off';"
        "            document.getElementById('heaterStatus').innerText = 'Heater: ' + (response.heater ? 'ON' : 'OFF');"
        "            document.getElementById('dehumidifierStatus').className = response.dehumidifier ? 'on' : 'off';"
        "            document.getElementById('dehumidifierStatus').innerText = 'Dehumidifier: ' + (response.dehumidifier ? 'ON' : 'OFF');"
        "            document.getElementById('tempValue').innerText = response.temperature.toFixed(2);"
        "            document.getElementById('humidityValue').innerText = response.humidity.toFixed(2);"
        "            document.getElementById('luxValue').innerText = response.lux;"
        "        }"
        "    };"
        "    xhr.send();"
        "}"
        "setInterval(fetchData, 5000);" // Fetch data every 5 seconds
        "window.onload = fetchData;" // Fetch data on page load
        "</script>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    snprintf(buffer, sizeof(buffer),
        "</div>"
        "</body>"
        "</html>");
    httpd_resp_send_chunk(req, buffer, strlen(buffer));

    // End the response
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}


esp_err_t get_data_api_handler(httpd_req_t *req) {
    char response[256];
    snprintf(response, sizeof(response), 
        "{\"temperature\":%.2f,\"humidity\":%.2f,\"lux\":%d,\"heater\":%s,\"dehumidifier\":%s,\"sp_temperature\":%.2f,\"sp_humidity\":%.2f}", 
        temperature, humidity, lux, heater ? "true" : "false", dehumidifier ? "true" : "false", SP_TEMP / 100.0, SP_HUM / 100.0);
    httpd_resp_sendstr(req, response);
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

        // Update global setpoints
        SP_TEMP = (int16_t)(set_temp * 100);
        SP_HUM = (int16_t)(set_humidity * 100);

        // Send response
        char response[128];
        snprintf(response, sizeof(response), "{\"heater\":%s,\"dehumidifier\":%s}", heater ? "true" : "false", dehumidifier ? "true" : "false");
        httpd_resp_sendstr(req, response);

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

        httpd_uri_t data_api_uri = {
            .uri = "/data",
            .method = HTTP_GET,
            .handler = get_data_api_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &data_api_uri);

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

