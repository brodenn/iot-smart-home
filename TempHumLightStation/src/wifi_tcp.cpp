#include "../include/wifi_tcp.h"
#include "../include/globals.h"
#include "../include/wifi_commands.h"
#include "../include/wifi_handshake.h"

// Wi-Fi credentials
extern const char* ssid;
extern const char* password;
extern const char* serverIP;
extern const int serverPort;

// Create a SoftwareSerial object for ESP8266 communication
extern SoftwareSerial espSerial;

bool handshake_done = false;
bool serialBusy = false; // Flag to control access to the serial port
String latestResponse = ""; // Define the latestResponse variable

void initializeWiFiAndTCP() {
    // Reset the ESP8266 module
    resetESP8266();

    // Enable echo on ESP8266
    while (serialBusy);
    serialBusy = true;
    espSerial.println("ATE1");
    delay(2000);
    printESPResponse();
    serialBusy = false;

    // Set Wi-Fi mode to station
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+CWMODE=1");
    delay(2000);
    printESPResponse();
    serialBusy = false;

    // Connect to Wi-Fi network
    while (serialBusy);
    serialBusy = true;
    espSerial.print("AT+CWJAP=\"");
    espSerial.print(ssid);
    espSerial.print("\",\"");
    espSerial.print(password);
    espSerial.println("\"");
    delay(10000);
    printESPResponse();
    serialBusy = false;

    // Check connection status
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+CIPSTATUS");
    delay(2000);
    printESPResponse();
    serialBusy = false;

    // Connect to TCP server
    connectToServer();

    // Check connection status again
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+CIPSTATUS");
    delay(2000);
    printESPResponse();
    serialBusy = false;

    // Perform handshake with ESP32
    performHandshake();
}