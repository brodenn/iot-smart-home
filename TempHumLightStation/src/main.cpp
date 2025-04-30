/**
 * @file main.cpp
 * @brief This file contains the main setup and loop functions for the TempHumLightStation project.
 *
 * The main functionalities provided by this file include:
 * - Initializing the system components such as serial communication, Wi-Fi, I2C, and automation.
 * - Reading setpoints from EEPROM and setting them in the automation module.
 * - Continuously monitoring and updating the system states based on sensor readings.
 * - Handling TCP communication with the server.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 * - wifi_tcp.h: Header file containing the declarations of Wi-Fi TCP functions.
 * - globals.h: Header file containing the declarations of global variables.
 * - automation.h: Header file containing the declarations of automation functions.
 * - sensor.h: Header file containing the declarations of sensor functions.
 * - i2c.h: Header file containing the declarations of I2C functions.
 * - helpers.h: Header file containing the declarations of helper functions.
 * - wifi_handshake.h: Header file containing the declarations of Wi-Fi handshake functions.
 * - eeprom.h: Header file containing the declarations of EEPROM functions.
 * - eeprom_addresses.h: Header file containing the EEPROM address definitions.
 * - wifi_commands.h: Header file containing the declarations of Wi-Fi command functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "../include/wifi_tcp.h"
#include "../include/globals.h"
#include "../include/automation.h"
#include "../include/sensor.h"
#include "../include/i2c.h"
#include "../include/helpers.h"
#include "../include/wifi_handshake.h"
#include "../include/eeprom.h"
#include "../include/wifi_commands.h"

void setup() {
    initializeSerial();
    initializeESP();
    readSetpointsFromEEPROM();
    readWiFiCredentialsFromEEPROM();
    storeDefaultCredentialsIfNeeded();
    initializeWiFiAndTCP();
    initializeSensors();
    Automation_Init(); 
}

void loop() {
    handleIncomingMessages();
    checkAndReconnectTCP();
    performHandshakeIfNeeded();
    readSensorData();
    updateAutomationStates();
    sendSensorData();
}
