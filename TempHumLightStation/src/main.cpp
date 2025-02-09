#include <Arduino.h>
#include <SoftwareSerial.h>
#include "../include/wifi_tcp.h"
#include "../include/globals.h"
#include "../include/automation.h"
#include "../include/sensor.h"
#include "../include/i2c.h"
#include "../include/helpers.h"
#include "../include/wifi_handshake.h"

// Define RX and TX pins for ESP8266 communication
#define RX 8
#define TX 9

// Create a SoftwareSerial object for ESP8266
SoftwareSerial espSerial(RX, TX);

// Initialize accumulatedResponse
String accumulatedResponse = "";

void setup() {
    // Initialize Serial for debugging
    Serial.begin(9600);
    delay(5000);

    // Initialize SoftwareSerial for ESP8266
    espSerial.begin(9600);
    delay(7000);

    initializeWiFiAndTCP(); // This will include the handshake process

    ADC_Init();
    I2C_Init();
    Automation_Init();
}

void loop() {
    // Check for incoming data from the server
    if (espSerial.available()) {
        receiveTCPMessage();
    }

    // Periodically check the connection status and send sensor data
    static unsigned long lastCheck = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= 30000) { // Check every 30 seconds
        lastCheck = currentMillis;
        if (!checkConnection()) {
            connectToServer();
        }

        // Perform handshake if not done
        if (!handshake_done) {
            performHandshake();
        }

        // Read sensor data and send only if handshake is done
        if (handshake_done) {
            int16_t temperature = Si7021_ReadTemperature();
            int16_t humidity = Si7021_ReadHumidity();
            uint16_t light = LightSensor_ReadLux();

            // Update automation system with sensor data
            Automation_Update(temperature, humidity);

            // Format sensor data into a JSON message
            String sensorData = "{\"temperature\":" + String(temperature / 100.0, 2) + 
                                ",\"humidity\":" + String(humidity / 100.0, 2) + 
                                ",\"lux\":" + String(light) + 
                                ",\"heater\":" + String(GetHeaterState() ? "true" : "false") + 
                                ",\"dehumidifier\":" + String(GetDehumidifierState() ? "true" : "false") + "}\n";

            sendTCPMessage(sensorData.c_str());
        }
    }
}