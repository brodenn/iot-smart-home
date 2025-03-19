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

// Sensor data
extern int16_t globalTemperature;      ///< Global variable to store the temperature reading.
extern int16_t globalHumidity;         ///< Global variable to store the humidity reading.
extern uint16_t globalLight;           ///< Global variable to store the light intensity reading;

/**
 * @brief Reads Wi-Fi credentials from EEPROM.
 *
 * This function reads the Wi-Fi SSID and password from EEPROM and stores them in the global variables.
 */
void readWiFiCredentialsFromEEPROM();

#endif // GLOBALS_H
