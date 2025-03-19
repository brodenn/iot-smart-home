/**
 * @file globals.cpp
 * @brief This file contains the definitions of global variables used throughout the TempHumLightStation project.
 *
 * The global variables defined in this file are used for managing the state of the system, storing Wi-Fi credentials,
 * and handling communication with the ESP8266 Wi-Fi module.
 *
 * The main functionalities provided by this file include:
 * - Managing connection status and handshake state.
 * - Storing the latest and accumulated responses from the ESP8266.
 * - Defining Wi-Fi credentials and server information.
 * - Initializing the SoftwareSerial interface for communication with the ESP8266.
 *
 * Dependencies:
 * - globals.h: Header file containing the declarations of the global variables.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/globals.h"

// Global Variable Definitions
bool connected = false;            ///< Indicates if the system is connected to the Wi-Fi network.
bool handshake_done = false;       ///< Indicates if the handshake with the server is completed.
bool serialBusy = false;           ///< Indicates if the serial communication is currently busy.
String latestResponse = "";        ///< Stores the latest response received from the ESP8266.
String accumulatedResponse = "";   ///< Stores the accumulated responses from the ESP8266.

// Wi-Fi Credentials
char ssid[32];       ///< Wi-Fi SSID.
char password[32];   ///< Wi-Fi password.
const char* serverIP = "192.168.10.206";    ///< Server IP address.
const int serverPort = 8080;                ///< Server port number;

// Sensor data
int16_t globalTemperature = 0;      ///< Global variable to store the temperature reading.
int16_t globalHumidity = 0;         ///< Global variable to store the humidity reading.
uint16_t globalLight = 0;           ///< Global variable to store the light intensity reading;

// Define SoftwareSerial for ESP8266 (avoids multiple definitions)
SoftwareSerial espSerial(8, 9);  // RX=8, TX=9
