#include "../include/i2c.h"
#include <util/delay.h>
#include <stdio.h>

#define SI7021_ADDR 0x40 // I2C address of Si7021

float Si7021_ReadTemperature() {
    uint8_t msb = 0, lsb = 0;
    uint16_t rawTemp;

    // Start I2C transaction to initiate temperature measurement
    I2C_Start();
    I2C_Write(SI7021_ADDR << 1); // Write mode
    I2C_Write(0xE3);             // Command to measure temperature
    I2C_Stop();

    _delay_ms(50); // Wait for the sensor to complete measurement

    // Read the temperature data (2 bytes)
    I2C_Start();
    I2C_Write((SI7021_ADDR << 1) | 1); // Read mode
    msb = I2C_Read_ACK();             // Most significant byte
    lsb = I2C_Read_NACK();            // Least significant byte
    I2C_Stop();

    // Print raw data for debugging
    char debugMsg[50];
    sprintf(debugMsg, "Raw Temp Data: MSB=0x%02X, LSB=0x%02X\r\n", msb, lsb);
    UART_SendString(debugMsg);

    // Combine the two bytes into a 16-bit value
    rawTemp = (msb << 8) | lsb;

    // Check for invalid values
    if (rawTemp == 0xFFFF || rawTemp == 0x0000) {
        UART_SendString("Error: Invalid temperature data\r\n");
        return -999.0; // Return error code
    }

    // Convert the raw temperature to °C
    return ((175.72 * rawTemp) / 65536.0) - 46.85;
}
