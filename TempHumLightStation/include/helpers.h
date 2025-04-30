/**
 * @file helpers.h
 * @brief This file contains the declarations of helper functions for the TempHumLightStation project.
 *
 * The helper functions declared in this file are used for various tasks such as clearing the ESP8266 serial buffer,
 * reading responses from the ESP8266, checking TCP connection status, handling incoming data, and formatting sensor data.
 *
 * The main functionalities provided by this file include:
 * - Clearing the ESP8266 serial buffer.
 * - Reading the full response from the ESP8266 into a fixed-size buffer.
 * - Waiting for a specific response from the ESP8266.
 * - Checking the TCP connection status with the ESP8266.
 * - Handling incoming data and extracting temperature and humidity setpoints.
 * - Generating a JSON string for sensor data with safe formatting.
 * - Initializing serial communication and the ESP8266 module.
 * - Reading and storing Wi-Fi credentials in EEPROM.
 * - Initializing sensors and reading sensor data.
 * - Handling incoming TCP messages and performing server handshake.
 * - Updating automation states and sending sensor data to the server.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

// Function declarations

/**
 * @brief Clears the serial buffer of the ESP8266.
 */
void clearESPBuffer();

/**
 * @brief Reads the full response from the ESP8266 into a fixed-size buffer.
 *
 * @param buffer The buffer to store the response.
 * @param len The length of the buffer.
 */
void getResponse(char* buffer, size_t len);

/**
 * @brief Wrapper function to get the response from the ESP8266 as a String.
 *
 * @return The response from the ESP8266 as a String.
 */
String getResponse();

/**
 * @brief Waits for a specific response from the ESP8266.
 *
 * @param expected The expected response string.
 * @param timeout The timeout period in milliseconds.
 * @return True if the expected response is received, false otherwise.
 */
bool waitForResponse(const char* expected, unsigned long timeout);

/**
 * @brief Checks the TCP connection status with the ESP8266.
 *
 * @return True if connected, false otherwise.
 */
bool checkConnection();

/**
 * @brief Handles incoming data and extracts temperature and humidity setpoints.
 *
 * @param data The incoming data containing the setpoints.
 */
void handleSetpoints(const String& data);

/**
 * @brief Generates a JSON string for sensor data with safe formatting.
 *
 * @param buffer The buffer to store the JSON string.
 * @param len The length of the buffer.
 * @param temperature The temperature reading (in tenths of degrees Celsius).
 * @param humidity The humidity reading (in tenths of percent relative humidity).
 * @param light The light intensity reading (in lux).
 */
void formatSensorData(char* buffer, size_t len, int16_t temperature, int16_t humidity, uint16_t light);

/**
 * @brief Initializes the serial communication for debugging.
 */
void initializeSerial();

/**
 * @brief Initializes the ESP8266 module.
 */
void initializeESP();

/**
 * @brief Reads the temperature and humidity setpoints from EEPROM.
 */
void readSetpointsFromEEPROM();

/**
 * @brief Reads the Wi-Fi credentials from EEPROM.
 */
void readWiFiCredentialsFromEEPROM();

/**
 * @brief Stores default Wi-Fi credentials in EEPROM if none are found.
 */
void storeDefaultCredentialsIfNeeded();

/**
 * @brief Initializes the sensors.
 */
void initializeSensors();

/**
 * @brief Handles incoming TCP messages.
 */
void handleIncomingMessages();

/**
 * @brief Checks and reconnects to the TCP server if needed.
 */
void checkAndReconnectTCP();

/**
 * @brief Performs a handshake with the server if needed.
 */
void performHandshakeIfNeeded();

/**
 * @brief Reads sensor data.
 */
void readSensorData();

/**
 * @brief Updates the automation states based on sensor data.
 */
void updateAutomationStates();

/**
 * @brief Sends sensor data to the server.
 */
void sendSensorData();

#endif // HELPERS_H
