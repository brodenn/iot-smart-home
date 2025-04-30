/**
 * @file automation.cpp
 * @brief This file contains the implementation of the automation system for controlling the heater and dehumidifier.
 *
 * The functions provided in this file allow for initializing the automation system and updating the states of the heater
 * and dehumidifier based on the current temperature and humidity readings. The automation system uses hysteresis to avoid
 * frequent switching of the heater and dehumidifier.
 *
 * The main functionalities provided by this file include:
 * - Initializing the automation system.
 * - Updating the state of the heater based on the current temperature.
 * - Updating the state of the dehumidifier based on the current humidity.
 *
 * Dependencies:
 * - automation.h: Automation function declarations.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/automation.h"

// Setpoints and hysteresis values
int16_t SP_TEMP = 2200;       ///< Default setpoint temperature: 22.00°C
int16_t SP_TEMP_HYS = 100;    ///< Hysteresis: 1.00°C
int16_t SP_HUM = 5000;        ///< Default setpoint humidity: 50.00%RH
int16_t SP_HUM_HYS = 200;     ///< Hysteresis: 2.00%RH

// States for heater and dehumidifier
static uint8_t heaterState = 0;
static uint8_t dehumidifierState = 0;

void Automation_Init(void) {
    heaterState = 0;
    dehumidifierState = 0;
}

void UpdateHeaterState(int16_t temperature) {
    if (heaterState == 0 && temperature < (SP_TEMP - SP_TEMP_HYS)) {
        heaterState = 1;
    } else if (heaterState == 1 && temperature > (SP_TEMP + SP_TEMP_HYS)) {
        heaterState = 0;
    }
}

void UpdateDehumidifierState(int16_t humidity) {
    if (dehumidifierState == 0 && humidity > (SP_HUM + SP_HUM_HYS)) {
        dehumidifierState = 1;
    } else if (dehumidifierState == 1 && humidity < (SP_HUM - SP_HUM_HYS)) {
        dehumidifierState = 0;
    }
}

void Automation_Update(int16_t temperature, int16_t humidity) {
    UpdateHeaterState(temperature);
    UpdateDehumidifierState(humidity);
}

uint8_t GetHeaterState(void) {
    return heaterState;
}

uint8_t GetDehumidifierState(void) {
    return dehumidifierState;
}

void Automation_SetSetpoints(int16_t temp, int16_t hum) {
    SP_TEMP = temp;
    SP_HUM = hum;
}