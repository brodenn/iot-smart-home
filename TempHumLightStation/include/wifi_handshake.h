/**
 * @file wifi_handshake.h
 * @brief This file contains the declarations of Wi-Fi handshake functions for the ESP8266 module.
 *
 * The functions provided in this file allow for performing a handshake with the ESP32 server,
 * checking if the ESP8266 is ready, sending TCP messages, and receiving TCP messages.
 *
 * The main functionalities provided by this file include:
 * - Performing a handshake with the ESP32 server.
 * - Checking if the ESP8266 is ready.
 * - Sending TCP messages.
 * - Receiving TCP messages.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef WIFI_HANDSHAKE_H
#define WIFI_HANDSHAKE_H

#include <Arduino.h>

// Function declarations

/**
 * @brief Checks if the ESP8266 is ready.
 *
 * This function sends the "AT" command to the ESP8266 and waits for an "OK" response.
 * If the ESP8266 is not ready, it resets the ESP8266.
 *
 * @return True if the ESP8266 is ready, false otherwise.
 */
bool isESP8266Ready();

/**
 * @brief Performs a handshake with the ESP32 server.
 *
 * This function sends a handshake message to the ESP32 server and waits for a response.
 * If the handshake is successful, it sets the handshake_done flag to true.
 *
 * @return True if the handshake is successful, false otherwise.
 */
bool performHandshake();

/**
 * @brief Attempts to perform a handshake with the ESP32 server.
 *
 * This function sends a handshake message to the ESP32 server and waits for a response.
 * If the handshake is successful, it sets the handshake_done flag to true.
 *
 * @return True if the handshake is successful, false otherwise.
 */
bool attemptHandshake();

/**
 * @brief Waits for a handshake response from the ESP32 server.
 *
 * This function waits for a handshake response from the ESP32 server within a specified timeout period.
 *
 * @return True if the handshake response is received, false otherwise.
 */
bool waitForHandshakeResponse();

/**
 * @brief Sends a TCP message to the server.
 *
 * This function sends a TCP message to the server. If the message is not acknowledged,
 * it retries up to three times. If the handshake fails, it performs the handshake again.
 *
 * @param message The message to send.
 */
void sendTCPMessage(const char* message);

/**
 * @brief Attempts to send a TCP message to the server.
 *
 * This function attempts to send a TCP message to the server and waits for an acknowledgment.
 *
 * @param fullMessage The full message to send.
 * @return True if the message is acknowledged, false otherwise.
 */
bool attemptSendMessage(const char* fullMessage);

/**
 * @brief Receives and processes TCP messages from the server.
 *
 * This function receives TCP messages from the server and processes them.
 * If the message contains setpoints, it updates the setpoints in the automation module.
 */
void receiveTCPMessage();

/**
 * @brief Processes incoming TCP messages from the server.
 *
 * This function processes incoming TCP messages from the server and updates the setpoints if necessary.
 */
void processIncomingMessage();

#endif // WIFI_HANDSHAKE_H
