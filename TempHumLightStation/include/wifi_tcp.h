/*
 * wifi_tcp.h - Header File for Wi-Fi TCP Operations
 */

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
 extern String accumulatedResponse; // ✅ Ensure this is declared only once
 
 // Function declarations
 void initializeWiFiAndTCP();
 void resetESP8266();
 bool connectToWiFi();
 bool connectToTCPServer(); // ✅ Only one TCP connection function
 bool performHandshake();
 void sendTCPMessage(const char* message);
 void receiveTCPMessage();
 bool checkConnection();
 bool sendATCommand(const char* command, unsigned long timeout);
 String getResponse();
 void printESPResponse();
 bool waitForResponse(const char* expected, unsigned long timeout);
 
 #endif // WIFI_TCP_H
 