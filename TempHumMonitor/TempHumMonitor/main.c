#include "i2c.h"
#include "sensor.h"
#include "uart.h"
#include <util/delay.h>
#include <stdio.h>

int main() {
    char buffer[50];

    I2C_Init();  // Initialize I2C
    UART_Init(); // Initialize UART for debugging

    while (1) {
        // Read temperature
        float temperature = Si7021_ReadTemperature();

        // Format the temperature reading for UART
        sprintf(buffer, "Temperature: %.2f°C\r\n", temperature);

        // Send the temperature to the serial monitor
        UART_SendString(buffer);

        _delay_ms(2000); // Delay before the next measurement
    }

    return 0;
}
