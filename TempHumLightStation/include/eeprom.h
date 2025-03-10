#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

void eeprom_write_word(uint16_t address, uint16_t value);
uint16_t eeprom_read_word(uint16_t address);

#endif // EEPROM_H