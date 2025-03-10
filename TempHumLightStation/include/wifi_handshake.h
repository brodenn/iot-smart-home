/*
 * wifi_handshake.h - Header File for Wi-Fi Handshake Operations
 */

 #ifndef WIFI_HANDSHAKE_H
 #define WIFI_HANDSHAKE_H
 
 #include <Arduino.h>
 
 // Function declarations
 bool isESP8266Ready();
 bool performHandshake();
 void receiveTCPMessage();
 void sendTCPMessage(const char* message);
 void printESPResponse();
 
 #endif // WIFI_HANDSHAKE_H
 