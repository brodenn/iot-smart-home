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
        while (serialBusy);
        serialBusy = true;

        clearESPBuffer();
        delay(200);

        // Ensure TCP connection is open before sending handshake
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

        if (!waitForResponse(">", 4000)) {  // Increase wait time
            Serial.println("[ESP8266] ❌ No `>` prompt. Retrying CIPSEND...");
            espSerial.println("AT+CIPSEND=26"); // Retry once before failing
            if (!waitForResponse(">", 4000)) {
                Serial.println("[ESP8266] ❌ CIPSEND failed again. Closing connection...");
                espSerial.println("AT+CIPCLOSE");
                delay(2000);
                connectToTCPServer();
                serialBusy = false;
                handshakeRetries++;
                continue;
            }
        }

        // Send clean handshake message
        espSerial.print("HANDSHAKE:ARDUINO_READY\n");
        delay(1000);

        char handshakeResponse[128] = {0};
        unsigned long startMillis = millis();

        while (millis() - startMillis < 10000) {
            if (espSerial.available()) {
                String response = getResponse(); // Store response in String
                strncat(handshakeResponse, response.c_str(), sizeof(handshakeResponse) - strlen(handshakeResponse) - 1); // Properly append data

                // Handle split "+IPD" messages properly
                if (strstr(handshakeResponse, "+IPD")) {
                    String moreData = getResponse();  // Get additional data
                    strncat(handshakeResponse, moreData.c_str(), sizeof(handshakeResponse) - strlen(handshakeResponse) - 1);
                }

                if (strstr(handshakeResponse, "HANDSHAKE:ESP32_READY")) {
                    Serial.println("[ESP8266] ✅ Handshake successful!");
                    handshake_done = true;
                    connected = true;
                    serialBusy = false;
                    return true;
                }
            }
            delay(100);
        }

        Serial.println("[ESP8266] ❌ No response. Retrying...");
        serialBusy = false;
        handshakeRetries++;
        delay(3000);
    }

    Serial.println("[ESP8266] 🚨 Handshake failed after max retries.");
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

    for (int i = 0; i < 5; i++) {
        if (!waitForESPReady(5000)) continue;

        clearESPBuffer();
        espSerial.println("AT");

        char response[16];
        getResponse(response, sizeof(response));

        if (strstr(response, "OK")) {
            return true;
        }
    }

    resetESP8266();
    return false;
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
    char fullMessage[128];  // Ensure full message fits buffer
    snprintf(fullMessage, sizeof(fullMessage), "DATA:%s\n", message);

    while (retries < maxRetries && !ackReceived) {
        while (serialBusy);
        serialBusy = true;

        if (!isESP8266Ready()) {
            serialBusy = false;
            delay(1000);
            continue;
        }

        char cipsendCommand[32];
        snprintf(cipsendCommand, sizeof(cipsendCommand), "AT+CIPSEND=%d", strlen(fullMessage));

        espSerial.println(cipsendCommand);
        if (!waitForResponse(">", 2000)) {  // Ensure we get `>` before sending
            Serial.println("[ESP8266] ❌ Failed to get `>` prompt, retrying...");
            serialBusy = false;
            retries++;
            continue;
        }

        espSerial.print(fullMessage);
        delay(1500);

        char response[64];  // Small buffer for response handling
        getResponse(response, sizeof(response));

        if (strstr(response, "ACK")) {
            ackReceived = true;
        } else if (strstr(response, "ERROR:HANDSHAKE_FAILED")) {
            performHandshake();
        }

        serialBusy = false;
        retries++;
    }
}

/**
 * @brief Prints the response from the ESP8266.
 *
 * This function fetches the response from the ESP8266 and stores it in the latestResponse variable.
 */
void printESPResponse() {
    latestResponse = getResponse();
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
            int dataIndex = latestResponse.indexOf(":");
            if (dataIndex != -1) {
                // Extract the received data as a C-string
                const char *dataStart = latestResponse.c_str() + dataIndex + 1;

                // Ensure buffer is large enough
                char buffer[128] = {0};
                strncpy(buffer, dataStart, sizeof(buffer) - 1);
                
                // Append data to accumulatedResponse
                accumulatedResponse += buffer;
            }

            // Check if the accumulated response contains the expected data
            if (accumulatedResponse.indexOf("temp=") >= 0 && accumulatedResponse.indexOf("&humidity=") >= 0) {
                handleSetpoints(accumulatedResponse);
                accumulatedResponse = ""; // Clear accumulated response

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
        } else {
            accumulatedResponse = "";
        }
    } else {
        serialBusy = false;
    }
}
