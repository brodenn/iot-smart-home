/**
 * @file sensor.h
 * @brief This file contains the declarations of sensor functions for the TempHumLightStation project.
 *
 * The functions provided in this file allow for initializing and reading data from the light sensor (using ADC)
 * and the Si7021 temperature and humidity sensor (using I2C).
 *
 * The main functionalities provided by this file include:
 * - Initializing the ADC for reading the light sensor.
 * - Reading the light sensor data and converting it to lux.
 * - Reading temperature and humidity data from the Si7021 sensor.
 *
 * Dependencies:
 * - stdint.h: Standard integer types.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

// -------- ADC Functions --------

/**
 * @brief Initializes the ADC for reading the light sensor.
 *
 * This function configures the ADC to use AVCC as the reference voltage and sets the prescaler to 8.
 */
void ADC_Init(void);

/**
 * @brief Reads the ADC value from the specified channel.
 *
 * This function selects the specified ADC channel, starts the conversion, and waits for it to complete.
 *
 * @param channel The ADC channel to read from.
 * @return The ADC value read from the specified channel.
 */
uint16_t ADC_Read(uint8_t channel);

/**
 * @brief Reads the light sensor data and converts it to lux.
 *
 * This function reads the ADC value from the light sensor, converts it to voltage, and then calculates the lux value.
 *
 * @return The light intensity in lux.
 */
uint16_t LightSensor_ReadLux(void);

// -------- Si7021 Sensor Functions --------

/**
 * @brief Reads the temperature from the Si7021 sensor.
 *
 * This function sends the temperature measurement command to the Si7021 sensor, reads the raw temperature data,
 * and converts it to hundredths of a degree Celsius.
 *
 * @return The temperature in hundredths of a degree Celsius.
 */
int16_t Si7021_ReadTemperature(void);

/**
 * @brief Reads the humidity from the Si7021 sensor.
 *
 * This function sends the humidity measurement command to the Si7021 sensor, reads the raw humidity data,
 * and converts it to hundredths of a percent relative humidity.
 *
 * @return The humidity in hundredths of a percent relative humidity.
 */
int16_t Si7021_ReadHumidity(void);

#endif // SENSOR_H
