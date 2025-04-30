/**
 * @file json_parser.h
 * @brief Header file for JSON parsing functionality in the ESP32 Smart Home Main Controller project.
 *
 * This file contains the declaration of functions used for parsing JSON data.
 */

#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "esp_err.h"
#include "esp_http_server.h"

/**
 * @brief Parses the provided JSON data.
 *
 * This function processes the input JSON string and extracts relevant information.
 *
 * @param json_data A pointer to the JSON string to be parsed.
 */
void parse_json(const char* json_data);

#endif // JSON_PARSER_H