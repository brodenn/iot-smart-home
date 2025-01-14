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

    UART_SendString("Starting temperature and humidity monitoring...\r\n");

    while (1) {
        // Read temperature
        float temperature = Si7021_ReadTemperature();
        int temp_whole = (int)temperature;
        int temp_fraction = (int)((temperature - temp_whole) * 100);
        if (temp_fraction < 0) temp_fraction = -temp_fraction;

        // Read humidity
        float humidity = Si7021_ReadHumidity();
        int hum_whole = (int)humidity;
        int hum_fraction = (int)((humidity - hum_whole) * 100);
        if (hum_fraction < 0) hum_fraction = -hum_fraction;

        // Format and display temperature and humidity
        sprintf(buffer, "Temperature: %d.%02d degC, Humidity: %d.%02d%%RH\r\n",
        temp_whole, temp_fraction, hum_whole, hum_fraction);
        UART_SendString(buffer);

        _delay_ms(2000); // 2-second delay
    }

    return 0;
}
