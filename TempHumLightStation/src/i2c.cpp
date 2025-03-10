/**
 * @file i2c.cpp
 * @brief This file contains the implementation of I2C communication functions for the AVR microcontroller.
 *
 * The functions provided in this file allow for initializing the I2C interface, sending start and stop conditions,
 * writing data to the I2C bus, and reading data from the I2C bus with or without acknowledgment.
 *
 * The main functionalities provided by this file include:
 * - Initializing the I2C interface.
 * - Sending start and stop conditions on the I2C bus.
 * - Writing data to the I2C bus.
 * - Reading data from the I2C bus with or without acknowledgment.
 *
 * Dependencies:
 * - i2c.h: Header file containing the declarations of the I2C functions.
 * - avr/io.h: AVR device-specific IO definitions.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include <avr/io.h>
#include "../include/i2c.h"

/**
 * @brief Initializes the I2C interface.
 *
 * This function sets the prescaler to 1, sets the SCL frequency to 100 kHz, and enables the TWI (I2C) interface.
 */
void I2C_Init() {
    TWSR = 0x00;        // Set prescaler to 1
    TWBR = 72;          // Set SCL frequency to 100 kHz
    TWCR = (1 << TWEN); // Enable TWI (I2C)
}

/**
 * @brief Sends a START condition on the I2C bus.
 *
 * This function sends a START condition on the I2C bus and waits for the operation to complete.
 */
void I2C_Start() {
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Send START condition
    while (!(TWCR & (1 << TWINT)));                  // Wait for completion
}

/**
 * @brief Sends a STOP condition on the I2C bus.
 *
 * This function sends a STOP condition on the I2C bus.
 */
void I2C_Stop() {
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Send STOP condition
}

/**
 * @brief Writes a byte of data to the I2C bus.
 *
 * This function loads a byte of data into the data register and starts the transmission.
 * It waits for the operation to complete before returning.
 *
 * @param data The byte of data to write to the I2C bus.
 */
void I2C_Write(uint8_t data) {
    TWDR = data;                      // Load data into data register
    TWCR = (1 << TWEN) | (1 << TWINT); // Start transmission
    while (!(TWCR & (1 << TWINT)));   // Wait for completion
}

/**
 * @brief Reads a byte of data from the I2C bus with acknowledgment.
 *
 * This function starts the reception of a byte of data from the I2C bus and sends an acknowledgment (ACK) after receiving the data.
 * It waits for the operation to complete before returning the received data.
 *
 * @return The byte of data received from the I2C bus.
 */
uint8_t I2C_Read_ACK() {
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT); // Enable ACK and start reception
    while (!(TWCR & (1 << TWINT)));                 // Wait for completion
    return TWDR;                                    // Return received data
}

/**
 * @brief Reads a byte of data from the I2C bus without acknowledgment.
 *
 * This function starts the reception of a byte of data from the I2C bus without sending an acknowledgment (NACK) after receiving the data.
 * It waits for the operation to complete before returning the received data.
 *
 * @return The byte of data received from the I2C bus.
 */
uint8_t I2C_Read_NACK() {
    TWCR = (1 << TWEN) | (1 << TWINT); // Start reception without ACK
    while (!(TWCR & (1 << TWINT)));   // Wait for completion
    return TWDR;                      // Return received data
}
