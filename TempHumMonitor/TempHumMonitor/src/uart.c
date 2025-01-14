#include <avr/io.h>
#include "uart.h"

void UART_Init() {
    // Set baud rate (9600 with 16MHz clock)
    UBRR0H = 0;               // High byte of baud rate
    UBRR0L = 103;             // Low byte of baud rate
    UCSR0B = (1 << TXEN0);    // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data, 1 stop bit, no parity
}

void UART_SendChar(char c) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait until data register is empty
    UDR0 = c;                         // Load the character into the data register
}

void UART_SendString(const char* str) {
    while (*str) {
        UART_SendChar(*str++); // Send each character in the string
    }
}

char UART_ReceiveChar() {
    while (!(UCSR0A & (1 << RXC0))); // Wait until data is received
    return UDR0;                     // Return received data
}
