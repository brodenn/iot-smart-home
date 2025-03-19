/**
 * @file wifi_handshake.cpp
 * @brief This file contains the implementation of Wi-Fi handshake functions for the ESP8266 module.
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
 * - wifi_handshake.h: Header file containing the declarations of the Wi-Fi handshake functions.
 * - globals.h: Header file containing the declarations of global variables.
 * - helpers.h: Header file containing the declarations of helper functions.
 * - wifi_commands.h: Header file containing the declarations of Wi-Fi command functions.
 * - wifi_tcp.h: Header file containing the declarations of Wi-Fi TCP functions.
 * - SoftwareSerial.h: Library for software-based serial communication.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/wifi_handshake.h"
#include "../include/globals.h"
#include "../include/helpers.h"
#include "../include/wifi_commands.h"
#include "../include/wifi_tcp.h"
#include <SoftwareSerial.h>

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;
extern bool handshake_done;

/**
 * @brief Performs a handshake with the ESP32 server.
 *
 * This function sends a handshake message to the ESP32 server and waits for a response.
 * If the handshake is successful, it sets the handshake_done flag to true.
 *
 * @return True if the handshake is successful, false otherwise.
 */
bool performHandshake() {
    if (handshake_done) return true;

    handshake_done = false;
    int handshakeRetries = 0;
    const int maxRetries = 5;

    while (!handshake_done && handshakeRetries < maxRetries) {
        if (!attemptHandshake()) {
            handshakeRetries++;
            delay(3000);
        }
    }

    if (!handshake_done) {
        Serial.println("[ESP8266] 🚨 Handshake failed after max retries.");
    }

    return handshake_done;
}

/**
 * @brief Attempts to perform a handshake with the ESP32 server.
 *
 * This function sends a handshake message to the ESP32 server and waits for a response.
 * If the handshake is successful, it sets the handshake_done flag to true.
 *
 * @return True if the handshake is successful, false otherwise.
 */
bool attemptHandshake() {
    while (serialBusy);
    serialBusy = true;

    clearESPBuffer();
    delay(200);

    if (!checkConnection()) {
        Serial.println("[ESP8266] ❌ TCP connection lost. Reconnecting...");
        connectToTCPServer();
        if (!checkConnection()) {
            serialBusy = false;
            return false;
        }
    }

    espSerial.println("AT+CIPSEND=26");
    delay(500);

    if (!waitForResponse(">", 4000)) {
        Serial.println("[ESP8266] ❌ No `>` prompt. Retrying CIPSEND...");
        espSerial.println("AT+CIPSEND=26");
        if (!waitForResponse(">", 4000)) {
            Serial.println("[ESP8266] ❌ CIPSEND failed again. Closing connection...");
            espSerial.println("AT+CIPCLOSE");
            delay(2000);
            connectToTCPServer();
            serialBusy = false;
            return false;
        }
    }

    espSerial.print("HANDSHAKE:ARDUINO_READY\n");
    delay(1000);

    bool success = waitForHandshakeResponse();
    serialBusy = false;
    return success;
}

/**
 * @brief Waits for a handshake response from the ESP32 server.
 *
 * This function waits for a handshake response from the ESP32 server within a specified timeout period.
 *
 * @return True if the handshake response is received, false otherwise.
 */
bool waitForHandshakeResponse() {
    char handshakeResponse[128] = {0};
    unsigned long startMillis = millis();

    while (millis() - startMillis < 10000) {
        if (espSerial.available()) {
            String response = getResponse();
            strncat(handshakeResponse, response.c_str(), sizeof(handshakeResponse) - strlen(handshakeResponse) - 1);

            if (strstr(handshakeResponse, "+IPD")) {
                String moreData = getResponse();
                strncat(handshakeResponse, moreData.c_str(), sizeof(handshakeResponse) - strlen(handshakeResponse) - 1);
            }

            if (strstr(handshakeResponse, "HANDSHAKE:ESP32_READY")) {
                Serial.println("[ESP8266] ✅ Handshake successful!");
                handshake_done = true;
                connected = true;
                return true;
            }
        }
        delay(100);
    }

    Serial.println("[ESP8266] ❌ No response. Retrying...");
    return false;
}

