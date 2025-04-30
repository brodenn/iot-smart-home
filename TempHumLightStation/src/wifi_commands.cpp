/**
 * @file wifi_commands.cpp
 * @brief This file contains the implementation of Wi-Fi command functions for the ESP8266 module.
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
 * - wifi_commands.h: Header file containing the declarations of the Wi-Fi command functions.
 * - globals.h: Header file containing the declarations of global variables.
 * - helpers.h: Header file containing the declarations of helper functions.
 * - Arduino.h: Arduino core functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/wifi_commands.h"
#include "../include/globals.h"
#include "../include/helpers.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;


void resetESP8266() {
    while (serialBusy);
    serialBusy = true;

    espSerial.println("AT+RST");
    delay(3000);  // Allow ESP8266 to reset
    clearESPBuffer();

    char response[64];
    getResponse(response, sizeof(response));

    if (!strstr(response, "OK")) {
        serialBusy = false;
        return;
    }

    sendATCommand("ATE1", 2000);  // Enable Echo

    if (!checkWiFiConnection()) {
        connectToWiFi();
    }

    serialBusy = false;
}


bool connectToWiFi() {
    while (serialBusy);
    serialBusy = true;

    clearESPBuffer();
    espSerial.println("AT+CWJAP?");
    delay(2000);  // Increased time to allow response

    char response[128];
    getResponse(response, sizeof(response));

    // Detect active connection using SSID pattern
    if (strstr(response, "+CWJAP:")) {  
        serialBusy = false;
        return true;  // Already connected
    }

    // If not connected, send SSID & Password
    clearESPBuffer();
    espSerial.print("AT+CWJAP=\"");
    espSerial.print(ssid);
    espSerial.print("\",\"");
    espSerial.print(password);
    espSerial.println("\"");

    delay(10000);  // Increased time for first connection attempt

    getResponse(response, sizeof(response));

    serialBusy = false;
    return (strstr(response, "WIFI CONNECTED") != NULL);
}


bool sendATCommand(const char* command, unsigned long timeout) {
    int retries = 0;
    char response[64];

    while (retries < 3) {
        while (serialBusy);
        serialBusy = true;

        if (!waitForESPReady(5000)) {
            serialBusy = false;
            return false;
        }

        clearESPBuffer();
        espSerial.println(command);
        delay(timeout);
        getResponse(response, sizeof(response));

        if (strstr(response, "busy")) {
            delay(2000);
            retries++;
            serialBusy = false;
            continue;
        }

        serialBusy = false;
        return true;
    }

    resetESP8266();
    return false;
}


bool waitForESPReady(unsigned long timeout) {
    unsigned long startMillis = millis();
    char response[64];

    while (millis() - startMillis < timeout) {
        clearESPBuffer();
        espSerial.println("AT");
        delay(1000);
        getResponse(response, sizeof(response));

        if (strstr(response, "OK")) {
            return true;
        }
    }
    return false;
}


bool checkWiFiConnection() {
    while (serialBusy);
    serialBusy = true;

    sendATCommand("AT+CWJAP?", 2000);

    char response[128];
    getResponse(response, sizeof(response));

    serialBusy = false;
    return (strstr(response, "+CWJAP:") != NULL);
}


void enableEcho() {
    sendATCommand("ATE1", 1000);
}
