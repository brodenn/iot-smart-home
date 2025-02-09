#ifndef WIFI_COMMANDS_H
#define WIFI_COMMANDS_H

void resetESP8266();
void setWiFiMode();
void connectToWiFi();
void connectToServer();
bool checkConnection();
bool sendATCommand(const char* command, unsigned long timeout);
String getResponse();
void printESPResponse();

#endif // WIFI_COMMANDS_H