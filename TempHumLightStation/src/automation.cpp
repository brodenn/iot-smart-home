/**
 * @file automation.cpp
 * @brief This file contains the implementation of the automation system for temperature and humidity control.
 *
 * The automation system is responsible for managing the states of a heater and a dehumidifier based on
 * the current temperature and humidity readings. It uses setpoints and hysteresis to avoid frequent switching
 * of the devices.
 *
 * The main functionalities provided by this file include:
 * - Initialization of the automation system.
 * - Updating the states of the heater and dehumidifier based on the current temperature and humidity.
 * - Retrieving the current states of the heater and dehumidifier.
 * - Setting new setpoints for temperature and humidity and storing them in EEPROM.
 *
 * The file includes the following functions:
 * - void Automation_Init(void): Initializes the automation system.
 * - void Automation_Update(int16_t temperature, int16_t humidity): Updates the states of the heater and dehumidifier.
 * - uint8_t GetHeaterState(void): Gets the current state of the heater.
 * - uint8_t GetDehumidifierState(void): Gets the current state of the dehumidifier.
 * - void Automation_SetSetpoints(int16_t temp, int16_t hum): Sets new setpoints for temperature and humidity.
 *
 * Dependencies:
 * - uart.h: UART communication functions.
 * - eeprom.h: EEPROM read/write functions.
 * - eeprom_addresses.h: EEPROM address definitions.
 * - Arduino.h: Arduino core functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/automation.h"
#include "../include/eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

// Variables for setpoints and hysteresis
int16_t SP_TEMP = 2500;       // Default setpoint temperature: 25.00°C
int16_t SP_TEMP_HYS = 100;    // Hysteresis: 1.00°C
int16_t SP_HUM = 5000;        // Default setpoint humidity: 50.00%RH
int16_t SP_HUM_HYS = 200;     // Hysteresis: 2.00%RH

// States for heater and dehumidifier
static uint8_t heaterState = 0;
static uint8_t dehumidifierState = 0;

/**
 * @brief Initializes the automation system.
 * 
 * This function sets the initial states of the heater and dehumidifier to off.
 * It should be called during the system startup to ensure that the automation
 * components are in a known state.
 */
void Automation_Init(void) {
    heaterState = 0;
    dehumidifierState = 0;
}

/**
 * @brief Updates the state of the heater and dehumidifier based on the current temperature and humidity.
 *
 * This function implements a simple thermostat and humidity control logic with hysteresis to avoid frequent switching.
 *
 * @param temperature The current temperature reading (in tenths of degrees Celsius).
 * @param humidity The current humidity reading (in tenths of percent relative humidity).
 *
 * Thermostat logic:
 * - If the heater is off and the temperature drops below the set point minus the hysteresis, the heater is turned on.
 * - If the heater is on and the temperature rises above the set point plus the hysteresis, the heater is turned off.
 *
 * Humidity control logic:
 * - If the dehumidifier is off and the humidity drops below the set point minus the hysteresis, the dehumidifier is turned on.
 * - If the dehumidifier is on and the humidity rises above the set point plus the hysteresis, the dehumidifier is turned off.
 */
void Automation_Update(int16_t temperature, int16_t humidity) {

    // Thermostat logic with hysteresis
    if (heaterState == 0 && temperature < (SP_TEMP - SP_TEMP_HYS)) {
        heaterState = 1;
    } else if (heaterState == 1 && temperature > (SP_TEMP + SP_TEMP_HYS)) {
        heaterState = 0;
    }

    // Humidity control logic with hysteresis
    if (dehumidifierState == 0 && humidity < (SP_HUM - SP_HUM_HYS)) {
        dehumidifierState = 1;
    } else if (dehumidifierState == 1 && humidity > (SP_HUM + SP_HUM_HYS)) {
        dehumidifierState = 0;
    }
}

/**
 * @brief Gets the current state of the heater.
 *
 * @return The current state of the heater (0: off, 1: on).
 */
uint8_t GetHeaterState(void) {
    return heaterState;
}

/**
 * @brief Gets the current state of the dehumidifier.
 *
 * @return The current state of the dehumidifier (0: off, 1: on).
 */
uint8_t GetDehumidifierState(void) {
    return dehumidifierState;
}

/**
 * @brief Sets the temperature and humidity setpoints.
 *
 * This function updates the setpoints for temperature and humidity and stores them in EEPROM.
 *
 * @param temp The new temperature setpoint (in tenths of degrees Celsius).
 * @param hum The new humidity setpoint (in tenths of percent relative humidity).
 */
void Automation_SetSetpoints(int16_t temp, int16_t hum) {
    SP_TEMP = temp; // Set the temperature setpoint directly
    SP_HUM = hum;   // Set the humidity setpoint directly

    // Store the setpoints in EEPROM
    eeprom_write_word(EEPROM_TEMP_ADDR, temp);
    eeprom_write_word(EEPROM_HUM_ADDR, hum);
}