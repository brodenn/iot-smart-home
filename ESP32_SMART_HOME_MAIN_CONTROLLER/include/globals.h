#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>  
// Declare the sensor data variables as extern to make them accessible across multiple files
extern float temperature;
extern float humidity;
extern uint16_t lux;
extern bool heater;
extern bool dehumidifier;

#endif // GLOBALS_H
