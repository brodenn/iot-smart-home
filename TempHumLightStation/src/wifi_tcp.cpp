#include "../include/wifi_tcp.h"
#include <SoftwareSerial.h>

// Wi-Fi credentials
extern const char* ssid;
extern const char* password;
extern const char* serverIP;
extern const int serverPort;

// Create a SoftwareSerial object for ESP8266 communication
extern SoftwareSerial espSerial;
extern bool connected;

// Reset the ESP8266 module
void resetESP8266() {
    Serial.println("Resetting ESP8266...");
    sendATCommand("AT+RST", 5000);
}

// Set the Wi-Fi mode of the ESP8266
void setWiFiMode() {
    Serial.println("Setting ESP8266 Wi-Fi mode...");
    sendATCommand("AT+CWMODE=1", 2000);
}

// Connect to Wi-Fi network
void connectToWiFi() {
    String command = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
    Serial.println("Connecting to Wi-Fi: " + String(ssid));
    sendATCommand(command.c_str(), 10000);
}

// Connect to the TCP server
void connectToServer() {
    Serial.println("Checking server connection status...");
    if (checkConnection()) {
        Serial.println("Already connected to the server.");
        connected = true;
        return;
    }

    String command = "AT+CIPSTART=\"TCP\",\"" + String(serverIP) + "\"," + String(serverPort);
    Serial.println("Connecting to server...");
    if (sendATCommand(command.c_str(), 10000)) {
        Serial.println("Successfully connected to the server.");
        connected = true;
    } else {
        Serial.println("Failed to connect to the server.");
        connected = false;
    }
}

// Send a message to the server over TCP
void sendTCPMessage(const char* message) {
    if (!connected) {
        Serial.println("No active connection. Cannot send message.");
        return;
    }

    String command = "AT+CIPSEND=" + String(strlen(message));
    Serial.println("Sending data: " + String(message));
    if (sendATCommand(command.c_str(), 5000)) {
        espSerial.print(message);
    } else {
        Serial.println("Failed to send data.");
        connected = false;
    }
}

// Check if the connection to the server is active
bool checkConnection() {
    sendATCommand("AT+CIPSTATUS", 2000);
    String response = getResponse();
    return response.indexOf("STATUS:3") >= 0; // STATUS:3 indicates active TCP connection
}

// Send an AT command and check the response
bool sendATCommand(const char* command, unsigned long timeout) {
    Serial.println("Sending command: " + String(command));
    espSerial.println(command);

    unsigned long startTime = millis();
    while ((millis() - startTime) < timeout) {
        if (espSerial.available()) {
            String response = getResponse();
            if (response.indexOf("OK") >= 0 || response.indexOf("CONNECT") >= 0 || response.indexOf("SEND OK") >= 0) {
                return true;
            }
            if (response.indexOf("ERROR") >= 0 || response.indexOf("FAIL") >= 0) {
                return false;
            }
        }
    }
    Serial.println("No response from ESP8266.");
    return false;
}

// Read the response from the ESP8266
String getResponse() {
    String response = "";
    while (espSerial.available()) {
        response += (char)espSerial.read();
    }
    Serial.println("ESP8266 response: " + response);
    return response;
}
