#include "../include/wifi_tcp.h"
#include "../include/globals.h"

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;
extern bool handshake_done;
extern String latestResponse;

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
                Serial.print("Received from ESP32: "); // Debugging statement
                Serial.println(latestResponse); // Debugging statement
                if (latestResponse.indexOf("HANDSHAKE:ESP32_READY\n") >= 0) {
                    handshake_done = true;
                    Serial.println("Handshake successful");
                } else {
                    Serial.println("Unexpected response: " + latestResponse);
                }
            } else {
                Serial.println("Failed to send handshake message");
            }
        } else {
            Serial.println("No response from ESP32");
        }
        serialBusy = false;
        handshakeRetries++;
    }

    if (!handshake_done) {
        Serial.println("Failed to perform handshake after retries");
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
        Serial.print("Not sending message. Connected: ");
        Serial.print(connected);
        Serial.print(", Handshake done: ");
        Serial.println(handshake_done);
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
                    Serial.println("Handshake failed, retrying...");
                    performHandshake();
                } else {
                    Serial.println("Unexpected response: " + latestResponse);
                }
            }
        }
        serialBusy = false;
        retries++;
    }

    if (!ackReceived) {
        Serial.println("Failed to send message after retries");
    }
}