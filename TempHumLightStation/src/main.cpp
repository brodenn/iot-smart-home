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

/**
 * @brief Sets up the system components.
 *
 * This function initializes the serial communication, reads setpoints from EEPROM,
 * initializes the Wi-Fi and TCP communication, and sets up the I2C and automation modules.
 */
void setup() {
    Serial.begin(9600);
    delay(5000);  // Wait for serial communication to stabilize

    espSerial.begin(9600);
    delay(7000);  // Wait for ESP8266 to initialize

    enableEcho();  // Enable echo on the ESP8266

    // Read Setpoints from EEPROM
    int16_t storedTemp = eeprom_read_word(EEPROM_TEMP_ADDR);
    int16_t storedHum = eeprom_read_word(EEPROM_HUM_ADDR);
    Automation_SetSetpoints(storedTemp, storedHum);

    initializeWiFiAndTCP();  // Initialize Wi-Fi and perform handshake

    ADC_Init();  // Initialize ADC
    I2C_Init();  // Initialize I2C
    Automation_Init();  // Initialize automation system
}

/**
 * @brief Main loop function.
 *
 * This function continuously monitors the system states, handles incoming TCP messages,
 * checks the TCP connection status, performs handshake if needed, and updates the system
 * states based on sensor readings.
 */
void loop() {
    if (espSerial.available()) {
        receiveTCPMessage();  // Handle incoming TCP messages
    }

    static unsigned long lastCheck = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= 30000) {  // Perform actions every 30 seconds
        lastCheck = currentMillis;

        if (!checkConnection()) {
            connectToTCPServer();  // Reconnect to TCP server if connection is lost
        }

        if (!handshake_done) {
            performHandshake();  // Perform handshake if not done
        }

        if (handshake_done) {
            int16_t temperature = Si7021_ReadTemperature();  // Read temperature
            int16_t humidity = Si7021_ReadHumidity();  // Read humidity
            uint16_t light = LightSensor_ReadLux();  // Read light intensity

            Automation_Update(temperature, humidity);  // Update automation states

            // Use a buffer instead of `String`
            char sensorData[128];
            formatSensorData(sensorData, sizeof(sensorData), temperature, humidity, light);

            sendTCPMessage(sensorData);  // Send sensor data to the server
        }
    }
}
