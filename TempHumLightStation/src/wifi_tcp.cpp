/**
 * @file wifi_tcp.cpp
 * @brief This file contains the implementation of Wi-Fi TCP functions for the ESP8266 module.
 *
 * The functions provided in this file allow for initializing Wi-Fi and TCP connections,
 * connecting to a TCP server, and attempting to establish a new TCP connection.
 *
 * The main functionalities provided by this file include:
 * - Initializing Wi-Fi and TCP connections.
 * - Connecting to a TCP server.
 *
 * Dependencies:
 * - wifi_tcp.h: Header file containing the declarations of the Wi-Fi TCP functions.
 * - globals.h: Header file containing the declarations of global variables.
 * - helpers.h: Header file containing the declarations of helper functions.
 * - wifi_commands.h: Header file containing the declarations of Wi-Fi command functions.
 * - wifi_handshake.h: Header file containing the declarations of Wi-Fi handshake functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/wifi_tcp.h"
#include "../include/globals.h"
#include "../include/helpers.h"
#include "../include/wifi_commands.h"
#include "../include/wifi_handshake.h"
#include <SoftwareSerial.h>

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;
extern bool handshake_done;

/**
 * @brief Initializes Wi-Fi and TCP connections.
 *
 * This function resets the ESP8266, connects to the Wi-Fi network, and establishes a TCP connection with the server.
 */
void initializeWiFiAndTCP() {
    resetESP8266();
    clearESPBuffer();
    
    if (!connectToWiFi()) {
        return;
    }

    if (!connectToTCPServer()) {
        return;
    }

    performHandshake();
}

/**
 * @brief Establishes a TCP connection with the ESP8266.
 *
 * This function sends the command to start a TCP connection with the specified server IP and port.
 * If the connection fails, it retries once before returning the result.
 *
 * @return True if the connection is successful, false otherwise.
 */
bool connectToTCPServer() {
    while (serialBusy);
    serialBusy = true;

    clearESPBuffer();
    
    char command[64];
    snprintf(command, sizeof(command), "AT+CIPSTART=\"TCP\",\"%s\",%d", serverIP, serverPort);

    espSerial.println(command);

    bool success = waitForResponse("CONNECT", 8000);

    if (!success) {
        espSerial.println("AT+CIPCLOSE");
        delay(500);
        espSerial.println(command);
        success = waitForResponse("CONNECT", 8000);
    }

    serialBusy = false;
    return success;
}
