#include "i2c.h"
#include <util/delay.h>

#define SI7021_ADDR 0x40 // I2C address of Si7021

float Si7021_ReadTemperature() {
    uint8_t msb, lsb;
    uint16_t rawTemp;

    // Start I2C transaction to initiate temperature measurement
    I2C_Start();
    I2C_Write(SI7021_ADDR << 1); // Send device address with write bit
    I2C_Write(0xE3);             // Command to measure temperature (Hold Master Mode)
    I2C_Stop();

    _delay_ms(50); // Wait for the sensor to complete measurement

    // Read the temperature data (2 bytes)
    I2C_Start();
    I2C_Write((SI7021_ADDR << 1) | 1); // Send device address with read bit
    msb = I2C_Read_ACK();             // Most significant byte
    lsb = I2C_Read_NACK();            // Least significant byte
    I2C_Stop();

    // Combine the two bytes into a 16-bit value
    rawTemp = (msb << 8) | lsb;

    // Convert the raw temperature to °C
    return ((175.72 * rawTemp) / 65536.0) - 46.85;
}
