/**
 * @file eeprom.h
 * @brief This file contains the declarations of EEPROM read and write functions and address definitions for the AVR microcontroller.
 *
 * The functions provided in this file allow for reading and writing 16-bit words to and from
 * the EEPROM memory of an AVR microcontroller. These functions are essential for storing
 * and retrieving persistent data such as configuration settings or calibration values.
 *
 * The addresses defined in this file are used for storing and retrieving setpoints for temperature and humidity
 * in the EEPROM memory of the AVR microcontroller.
 *
 * The main functionalities provided by this file include:
 * - Writing a 16-bit word to a specified EEPROM address.
 * - Reading a 16-bit word from a specified EEPROM address.
 * - Defining EEPROM addresses for temperature and humidity setpoints.
 *
 * Dependencies:
 * - stdint.h: Standard integer types.
 *
 * @note This file is part of the TempHumLightStation project.
 */

#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

// EEPROM addresses for setpoints
const int EEPROM_TEMP_ADDR = 0; ///< EEPROM address for storing the temperature setpoint.
const int EEPROM_HUM_ADDR = 2;  ///< EEPROM address for storing the humidity setpoint.

// EEPROM addresses for Wi-Fi credentials
const int EEPROM_SSID_ADDR = 4;       ///< EEPROM address for storing the Wi-Fi SSID.
const int EEPROM_PASSWORD_ADDR = 36;  ///< EEPROM address for storing the Wi-Fi password.

/**
 * @brief Writes a 16-bit word to the specified EEPROM address.
 *
 * This function writes a 16-bit word to the EEPROM memory of the AVR microcontroller.
 * It first writes the low byte and then the high byte to consecutive addresses.
 *
 * @param address The EEPROM address to write to.
 * @param value The 16-bit value to write to the EEPROM.
 */
void eeprom_write_word(uint16_t address, uint16_t value);

/**
 * @brief Reads a 16-bit word from the specified EEPROM address.
 *
 * This function reads a 16-bit word from the EEPROM memory of the AVR microcontroller.
 * It first reads the low byte and then the high byte from consecutive addresses and combines them.
 *
 * @param address The EEPROM address to read from.
 * @return The 16-bit value read from the EEPROM.
 */
uint16_t eeprom_read_word(uint16_t address);

/**
 * @brief Writes a string to the specified EEPROM address.
 *
 * This function writes a string to the EEPROM memory of the AVR microcontroller.
 *
 * @param address The EEPROM address to write to.
 * @param str The string to write to the EEPROM.
 */
void eeprom_write_string(uint16_t address, const char* str);

/**
 * @brief Reads a string from the specified EEPROM address.
 *
 * This function reads a string from the EEPROM memory of the AVR microcontroller.
 *
 * @param address The EEPROM address to read from.
 * @param buffer The buffer to store the read string.
 * @param length The maximum length of the string to read.
 */
void eeprom_read_string(uint16_t address, char* buffer, uint16_t length);

#endif // EEPROM_H