#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>  // Ensure standard integer types are available
#include <stdbool.h> // Ensure boolean type is available

#ifdef __cplusplus
extern "C" {
#endif

// Declare global variables
extern float temperature;
extern float humidity;
extern uint16_t lux;  // Ensure this matches the definition elsewhere
extern bool heater;
extern bool dehumidifier;

#ifdef __cplusplus
}
#endif

#endif // GLOBALS_H
