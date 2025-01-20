#define F_CPU 16000000UL
#include "../include/sensor.h"
#include "../include/uart.h"
#include "../include/i2c.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// Si7021 I2C address
#define SI7021_ADDR 0x40

// Si7021 Commands
#define TEMP_MEASURE_HOLD 0xE3
#define HUMID_MEASURE_HOLD 0xE5

// -------- ADC Functions --------

// Initialize the ADC for reading the light sensor
void ADC_Init() {
    // Configure ADC: AVCC as reference, right-adjusted result
    ADMUX = (1 << REFS0); // Reference voltage = AVCC
    ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC and set prescaler to 8
}

uint16_t ADC_Read(uint8_t channel) {
    // Select ADC channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // Start conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));
    
    // Return ADC result
    return ADC;
}

uint16_t LightSensor_ReadLux() {
    uint16_t adcValue = ADC_Read(1); // Read from channel A1
    char buffer[50];

    // Debug raw ADC value
    sprintf(buffer, "Raw ADC Value: %u\r\n", adcValue);
    UART_SendString(buffer);

    // Convert ADC value to voltage
    float voltage = (adcValue * 3.3) / 1023.0; // Assuming 3.3V reference voltage

    // Calculate lux (adjust sensitivity based on ALS-PT19 datasheet)
    uint16_t lux = (uint16_t)(voltage / 0.005); // Example sensitivity: 0.005 V/lux

    return lux;
}



// -------- Si7021 Functions --------

int16_t Si7021_ReadTemperature() {
    uint8_t msb, lsb;
    uint16_t rawTemp;

    I2C_Start();
    I2C_Write(SI7021_ADDR << 1);
    I2C_Write(0xE3); // Temperature measurement command
    I2C_Stop();

    _delay_ms(50);

    I2C_Start();
    I2C_Write((SI7021_ADDR << 1) | 1);
    msb = I2C_Read_ACK();
    lsb = I2C_Read_NACK();
    I2C_Stop();

    rawTemp = (msb << 8) | lsb;

    // Debug raw temperature data
    char buffer[50];
    sprintf(buffer, "Raw Temp Data: MSB=0x%02X, LSB=0x%02X\r\n", msb, lsb);
    UART_SendString(buffer);

    // Calculate temperature in hundredths of a degree
    return ((17572L * rawTemp) / 65536L) - 4685;
}


int16_t Si7021_ReadHumidity() {
    uint8_t msb, lsb;
    uint16_t rawHum;

    I2C_Start();
    I2C_Write(SI7021_ADDR << 1);
    I2C_Write(0xE5); // Humidity measurement command
    I2C_Stop();

    _delay_ms(50);

    I2C_Start();
    I2C_Write((SI7021_ADDR << 1) | 1);
    msb = I2C_Read_ACK();
    lsb = I2C_Read_NACK();
    I2C_Stop();

    rawHum = (msb << 8) | lsb;

    // Debug raw humidity data
    char buffer[50];
    sprintf(buffer, "Raw Hum Data: MSB=0x%02X, LSB=0x%02X\r\n", msb, lsb);
    UART_SendString(buffer);

    // Calculate humidity in hundredths of a percent
    return ((12500L * rawHum) / 65536L) - 600;
}
