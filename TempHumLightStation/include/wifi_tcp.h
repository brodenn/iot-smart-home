#ifndef WIFI_TCP_H
#define WIFI_TCP_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Wi-Fi credentials
extern const char* ssid;
extern const char* password;
extern const char* serverIP;
extern const int serverPort;

// Create a SoftwareSerial object for ESP8266 communication
extern SoftwareSerial espSerial;

// Global variables
extern bool handshake_done;
extern bool serialBusy;
extern bool connected;
extern String latestResponse;

// Function declarations
void initializeWiFiAndTCP();
void resetESP8266();
void setWiFiMode();
void connectToWiFi();
void connectToServer();
void performHandshake();
void sendTCPMessage(const char* message);
bool checkConnection();
bool sendATCommand(const char* command, unsigned long timeout);
String getResponse();
void printESPResponse();

#endif // WIFI_TCP_H