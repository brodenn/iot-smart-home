#include <Arduino.h>
#include "globals.h"
#include "sensor.h"
#include "automation.h"
#include "i2c.h"
#include "wifi_tcp.h"
#include <SoftwareSerial.h>

// Define RX and TX pins for ESP8266 communication
#define RX 8
#define TX 9

// Create a SoftwareSerial object for ESP8266
SoftwareSerial espSerial(RX, TX);

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