/**
 * @brief Checks if the ESP8266 is ready.
 *
 * This function sends the "AT" command to the ESP8266 and waits for an "OK" response.
 * If the ESP8266 is not ready, it resets the ESP8266.
 *
 * @return True if the ESP8266 is ready, false otherwise.
 */
bool isESP8266Ready() {
    clearESPBuffer();
    espSerial.println("AT");
    return waitForResponse("OK", 2000);
}

/**
 * @brief Sends a TCP message to the server.
 *
 * This function sends a TCP message to the server. If the message is not acknowledged,
 * it retries up to three times. If the handshake fails, it performs the handshake again.
 *
 * @param message The message to send.
 */
void sendTCPMessage(const char* message) {
    if (!checkConnection()) {
        connectToTCPServer();
    }
    if (!handshake_done) {
        performHandshake();
    }

    if (!connected || !handshake_done) {
        return;
    }

    const int maxRetries = 3;
    int retries = 0;
    bool ackReceived = false;
    char fullMessage[128];
    snprintf(fullMessage, sizeof(fullMessage), "DATA:%s\n", message);

    while (retries < maxRetries && !ackReceived) {
        if (attemptSendMessage(fullMessage)) {
            ackReceived = true;
        } else {
            retries++;
        }
    }
}

/**
 * @brief Attempts to send a TCP message to the server.
 *
 * This function attempts to send a TCP message to the server and waits for an acknowledgment.
 *
 * @param fullMessage The full message to send.
 * @return True if the message is acknowledged, false otherwise.
 */
bool attemptSendMessage(const char* fullMessage) {
    while (serialBusy);
    serialBusy = true;

    if (!isESP8266Ready()) {
        serialBusy = false;
        delay(1000);
        return false;
    }

    char cipsendCommand[32];
    snprintf(cipsendCommand, sizeof(cipsendCommand), "AT+CIPSEND=%d", strlen(fullMessage));

    espSerial.println(cipsendCommand);
    if (!waitForResponse(">", 2000)) {
        Serial.println("[ESP8266] ❌ Failed to get `>` prompt, retrying...");
        serialBusy = false;
        return false;
    }

    espSerial.print(fullMessage);
    delay(1500);

    char response[64];
    getResponse(response, sizeof(response));

    if (strstr(response, "ACK")) {
        serialBusy = false;
        return true;
    } else if (strstr(response, "ERROR:HANDSHAKE_FAILED")) {
        performHandshake();
    }

    serialBusy = false;
    return false;
}

/**
 * @brief Receives and processes TCP messages from the server.
 *
 * This function receives TCP messages from the server and processes them.
 * If the message contains setpoints, it updates the setpoints in the automation module.
 */
void receiveTCPMessage() {
    while (serialBusy);
    serialBusy = true;

    if (espSerial.available()) {
        latestResponse = getResponse();
        serialBusy = false;

        if (latestResponse.startsWith("+IPD")) {
            processIncomingMessage();
        } else {
            accumulatedResponse = "";
        }
    } else {
        serialBusy = false;
    }
}

/**
 * @brief Processes incoming TCP messages from the server.
 *
 * This function processes incoming TCP messages from the server and updates the setpoints if necessary.
 */
void processIncomingMessage() {
    int dataIndex = latestResponse.indexOf(":");
    if (dataIndex != -1) {
        const char *dataStart = latestResponse.c_str() + dataIndex + 1;
        char buffer[128] = {0};
        strncpy(buffer, dataStart, sizeof(buffer) - 1);
        accumulatedResponse += buffer;
    }

    if (accumulatedResponse.indexOf("temp=") >= 0 && accumulatedResponse.indexOf("&humidity=") >= 0) {
        handleSetpoints(accumulatedResponse);
        accumulatedResponse = "";

        while (serialBusy);
        serialBusy = true;

        if (!isESP8266Ready()) {
            serialBusy = false;
            delay(1000);
            return;
        }

        espSerial.println("AT+CIPSEND=14");
        if (!sendATCommand("AT+CIPSEND=14", 1000)) {
            serialBusy = false;
            return;
        }

        espSerial.print("SETPOINTS_ACK\n");
        delay(500);
        espSerial.find("SEND OK");
        serialBusy = false;
    }
}
