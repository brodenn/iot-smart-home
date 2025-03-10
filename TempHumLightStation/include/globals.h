#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <SoftwareSerial.h>  

// Wi-Fi credentials
extern const char* ssid;
extern const char* password;
extern const char* serverIP;
extern const int serverPort;

// Global variables
extern bool handshake_done;
extern bool serialBusy;
extern bool connected;
extern String latestResponse;
extern String accumulatedResponse;

// Declare `espSerial` as an external variable
extern SoftwareSerial espSerial;  // ✅ Added to fix undefined reference

#endif // GLOBALS_H
