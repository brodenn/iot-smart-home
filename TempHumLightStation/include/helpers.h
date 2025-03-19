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

void clearESPBuffer();
void getResponse(char* buffer, size_t len);
String getResponse();
bool waitForResponse(const char* expected, unsigned long timeout);
bool checkConnection();
void handleSetpoints(const String& data);
void formatSensorData(char* buffer, size_t len, int16_t temperature, int16_t humidity, uint16_t light);

void initializeSerial();
void initializeESP();
void readSetpointsFromEEPROM();
void readWiFiCredentialsFromEEPROM();
void storeDefaultCredentialsIfNeeded();
void initializeSensors();
void handleIncomingMessages();
void checkAndReconnectTCP();
void performHandshakeIfNeeded();
void readSensorData();
void updateAutomationStates();
void sendSensorData();

#endif // HELPERS_H
