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
#include "../include/automation.h"
#include "../include/sensor.h"
#include "../include/wifi_tcp.h"

#include <Arduino.h>
#include <string.h>

/**
 * @brief Clears the serial buffer of the ESP8266.
 *
 * This function reads and discards any data available in the ESP8266's serial buffer.
 * It is useful for ensuring that the buffer is empty before sending new commands.
 */
void clearESPBuffer() {
    while (espSerial.available()) {
        espSerial.read();
    }
}

/**
 * @brief Reads the full response from the ESP8266 into a fixed-size buffer.
 *
 * This function reads data from the ESP8266's serial buffer into the provided buffer
 * until a timeout occurs or a key response is detected.
 *
 * @param buffer The buffer to store the response.
 * @param len The length of the buffer.
 */
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

/**
 * @brief Wrapper function to get the response from the ESP8266 as a String.
 *
 * This function calls getResponse() and returns the response as a String.
 *
 * @return The response from the ESP8266 as a String.
 */
String getResponse() {
    char response[128];  // Increased buffer size for longer responses
    getResponse(response, sizeof(response));
    return String(response);  
}

/**
 * @brief Waits for a specific response from the ESP8266.
 *
 * This function waits for the specified response from the ESP8266 within the given timeout period.
 *
 * @param expected The expected response string.
 * @param timeout The timeout period in milliseconds.
 * @return True if the expected response is received, false otherwise.
 */
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

/**
 * @brief Checks the TCP connection status with the ESP8266.
 *
 * This function checks if the system is connected to the TCP server. If not, it attempts to reconnect.
 *
 * @return True if connected, false otherwise.
 */
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

/**
 * @brief Handles incoming data and extracts temperature and humidity setpoints.
 *
 * This function parses the incoming data to extract temperature and humidity setpoints
 * and updates the automation module with the new setpoints.
 *
 * @param data The incoming data containing the setpoints.
 */
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

/**
 * @brief Generates a JSON string for sensor data with safe formatting.
 *
 * This function formats the sensor data into a JSON string and stores it in the provided buffer.
 *
 * @param buffer The buffer to store the JSON string.
 * @param len The length of the buffer.
 * @param temperature The temperature reading (in tenths of degrees Celsius).
 * @param humidity The humidity reading (in tenths of percent relative humidity).
 * @param light The light intensity reading (in lux).
 */
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
