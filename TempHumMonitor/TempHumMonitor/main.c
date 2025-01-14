#define F_CPU 16000000UL // Define clock frequency for delay
#include "include/i2c.h"
#include "include/sensor.h"
#include "include/uart.h"
#include <util/delay.h>
#include <stdio.h>

int main() {
    char buffer[50];

    I2C_Init();  // Initialize I2C
    UART_Init(); // Initialize UART

    UART_SendString("Starting temperature monitoring...\r\n");

    while (1) {
        // Read temperature
        float temperature = Si7021_ReadTemperature();

        // Convert temperature to integer-based format
        int temp_whole = (int)temperature;                  // Whole number part
        int temp_fraction = (int)((temperature - temp_whole) * 100); // Fractional part

        // Handle negative fractional part
        if (temp_fraction < 0) temp_fraction = -temp_fraction;

        // Format and display temperature with degree symbol
        sprintf(buffer, "Temperature: %d.%02d degC\r\n", temp_whole, temp_fraction);
        UART_SendString(buffer);


        _delay_ms(2000); // 2-second delay
    }

    return 0;
}