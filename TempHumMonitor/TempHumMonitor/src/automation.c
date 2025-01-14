#include "../include/automation.h"
#include "../include/uart.h"
#include <stdio.h>

// Variables for setpoints and hysteresis
int16_t SP_TEMP = 2500;       // Default setpoint temperature: 25.00°C
int16_t SP_TEMP_HYS = 100;    // Hysteresis: 1.00°C
int16_t SP_HUM = 5000;        // Default setpoint humidity: 50.00%RH
int16_t SP_HUM_HYS = 200;     // Hysteresis: 2.00%RH

// States for heater and dehumidifier
static uint8_t heaterState = 0;
static uint8_t dehumidifierState = 0;

void Automation_Init(void) {
    heaterState = 0;
    dehumidifierState = 0;
    UART_SendString("Automation initialized.\r\n");
}

void Automation_Update(int16_t temperature, int16_t humidity) {
    char buffer[100];

    // Thermostat logic with hysteresis
    if (heaterState == 0 && temperature < (SP_TEMP - SP_TEMP_HYS)) {
        heaterState = 1;
        sprintf(buffer, "Heater turned ON. Temp: %d.%02d°C\r\n", temperature / 100, temperature % 100);
        UART_SendString(buffer);
        } else if (heaterState == 1 && temperature > (SP_TEMP + SP_TEMP_HYS)) {
        heaterState = 0;
        sprintf(buffer, "Heater turned OFF. Temp: %d.%02d°C\r\n", temperature / 100, temperature % 100);
        UART_SendString(buffer);
    }

    // Humidity control logic with hysteresis
    if (dehumidifierState == 0 && humidity > (SP_HUM + SP_HUM_HYS)) {
        dehumidifierState = 1;
        sprintf(buffer, "Dehumidifier turned ON. Hum: %d.%02d%%RH\r\n", humidity / 100, humidity % 100);
        UART_SendString(buffer);
        } else if (dehumidifierState == 1 && humidity < (SP_HUM - SP_HUM_HYS)) {
        dehumidifierState = 0;
        sprintf(buffer, "Dehumidifier turned OFF. Hum: %d.%02d%%RH\r\n", humidity / 100, humidity % 100);
        UART_SendString(buffer);
    }
}

uint8_t GetHeaterState(void) {
    return heaterState;
}

uint8_t GetDehumidifierState(void) {
    return dehumidifierState;
}

void Automation_SetSetpoints(int16_t temp, int16_t hum) {
    SP_TEMP = temp;
    SP_HUM = hum;

    char buffer[100];
    sprintf(buffer, "Setpoints updated. Temp: %d.%02d°C, Hum: %d.%02d%%RH\r\n",
    SP_TEMP / 100, SP_TEMP % 100, SP_HUM / 100, SP_HUM % 100);
    UART_SendString(buffer);
}
