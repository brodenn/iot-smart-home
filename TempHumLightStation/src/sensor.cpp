/**
 * @file sensor.cpp
 * @brief This file contains the implementation of sensor functions for the TempHumLightStation project.
 *
 * The functions provided in this file allow for initializing and reading data from the light sensor (using ADC)
 * and the Si7021 temperature and humidity sensor (using I2C).
 *
 * The main functionalities provided by this file include:
 * - Initializing the ADC for reading the light sensor.
 * - Reading the light sensor data and converting it to lux.
 * - Reading temperature and humidity data from the Si7021 sensor.
 *
 * Dependencies:
 * - sensor.h: Header file containing the declarations of the sensor functions.
 * - i2c.h: Header file containing the declarations of I2C functions.
 * - avr/io.h: AVR device-specific IO definitions.
 * - util/delay.h: Utility functions for delay.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#define F_CPU 16000000UL
#include "../include/sensor.h"
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

    // Convert ADC value to voltage
    float voltage = (adcValue * 5) / 1023.0; 

    // Calculate lux (adjust sensitivity based on ALS-PT19 datasheet)
    uint16_t lux = (uint16_t)(voltage / 0.005);

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

    // Calculate humidity in hundredths of a percent
    return ((12500L * rawHum) / 65536L) - 600;
}