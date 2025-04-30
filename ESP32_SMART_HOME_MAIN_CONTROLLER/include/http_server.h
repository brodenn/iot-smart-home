/**
 * @file http_server.h
 * @brief Header file for the HTTP server functionality in the ESP32 Smart Home Main Controller.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_http_server.h"

/**
 * @brief Handles HTTP requests for setpoints.
 *
 * @param req Pointer to the HTTP request structure.
 * @return ESP_OK on success, or an appropriate error code.
 */
esp_err_t setpoints_handler(httpd_req_t *req);

/**
 * @brief Starts the HTTP server.
 */
void start_http_server(void);

#endif // HTTP_SERVER_H