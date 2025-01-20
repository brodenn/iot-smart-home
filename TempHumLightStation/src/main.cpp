#include <Arduino.h>
#include <SoftwareSerial.h>

// Include C files for sensors and automation
extern "C" {
    #include "sensor.h"
    #include "automation.h"
    #include "i2c.h"
}

// Define RX and TX pins for ESP8266 communication
#define RX 8
#define TX 9

// Create a SoftwareSerial object for ESP8266
SoftwareSerial espSerial(RX, TX);

// Wi-Fi credentials
const char* ssid = "TN_24GHz_F3908D";
const char* password = "UP7ADFCFXJ";

// Server details
const char* serverIP = "192.168.10.206"; // ESP32 IP address
const int serverPort = 8080;

// Connection state
bool connected = false;

// Function declarations
void resetESP8266();
void setWiFiMode();
void connectToWiFi();
void connectToServer();
void sendTCPMessage(const char* message);
bool checkConnection();
bool sendATCommand(const char* command, unsigned long timeout);
String getResponse();

void setup() {
    // Initialize Serial for debugging
    Serial.begin(9600);
    delay(1000);

    // Initialize SoftwareSerial for ESP8266
    espSerial.begin(9600);
    delay(1000);

    // Initialize sensors and automation
    I2C_Init();
    ADC_Init();
    Automation_Init();

    Serial.println("=== ESP8266 TCP Client with Sensor Integration ===");

    // Initialize ESP8266
    resetESP8266();
    setWiFiMode();
    connectToWiFi();
}

void loop() {
    if (!connected) {
        connectToServer();
    }

    if (connected) {
        // Read sensor data
        int16_t temperature = Si7021_ReadTemperature();
        int16_t humidity = Si7021_ReadHumidity();
        uint16_t lux = LightSensor_ReadLux();

        // Update automation states
        Automation_Update(temperature, humidity);
        uint8_t heaterState = GetHeaterState();
        uint8_t dehumidifierState = GetDehumidifierState();

        // Create JSON string with sensor data
        String jsonData = String("{\"temperature\":") + (temperature / 100.0) +
                          ",\"humidity\":" + (humidity / 100.0) +
                          ",\"lux\":" + lux +
                          ",\"heater\":" + (heaterState ? "true" : "false") +
                          ",\"dehumidifier\":" + (dehumidifierState ? "true" : "false") + "}";

        // Send sensor data to the ESP32
        sendTCPMessage(jsonData.c_str());
        delay(10000); // Send data every 10 seconds
    } else {
        Serial.println("Reconnecting to server...");
        connectToServer();
    }
}

// Function to reset the ESP8266
void resetESP8266() {
    Serial.println("Resetting ESP8266...");
    sendATCommand("AT+RST", 5000);
}

// Function to set Wi-Fi mode
void setWiFiMode() {
    Serial.println("Setting ESP8266 Wi-Fi mode...");
    sendATCommand("AT+CWMODE=1", 2000);
}

// Function to connect to Wi-Fi
void connectToWiFi() {
    String command = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
    Serial.println("Connecting to Wi-Fi: " + String(ssid));
    sendATCommand(command.c_str(), 10000);
}

// Function to connect to the TCP server
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

// Function to send a TCP message
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

// Function to check the server connection
bool checkConnection() {
    sendATCommand("AT+CIPSTATUS", 2000);
    String response = getResponse();
    return response.indexOf("STATUS:3") >= 0; // STATUS:3 indicates active TCP connection
}

// Function to send an AT command and check the response
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

// Function to read the ESP8266 response
String getResponse() {
    String response = "";
    while (espSerial.available()) {
        response += (char)espSerial.read();
    }
    Serial.println("ESP8266 response: " + response);
    return response;
}
