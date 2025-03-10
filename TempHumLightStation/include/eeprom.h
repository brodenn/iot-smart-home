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

#endif // EEPROM_H