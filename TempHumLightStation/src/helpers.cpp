/**
 * @file helpers.cpp
 * @brief This file contains helper functions for the TempHumLightStation project.
 *
 * The helper functions provided in this file are used for various tasks such as clearing the ESP8266 serial buffer,
 * reading responses from the ESP8266, checking TCP connection status, handling incoming data, and formatting sensor data.
 *
 * The main functionalities provided by this file include:
 * - Clearing the ESP8266 serial buffer.
 * - Reading the full response from the ESP8266 into a fixed-size buffer.
 * - Waiting for a specific response from the ESP8266.
 * - Checking the TCP connection status with the ESP8266.
 * - Handling incoming data and extracting temperature and humidity setpoints.
 * - Generating a JSON string for sensor data with safe formatting.
 *
 * Dependencies:
 * - helpers.h: Header file containing the declarations of the helper functions.
 * - globals.h: Header file containing the declarations of global variables.
 * - automation.h: Header file containing the declarations of automation functions.
 * - sensor.h: Header file containing the declarations of sensor functions.
 * - wifi_tcp.h: Header file containing the declarations of Wi-Fi TCP functions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include "../include/helpers.h"
#include "../include/globals.h"
#include "../include/eeprom.h"
#include "../include/wifi_tcp.h"
#include "../include/automation.h"
#include "../include/sensor.h"
#include "../include/i2c.h"
#include "../include/wifi_handshake.h"
#include "../include/wifi_commands.h"

#include <Arduino.h>
#include <string.h>


void clearESPBuffer() {
    while (espSerial.available()) {
        espSerial.read();
    }
}


void getResponse(char *buffer, size_t len) {
    memset(buffer, 0, len);  // Clear buffer
    size_t index = 0;
    unsigned long startTime = millis();

    while (millis() - startTime < 5000) {
        while (espSerial.available()) {
            char c = (char)espSerial.read();
            if (index < len - 1) {  // Prevent buffer overflow
                buffer[index++] = c;
            }
        }

        // Stop reading if key responses are detected
        if (strstr(buffer, "OK") || strstr(buffer, "ERROR") || strstr(buffer, "FAIL") || strstr(buffer, "+IPD")) {
            break;
        }

        delay(50);
    }

    buffer[index] = '\0'; // Null-terminate the string

    // Debug only when needed (avoiding RAM waste)
    if (index > 0) {
        Serial.print("[ESP8266] Response: ");
        Serial.println(buffer);
    }
}


String getResponse() {
    char response[128];  // Increased buffer size for longer responses
    getResponse(response, sizeof(response));
    return String(response);  
}


bool waitForResponse(const char *expected, unsigned long timeout) {
    char response[128];  
    unsigned long startTime = millis();

    while (millis() - startTime < timeout) {
        if (espSerial.available()) {
            getResponse(response, sizeof(response));
            if (strstr(response, expected)) {
                return true;
            }
        }
        delay(50);
    }
    return false;
}

bool checkConnection() {
    if (connected) {
        return true;
    }

    clearESPBuffer();
    espSerial.println("AT+CIPSTATUS");
    char response[64] = {0};
    getResponse(response, sizeof(response));

    if (strstr(response, "STATUS:3")) {
        connected = true;
        return true;
    }

    Serial.println("[ESP8266] ❌ TCP Connection lost! Reconnecting...");
    connectToTCPServer();
    return connected;
}


void handleSetpoints(const String &data) {
    const char *cstrData = data.c_str();
    const char *tempPtr = strstr(cstrData, "temp=");
    const char *humPtr = strstr(cstrData, "&humidity=");

    if (tempPtr && humPtr) {
        char tempStr[10] = {0}, humStr[10] = {0};

        // Check length before copying to prevent buffer overflow
        size_t tempLen = humPtr - (tempPtr + 5);
        if (tempLen >= sizeof(tempStr)) tempLen = sizeof(tempStr) - 1;
        strncpy(tempStr, tempPtr + 5, tempLen);

        size_t humLen = strlen(humPtr + 10);
        if (humLen >= sizeof(humStr)) humLen = sizeof(humStr) - 1;
        strncpy(humStr, humPtr + 10, humLen);

        float temp = atof(tempStr);
        float hum = atof(humStr);

        int16_t tempInt = static_cast<int16_t>(temp * 100);
        int16_t humInt = static_cast<int16_t>(hum * 100);

        // Update setpoints using automation module
        Automation_SetSetpoints(tempInt, humInt);
    }
}


void formatSensorData(char *buffer, size_t len, int16_t temperature, int16_t humidity, uint16_t light) {
    snprintf(buffer, len,
        "{\"temperature\":%.2f,\"humidity\":%.2f,\"lux\":%u,\"heater\":%s,\"dehumidifier\":%s,\"sp_temperature\":%.2f,\"sp_humidity\":%.2f}\n",
        temperature / 100.0, 
        humidity / 100.0, 
        light, 
        GetHeaterState() ? "true" : "false",
        GetDehumidifierState() ? "true" : "false",
        SP_TEMP / 100.0, 
        SP_HUM / 100.0
    );
}


void initializeSerial() {
    Serial.begin(9600);
    delay(5000);  // Wait for serial communication to stabilize
}


void initializeESP() {
    espSerial.begin(9600);
    delay(7000);  // Wait for ESP8266 to initialize
    enableEcho();  
}


void readSetpointsFromEEPROM() {
    int16_t storedTemp = eeprom_read_word(EEPROM_TEMP_ADDR);
    int16_t storedHum = eeprom_read_word(EEPROM_HUM_ADDR);
    Automation_SetSetpoints(storedTemp, storedHum);
}


void readWiFiCredentialsFromEEPROM() {
    eeprom_read_string(EEPROM_SSID_ADDR, ssid, sizeof(ssid));
    eeprom_read_string(EEPROM_PASSWORD_ADDR, password, sizeof(password));
}


void storeDefaultCredentialsIfNeeded() {
    if (strlen(ssid) == 0 || strlen(password) == 0) {
        strcpy(ssid, "TN_24GHz_F3908D");
        strcpy(password, "UP7ADFCFXJ");
        eeprom_write_string(EEPROM_SSID_ADDR, ssid);
        eeprom_write_string(EEPROM_PASSWORD_ADDR, password);
    }
}


void initializeSensors() {
    ADC_Init();  // Initialize ADC
    I2C_Init();  // Initialize I2C
}


void handleIncomingMessages() {
    if (espSerial.available()) {
        receiveTCPMessage();  // Handle incoming TCP messages
    }
}


void checkAndReconnectTCP() {
    if (!checkConnection()) {
        connectToTCPServer();  // Reconnect to TCP server if connection is lost
    }
}


void performHandshakeIfNeeded() {
    if (!handshake_done) {
        performHandshake();  // Perform handshake if not done
    }
}

void readSensorData() {
    globalTemperature = Si7021_ReadTemperature();  // Read temperature
    globalHumidity = Si7021_ReadHumidity();  // Read humidity
    globalLight = LightSensor_ReadLux();  // Read light intensity
}


void updateAutomationStates() {
    Automation_Update(globalTemperature, globalHumidity);  // Update automation states
}


void sendSensorData() {
    // Use the global variables for sensor data
    char sensorData[128];
    formatSensorData(sensorData, sizeof(sensorData), globalTemperature, globalHumidity, globalLight);

    sendTCPMessage(sensorData);  // Send sensor data to the server
}
