/**
 * @file eeprom.cpp
 * @brief This file contains the implementation of EEPROM read and write functions for the AVR microcontroller.
 *
 * The functions provided in this file allow for reading and writing 16-bit words to and from
 * the EEPROM memory of an AVR microcontroller. These functions are essential for storing
 * and retrieving persistent data such as configuration settings or calibration values.
 *
 * The main functionalities provided by this file include:
 * - Writing a 16-bit word to a specified EEPROM address.
 * - Reading a 16-bit word from a specified EEPROM address.
 * - Writing a string to a specified EEPROM address.
 * - Reading a string from a specified EEPROM address.
 *
 * Dependencies:
 * - avr/io.h: AVR device-specific IO definitions.
 * - eeprom.h: EEPROM function declarations.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#include <avr/io.h>
#include "../include/eeprom.h"

/**
 * @brief Writes a 16-bit word to the specified EEPROM address.
 *
 * This function writes a 16-bit word to the EEPROM memory of the AVR microcontroller.
 * It first writes the low byte and then the high byte to consecutive addresses.
 *
 * @param address The EEPROM address to write to.
 * @param value The 16-bit value to write to the EEPROM.
 */
void eeprom_write_word(uint16_t address, uint16_t value) {
    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address;             // Set up address register
    EEDR = value & 0xFF;        // Set up data register (low byte)
    EECR = (1 << EEMPE);        // Write logical one to EEMPE
    EECR |= (1 << EEPE);        // Start EEPROM write by setting EEPE

    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address + 1;         // Set up address register for high byte
    EEDR = (value >> 8);        // Set up data register (high byte)
    EECR = (1 << EEMPE);        // Write logical one to EEMPE
    EECR |= (1 << EEPE);        // Start EEPROM write by setting EEPE
}

/**
 * @brief Reads a 16-bit word from the specified EEPROM address.
 *
 * This function reads a 16-bit word from the EEPROM memory of the AVR microcontroller.
 * It first reads the low byte and then the high byte from consecutive addresses and combines them.
 *
 * @param address The EEPROM address to read from.
 * @return The 16-bit value read from the EEPROM.
 */
uint16_t eeprom_read_word(uint16_t address) {
    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address;             // Set up address register
    EECR |= (1 << EERE);        // Start EEPROM read by writing EERE
    uint8_t low_byte = EEDR;    // Read the low byte

    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address + 1;         // Set up address register for high byte
    EECR |= (1 << EERE);        // Start EEPROM read by writing EERE
    uint8_t high_byte = EEDR;   // Read the high byte

    return (high_byte << 8) | low_byte; // Combine high and low bytes
}

/**
 * @brief Writes a string to the specified EEPROM address.
 *
 * This function writes a string to the EEPROM memory of the AVR microcontroller.
 * It writes each character of the string to consecutive addresses until the null terminator is encountered.
 *
 * @param address The EEPROM address to write to.
 * @param str The string to write to the EEPROM.
 */
void eeprom_write_string(uint16_t address, const char* str) {
    while (*str) {
        while (EECR & (1 << EEPE)); // Wait for completion of previous write
        EEAR = address++;           // Set up address register
        EEDR = *str++;              // Set up data register
        EECR = (1 << EEMPE);        // Write logical one to EEMPE
        EECR |= (1 << EEPE);        // Start EEPROM write by setting EEPE
    }
    while (EECR & (1 << EEPE)); // Wait for completion of previous write
    EEAR = address;             // Set up address register
    EEDR = 0;                   // Write null terminator
    EECR = (1 << EEMPE);        // Write logical one to EEMPE
    EECR |= (1 << EEPE);        // Start EEPROM write by setting EEPE
}

/**
 * @brief Reads a string from the specified EEPROM address.
 *
 * This function reads a string from the EEPROM memory of the AVR microcontroller.
 * It reads each character from consecutive addresses until the null terminator is encountered or the specified length is reached.
 *
 * @param address The EEPROM address to read from.
 * @param buffer The buffer to store the read string.
 * @param length The maximum length of the string to read.
 */
void eeprom_read_string(uint16_t address, char* buffer, uint16_t length) {
    while (length--) {
        while (EECR & (1 << EEPE)); // Wait for completion of previous write
        EEAR = address++;           // Set up address register
        EECR |= (1 << EERE);        // Start EEPROM read by writing EERE
        char c = EEDR;              // Read the data
        *buffer++ = c;
        if (c == 0) break;          // Stop reading if null terminator is found
    }
}