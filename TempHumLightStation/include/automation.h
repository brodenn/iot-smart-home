#ifndef AUTOMATION_H
#define AUTOMATION_H

#include <stdint.h>

// Function prototypes
void Automation_Init(void);
void Automation_Update(int16_t temperature, int16_t humidity);
uint8_t GetHeaterState(void);
uint8_t GetDehumidifierState(void);
void Automation_SetSetpoints(int16_t temp, int16_t hum);

#endif // AUTOMATION_H
