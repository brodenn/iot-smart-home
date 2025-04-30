#ifndef HANDSHAKE_H
#define HANDSHAKE_H

/**
 * @file handshake.h
 * @brief Header file for handshake-related functions in the ESP32 Smart Home Main Controller project.
 */

/**
 * @brief Performs the handshake process between devices.
 */
void performHandshake();

/**
 * @brief Sanitizes the input string to ensure it is safe for processing.
 * 
 * @param input Pointer to the input string to be sanitized.
 */
void sanitize_input(char *input);

#endif // HANDSHAKE_H