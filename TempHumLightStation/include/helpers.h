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

/**
 * @brief Clears the serial buffer of the ESP8266.
 *
 * This function reads and discards any data available in the ESP8266's serial buffer.
 * It is useful for ensuring that the buffer is empty before sending new commands.
 */
void clearESPBuffer();

/**
 * @brief Reads the full response from the ESP8266 into a fixed-size buffer.
 *
 * This function reads data from the ESP8266's serial buffer into the provided buffer
 * until a timeout occurs or a key response is detected.
 *
 * @param buffer The buffer to store the response.
 * @param len The length of the buffer.
 */
void getResponse(char* buffer, size_t len);

/**
 * @brief Wrapper function to get the response from the ESP8266 as a String.
 *
 * This function calls getResponse() and returns the response as a String.
 *
 * @return The response from the ESP8266 as a String.
 */
String getResponse();

/**
 * @brief Waits for a specific response from the ESP8266.
 *
 * This function waits for the specified response from the ESP8266 within the given timeout period.
 *
 * @param expected The expected response string.
 * @param timeout The timeout period in milliseconds.
 * @return True if the expected response is received, false otherwise.
 */
bool waitForResponse(const char* expected, unsigned long timeout);

/**
 * @brief Checks the TCP connection status with the ESP8266.
 *
 * This function checks if the system is connected to the TCP server. If not, it attempts to reconnect.
 *
 * @return True if connected, false otherwise.
 */
bool checkConnection();

/**
 * @brief Handles incoming data and extracts temperature and humidity setpoints.
 *
 * This function parses the incoming data to extract temperature and humidity setpoints
 * and updates the automation module with the new setpoints.
 *
 * @param data The incoming data containing the setpoints.
 */
void handleSetpoints(const String& data);

/**
 * @brief Generates a JSON string for sensor data with safe formatting.
 *
 * This function formats the sensor data into a JSON string and stores it in the provided buffer.
 *
 * @param buffer The buffer to store the JSON string.
 * @param len The length of the buffer.
 * @param temperature The temperature reading (in tenths of degrees Celsius).
 * @param humidity The humidity reading (in tenths of percent relative humidity).
 * @param light The light intensity reading (in lux).
 */
void formatSensorData(char* buffer, size_t len, int16_t temperature, int16_t humidity, uint16_t light);

#endif // HELPERS_H
