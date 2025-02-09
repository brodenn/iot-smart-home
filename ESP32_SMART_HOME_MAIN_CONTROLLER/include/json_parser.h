#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "esp_err.h"
#include "esp_http_server.h"

void parse_json(const char* json_data);

#endif // JSON_PARSER_H