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
extern char ssid[32];       ///< Wi-Fi SSID.
extern char password[32];   ///< Wi-Fi password.
extern const char* serverIP;   ///< Server IP address.
extern const int serverPort;   ///< Server port number;

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
 * @brief Establishes a TCP connection with the ESP8266.
 *
 * This function sends the command to start a TCP connection with the specified server IP and port.
 * If the connection fails, it retries once before returning the result.
 *
 * @return True if the connection is successful, false otherwise.
 */
bool connectToTCPServer();

#endif // WIFI_TCP_H
