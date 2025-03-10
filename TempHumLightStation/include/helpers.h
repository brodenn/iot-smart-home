#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

// Function declarations
void clearESPBuffer();
void getResponse(char* buffer, size_t len);  // Updated to match implementation
String getResponse();  // Wrapper to maintain compatibility
bool waitForResponse(const char* expected, unsigned long timeout);
bool checkConnection();
void handleSetpoints(const String& data);
void formatSensorData(char* buffer, size_t len, int16_t temperature, int16_t humidity, uint16_t light);

#endif // HELPERS_H
