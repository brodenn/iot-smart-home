#include "../include/wifi_tcp.h"
#include "../include/globals.h"
#include "../include/helpers.h"
#include "../include/wifi_handshake.h"
#include <SoftwareSerial.h>

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;
extern bool handshake_done;
extern String latestResponse;
extern String accumulatedResponse;

void performHandshake() {
    handshake_done = false;
    int handshakeRetries = 0;
    const int maxHandshakeRetries = 5;

    while (!handshake_done && handshakeRetries < maxHandshakeRetries) {
        if (!checkConnection()) {
            connectToWiFi();
            connectToServer();
        }

        while (serialBusy);
        serialBusy = true;
        espSerial.print("AT+CIPSEND=");
        espSerial.println(24); // Length of "HANDSHAKE:ARDUINO_READY\n"
        delay(1000);
        if (espSerial.find('>')) {
            espSerial.print("HANDSHAKE:ARDUINO_READY\n");
            delay(2000); // Wait for ESP32 to respond
            if (espSerial.find("SEND OK")) {
                latestResponse = espSerial.readStringUntil('\n');
                if (latestResponse.indexOf("HANDSHAKE:ESP32_READY\n") >= 0) {
                    handshake_done = true;
                }
            }
        }
        serialBusy = false;
        handshakeRetries++;
    }
}

void sendTCPMessage(const char* message) {
    if (!checkConnection()) {
        connectToServer();
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

    while (retries < maxRetries && !ackReceived) {
        while (serialBusy);
        serialBusy = true;
        // Send the message with the DATA tag and wait for acknowledgment
        String fullMessage = "DATA:" + String(message);
        espSerial.print("AT+CIPSEND=");
        espSerial.println(fullMessage.length());
        delay(1000);
        if (espSerial.find('>')) {
            espSerial.print(fullMessage);
            delay(1000);
            if (espSerial.find("SEND OK")) {
                latestResponse = espSerial.readStringUntil('\n');
                if (latestResponse.indexOf("ACK\n") >= 0) {
                    ackReceived = true;
                } else if (latestResponse.indexOf("ERROR:HANDSHAKE_FAILED\n") >= 0) {
                    performHandshake();
                }
            }
        }
        serialBusy = false;
        retries++;
    }
}

void receiveTCPMessage() {
    while (serialBusy);
    serialBusy = true;
    if (espSerial.available()) {
        latestResponse = espSerial.readStringUntil('\n');
        serialBusy = false;

        // Debugging statement to print the latest response
        Serial.print("Latest response: ");
        Serial.println(latestResponse);

        // Check if the response contains setpoint data
        if (latestResponse.startsWith("+IPD")) {
            accumulatedResponse += latestResponse.substring(latestResponse.indexOf(":") + 1);

            // Debugging statement to print the accumulated response
            Serial.print("Accumulated response: ");
            Serial.println(accumulatedResponse);

            if (accumulatedResponse.indexOf("temp=") >= 0 && accumulatedResponse.indexOf("&humidity=") >= 0) {
                // Handle the setpoint data
                handleSetpoints(accumulatedResponse);

                // Clear the accumulated response after processing
                accumulatedResponse = "";

                // Acknowledge the receipt of setpoint data
                while (serialBusy);
                serialBusy = true;
                espSerial.print("AT+CIPSEND=");
                espSerial.println(14); // Length of "SETPOINTS_ACK\n"
                delay(1000);
                if (espSerial.find('>')) {
                    espSerial.print("SETPOINTS_ACK\n");
                    delay(1000);
                    espSerial.find("SEND OK");
                }
                serialBusy = false;
            }
        } else {
            // Ignore corrupted or irrelevant data
            Serial.println("Ignoring irrelevant data.");
            accumulatedResponse = ""; // Clear accumulated response to avoid mixing data
        }
    } else {
        serialBusy = false;
    }
}