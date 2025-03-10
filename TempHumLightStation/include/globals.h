/**
 * @file globals.h
 * @brief This file contains the declarations of global variables and Wi-Fi credentials for the TempHumLightStation project.
 *
 * The global variables declared in this file are used for managing the state of the system, storing Wi-Fi credentials,
 * and handling communication with the ESP8266 Wi-Fi module.
 *
 * The main functionalities provided by this file include:
 * - Declaring Wi-Fi credentials and server information.
 * - Declaring global variables for connection status and communication.
 * - Declaring the SoftwareSerial interface for communication with the ESP8266.
 *
 * Dependencies:
 * - Arduino.h: Arduino core functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SoftwareSerial.h>  

// Wi-Fi credentials
extern const char* ssid;       ///< Wi-Fi SSID.
extern const char* password;   ///< Wi-Fi password.
extern const char* serverIP;   ///< Server IP address.
extern const int serverPort;   ///< Server port number.

// Global variables
extern bool handshake_done;            ///< Indicates if the handshake with the server is completed.
extern bool serialBusy;                ///< Indicates if the serial communication is currently busy.
extern bool connected;                 ///< Indicates if the system is connected to the Wi-Fi network.
extern String latestResponse;          ///< Stores the latest response received from the ESP8266.
extern String accumulatedResponse;     ///< Stores the accumulated responses from the ESP8266.

// Declare `espSerial` as an external variable
extern SoftwareSerial espSerial;  ///< SoftwareSerial interface for communication with the ESP8266.

#endif // GLOBALS_H
