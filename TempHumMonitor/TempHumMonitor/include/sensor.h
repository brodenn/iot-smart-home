#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

// Function to read temperature from the Si7021 sensor
float Si7021_ReadTemperature(void);

// Function to read humidity from the Si7021 sensor
float Si7021_ReadHumidity(void);

#endif // SENSOR_H
