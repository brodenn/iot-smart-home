#ifndef WIFITCP_H
#define WIFITCP_H

#include <Arduino.h>

extern bool connected;

void resetESP8266();
void setWiFiMode();
void connectToWiFi();
void connectToServer();
void sendTCPMessage(const char* message);
bool checkConnection();
bool sendATCommand(const char* command, unsigned long timeout);
String getResponse();

#endif // WIFITCP_H
