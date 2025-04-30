/**
 * @file globals.h
 * @brief This header file contains global variable declarations for the ESP32 Smart Home Main Controller project.
 *
 * The variables declared here are used across multiple source files to manage
 * temperature, humidity, light intensity, and device states, as well as network communication.
 *
 * @note Ensure proper synchronization when accessing these variables in a multithreaded environment.
 */
#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern float temperature;
extern float humidity;
extern uint16_t lux;
extern bool heater;
extern bool dehumidifier;

extern int client_sock;
extern bool handshake_done;

extern int16_t SP_TEMP; // Declare the external variables
extern int16_t SP_HUM;

#ifdef __cplusplus
}
#endif

#endif // GLOBALS_H
