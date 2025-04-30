/**
 * @file globals.c
 * @brief This file contains the definitions of global variables and functions for the ESP32 Smart Home Main Controller project.
 *
 * The global variables defined in this file are used for storing sensor readings, device states, and setpoints.
 * The functions provided in this file include logging messages with deduplication.
 *
 * The main functionalities provided by this file include:
 * - Storing sensor readings (temperature, humidity, lux).
 * - Storing device states (heater, dehumidifier).
 * - Storing setpoints for temperature and humidity.
 * - Logging messages with deduplication.
 *
 * Dependencies:
 * - globals.h: Global variables and function declarations.
 * - esp_log.h: ESP32 logging functions.
 * - string.h: String manipulation functions.
 *
 * @note This file is part of the ESP32 Smart Home Main Controller project.
 */

#include "globals.h"
#include "esp_log.h"
#include <string.h>

// Global variables
float temperature = 0.0;       ///< Current temperature reading.
float humidity = 0.0;          ///< Current humidity reading.
uint16_t lux = 0;              ///< Current light intensity reading in lux.
bool heater = false;           ///< Current state of the heater (false: off, true: on).
bool dehumidifier = false;     ///< Current state of the dehumidifier (false: off, true: on).

int client_sock = -1;          ///< Client socket for TCP communication.
bool handshake_done = false;   ///< Indicates if the handshake with the Arduino is completed.

int16_t SP_TEMP = 2500;        ///< Default setpoint temperature: 25.00°C.
int16_t SP_HUM = 5000;         ///< Default setpoint humidity: 50.00%RH.

static char last_log_message[256] = ""; ///< Buffer to store the last logged message for deduplication.


void log_message(const char *tag, const char *message) {
    // Check if the new message is similar to the last logged message
    if (strncmp(message, last_log_message, 100) != 0) {
        ESP_LOGI(tag, "%s", message);
        strncpy(last_log_message, message, sizeof(last_log_message) - 1);
        last_log_message[sizeof(last_log_message) - 1] = '\0';
    }
}
