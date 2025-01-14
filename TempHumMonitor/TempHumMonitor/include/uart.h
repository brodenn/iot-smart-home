#ifndef UART_H
#define UART_H

#include <stdint.h>

// Initialize UART with a given baud rate
void UART_Init(void);

// Send a single character over UART
void UART_SendChar(char c);

// Send a null-terminated string over UART
void UART_SendString(const char* str);

// (Optional) Receive a single character from UART
char UART_ReceiveChar(void);

#endif // UART_H
