#define F_CPU 16000000UL
#include "include/i2c.h"
#include "include/sensor.h"
#include "include/uart.h"
#include <util/delay.h>
#include <stdio.h>

int main() {
    char buffer[100]; // Buffer for UART messages

    UART_Init();   // Initialize UART for serial communication
    I2C_Init();    // Initialize I2C for Si7021 sensor
    ADC_Init();    // Initialize ADC for light sensor

    UART_SendString("Starting sensor monitoring...\r\n");

    while (1) {
        // Read temperature and humidity
        int16_t temperature = Si7021_ReadTemperature();
        int16_t humidity = Si7021_ReadHumidity();

        // Read light intensity
        uint16_t lux = LightSensor_ReadLux();

        // Format and send sensor data
        sprintf(buffer, "Temp: %d.%02d degC, Hum: %d.%02d%%RH, Light: %u lux\r\n",
        temperature / 100, temperature % 100,
        humidity / 100, humidity % 100,
        lux);
        UART_SendString(buffer); // Send the formatted string

        _delay_ms(2000); // 2-second delay
    }

    return 0;
}