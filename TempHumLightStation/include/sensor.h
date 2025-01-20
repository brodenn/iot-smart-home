#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

// -------- ADC Functions --------
// Initialize ADC
void ADC_Init(void);

// Read ADC value from a specific channel
uint16_t ADC_Read(uint8_t channel);

// Read light intensity (lux) from the light sensor
uint16_t LightSensor_ReadLux(void);

// -------- Si7021 Sensor Functions --------
// Read temperature as an integer in hundredths of a degree (e.g., 2514 = 25.14°C)
int16_t Si7021_ReadTemperature(void);

// Read humidity as an integer in hundredths of a percent (e.g., 3571 = 35.71%RH)
int16_t Si7021_ReadHumidity(void);

#endif // SENSOR_H
