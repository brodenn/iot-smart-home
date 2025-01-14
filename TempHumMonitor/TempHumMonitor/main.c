#include "i2c.h"      // I2C driver
#include "sensor.h"   // Sensor-specific functions
#include "uart.h"     // UART driver
#include <util/delay.h>
#include <stdio.h>

int main() {
    char buffer[50];  // Buffer for formatting messages

    // Initialize peripherals
    I2C_Init();       // Initialize I2C
    UART_Init();      // Initialize UART for serial communication

    // Main loop
    while (1) {
        // Read temperature from Si7021 sensor
        float temperature = Si7021_ReadTemperature();

        // Format the temperature data into the buffer
        sprintf(buffer, "Temperature: %.2f°C\r\n", temperature);

        // Send the formatted data over UART
        UART_SendString(buffer);

        // Delay before the next reading
        _delay_ms(2000); // 2-second delay
    }

    return 0;
}
