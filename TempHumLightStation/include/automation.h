/**
 * @file automation.h
 * @brief This file contains the declarations of the automation functions for the TempHumLightStation project.
 *
 * The functions provided in this file allow for initializing the automation system, updating the states of the heater
 * and dehumidifier based on temperature and humidity readings, retrieving the current states of the heater and dehumidifier,
 * and setting new setpoints for temperature and humidity.
 *
 * The main functionalities provided by this file include:
 * - Initializing the automation system.
 * - Updating the states of the heater and dehumidifier.
 * - Retrieving the current states of the heater and dehumidifier.
 * - Setting new setpoints for temperature and humidity.
 *
 * Dependencies:
 * - stdint.h: Standard integer types.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <stdint.h>

// Declare setpoint variables as extern
extern int16_t SP_TEMP; ///< Temperature setpoint in hundredths of degrees Celsius.
extern int16_t SP_HUM;  ///< Humidity setpoint in hundredths of percent relative humidity.

/**
 * @brief Initializes the automation system.
 *
 * This function sets the initial states of the heater and dehumidifier to off.
 * It should be called during the system startup to ensure that the automation
 * components are in a known state.
 */
void Automation_Init(void);

/**
 * @brief Updates the state of the heater and dehumidifier based on the current temperature and humidity.
 *
 * This function implements a simple thermostat and humidity control logic with hysteresis to avoid frequent switching.
 *
 * @param temperature The current temperature reading (in hundredths of degrees Celsius).
 * @param humidity The current humidity reading (in hundredths of percent relative humidity).
 */
void Automation_Update(int16_t temperature, int16_t humidity);

/**
 * @brief Gets the current state of the heater.
 *
 * @return The current state of the heater (0: off, 1: on).
 */
uint8_t GetHeaterState(void);

/**
 * @brief Gets the current state of the dehumidifier.
 *
 * @return The current state of the dehumidifier (0: off, 1: on).
 */
uint8_t GetDehumidifierState(void);

/**
 * @brief Sets the temperature and humidity setpoints.
 *
 * This function updates the setpoints for temperature and humidity and stores them in EEPROM.
 *
 * @param temp The new temperature setpoint (in hundredths of degrees Celsius).
 * @param hum The new humidity setpoint (in hundredths of percent relative humidity).
 */
void Automation_SetSetpoints(int16_t temp, int16_t hum);

#endif // AUTOMATION_H
