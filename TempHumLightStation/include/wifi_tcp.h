/**
 * @file wifi_tcp.h
 * @brief This file contains the declarations of Wi-Fi TCP functions for the ESP8266 module.
 *
 * The functions provided in this file allow for initializing Wi-Fi and TCP connections,
 * connecting to a TCP server, and handling TCP communication with the server.
 *
 * The main functionalities provided by this file include:
 * - Initializing Wi-Fi and TCP connections.
 * - Connecting to a TCP server.
 * - Handling TCP communication with the server.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef WIFI_TCP_H
#define WIFI_TCP_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Wi-Fi credentials
extern const char* ssid;       ///< Wi-Fi SSID.
extern const char* password;   ///< Wi-Fi password.
extern const char* serverIP;   ///< Server IP address.
extern const int serverPort;   ///< Server port number.

// Create a SoftwareSerial object for ESP8266 communication
extern SoftwareSerial espSerial;  ///< SoftwareSerial interface for communication with the ESP8266.

// Global variables
extern bool handshake_done;            ///< Indicates if the handshake with the server is completed.
extern bool serialBusy;                ///< Indicates if the serial communication is currently busy.
extern bool connected;                 ///< Indicates if the system is connected to the Wi-Fi network.
extern String latestResponse;          ///< Stores the latest response received from the ESP8266.
extern String accumulatedResponse;     ///< Stores the accumulated responses from the ESP8266.

// Function declarations

/**
 * @brief Initializes Wi-Fi and TCP connections.
 *
 * This function resets the ESP8266, connects to the Wi-Fi network, and establishes a TCP connection with the server.
 */
void initializeWiFiAndTCP();

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
 * @brief Establishes a TCP connection with the ESP8266.
 *
 * This function sends the command to start a TCP connection with the specified server IP and port.
 * If the connection fails, it retries once before returning the result.
 *
 * @return True if the connection is successful, false otherwise.
 */
bool connectToTCPServer();

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
 * @brief Sends a TCP message to the server.
 *
 * This function sends a TCP message to the server. If the message is not acknowledged,
 * it retries up to three times. If the handshake fails, it performs the handshake again.
 *
 * @param message The message to send.
 */
void sendTCPMessage(const char* message);

/**
 * @brief Receives and processes TCP messages from the server.
 *
 * This function receives TCP messages from the server and processes them.
 * If the message contains setpoints, it updates the setpoints in the automation module.
 */
void receiveTCPMessage();

/**
 * @brief Checks the TCP connection status with the ESP8266.
 *
 * This function checks if the system is connected to the TCP server. If not, it attempts to reconnect.
 *
 * @return True if connected, false otherwise.
 */
bool checkConnection();

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
 * @brief Wrapper function to get the response from the ESP8266 as a String.
 *
 * This function calls getResponse() and returns the response as a String.
 *
 * @return The response from the ESP8266 as a String.
 */
String getResponse();

/**
 * @brief Prints the response from the ESP8266.
 *
 * This function fetches the response from the ESP8266 and stores it in the latestResponse variable.
 */
void printESPResponse();

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

#endif // WIFI_TCP_H
