#include "../include/wifi_tcp.h"
#include "../include/globals.h"

extern SoftwareSerial espSerial;
extern bool serialBusy;
extern bool connected;
extern String latestResponse;
extern bool handshake_done;

void resetESP8266() {
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+RST");
    delay(5000);
    printESPResponse();
    serialBusy = false;
}

void setWiFiMode() {
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+CWMODE=1");
    delay(2000);
    printESPResponse();
    serialBusy = false;
}

void connectToWiFi() {
    while (serialBusy);
    serialBusy = true;
    String command = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
    espSerial.println(command);
    delay(10000);
    printESPResponse();
    serialBusy = false;
}

void connectToServer() {
    for (int i = 0; i < 3; i++) { // Retry up to 3 times
        while (serialBusy);
        serialBusy = true;
        String command = "AT+CIPSTART=\"TCP\",\"" + String(serverIP) + "\"," + String(serverPort);
        espSerial.println(command);
        delay(5000); // Decreased delay to give more time for connection
        printESPResponse();
        serialBusy = false;
        if (checkConnection()) {
            connected = true;
            return;
        }
    }
    connected = false;
}

bool checkConnection() {
    while (serialBusy);
    serialBusy = true;
    espSerial.println("AT+CIPSTATUS");
    delay(2000);
    getResponse(); // Update latestResponse
    serialBusy = false;
    return latestResponse.indexOf("STATUS:3") >= 0; // STATUS:3 indicates active TCP connection
}

bool sendATCommand(const char* command, unsigned long timeout) {
    while (serialBusy);
    serialBusy = true;
    espSerial.println(command);
    delay(timeout);
    printESPResponse();
    serialBusy = false;
    return true;
}

String getResponse() {
    String response = "";
    while (espSerial.available()) {
        char c = (char)espSerial.read();
        response += c;
        Serial.print(c); // Debugging statement to print each character
    }

    // Check for +IPD message and read the subsequent data
    if (response.indexOf("+IPD") >= 0) {
        int dataLengthStart = response.indexOf(":") + 1;
        int dataLengthEnd = response.indexOf(",", dataLengthStart);
        int dataLength = response.substring(dataLengthStart, dataLengthEnd).toInt();
        while (dataLength > 0 && espSerial.available()) {
            char c = (char)espSerial.read();
            response += c;
            Serial.print(c); // Debugging statement to print each character
            dataLength--;
        }
        // Confirm handshake if ESP32_READY is received
        if (response.indexOf("ESP32_READY") >= 0) {
            handshake_done = true;
            Serial.println("Handshake confirmed."); // Debugging statement
        }
    }

    latestResponse = response; // Store the latest response
    Serial.println(response); // Debugging statement
    return response;
}

void printESPResponse() {
    String response = getResponse();
    Serial.println(response);
}