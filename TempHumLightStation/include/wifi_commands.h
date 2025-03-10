/**
 * @file wifi_commands.h
 * @brief This file contains the declarations of Wi-Fi command functions for the ESP8266 module.
 *
 * The functions provided in this file allow for resetting the ESP8266, connecting to a Wi-Fi network,
 * sending AT commands with retries, and checking the Wi-Fi connection status.
 *
 * The main functionalities provided by this file include:
 * - Resetting the ESP8266 module.
 * - Connecting to a Wi-Fi network.
 * - Sending AT commands with retries.
 * - Checking the Wi-Fi connection status.
 * - Enabling command echo on the ESP8266.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef WIFI_COMMANDS_H
#define WIFI_COMMANDS_H

#include <Arduino.h>

// Function declarations

/**
 * @brief Resets the ESP8266 and ensures it's ready before continuing.
 *
 * This function sends the reset command to the ESP8266, waits for it to reset, and then checks if it is ready.
 * If the ESP8266 is not connected to Wi-Fi, it attempts to connect.
 */
void resetESP8266();

/**
 * @brief Connects to Wi-Fi (only if necessary).
 *
 * This function checks if the ESP8266 is already connected to a Wi-Fi network. If not, it sends the SSID and password
 * to connect to the specified Wi-Fi network.
 *
 * @return True if connected, false otherwise.
 */
bool connectToWiFi();

/**
 * @brief Executes AT commands with retries.
 *
 * This function sends an AT command to the ESP8266 and waits for a response. If the response indicates that the ESP8266
 * is busy, it retries the command up to three times. If the command fails, it resets the ESP8266.
 *
 * @param command The AT command to send.
 * @param timeout The timeout period in milliseconds.
 * @return True if the command is successful, false otherwise.
 */
bool sendATCommand(const char* command, unsigned long timeout);

/**
 * @brief Enables command echo on the ESP8266.
 *
 * This function sends the "ATE1" command to the ESP8266 to enable command echo.
 */
void enableEcho();

/**
 * @brief Clears the serial buffer of the ESP8266.
 *
 * This function reads and discards any data available in the ESP8266's serial buffer.
 * It is useful for ensuring that the buffer is empty before sending new commands.
 */
void clearESPBuffer();

/**
 * @brief Wrapper function to get the response from the ESP8266 as a String.
 *
 * This function calls getResponse() and returns the response as a String.
 *
 * @return The response from the ESP8266 as a String.
 */
String getResponse();

/**
 * @brief Waits for the ESP8266 to be ready.
 *
 * This function sends the "AT" command to the ESP8266 and waits for an "OK" response within the specified timeout period.
 *
 * @param timeout The timeout period in milliseconds.
 * @return True if the ESP8266 is ready, false otherwise.
 */
bool waitForESPReady(unsigned long timeout);

/**
 * @brief Checks the Wi-Fi connection status based on the AT+CWJAP? response.
 *
 * This function sends the "AT+CWJAP?" command to the ESP8266 and checks if it is connected to a Wi-Fi network.
 *
 * @return True if connected, false otherwise.
 */
bool checkWiFiConnection();

#endif // WIFI_COMMANDS_H
