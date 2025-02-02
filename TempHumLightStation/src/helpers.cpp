#include "../include/helpers.h"

void handleSetpoints(const String& data) {
    Serial.println("Received Setpoints: " + data);
    // Add JSON parsing to extract temperature and humidity, then update automation
}
