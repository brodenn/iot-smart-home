/**
 * @file tcp_server.h
 * @brief Header file for the TCP server functionality in the ESP32 Smart Home Main Controller.
 *
 * This file contains the declarations of functions used for managing TCP server tasks,
 * sending messages, handling received data, and communicating with a web server.
 */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdbool.h>  // ✅ Fix: Include the bool type
#include <stdint.h>   // ✅ Ensures correct type usage in ESP32

/**
 * @brief Task function for the TCP server.
 *
 * This function initializes and manages the TCP server, handling incoming connections
 * and data from clients.
 *
 * @param pvParameters Pointer to task parameters (if any).
 */
void tcp_server_task(void *pvParameters);

/**
 * @brief Sends a TCP message to the connected client.
 *
 * @param message The message to be sent as a null-terminated string.
 */
void send_tcp_message(const char *message);

/**
 * @brief Sends a TCP message and waits for an acknowledgment.
 *
 * @param message The message to be sent as a null-terminated string.
 * @param ackReceived Pointer to a boolean variable that will be set to true if acknowledgment is received.
 */
void send_tcp_message_with_ack(const char *message, bool *ackReceived);

/**
 * @brief Sends setpoints to the client and waits for an acknowledgment.
 *
 * @param setpoints The setpoints to be sent as a null-terminated string.
 */
void send_setpoints_with_ack(const char *setpoints);

/**
 * @brief Handles data received from the TCP client.
 *
 * This function processes the received data and performs appropriate actions based on its content.
 *
 * @param data The received data as a null-terminated string.
 */
void handle_received_data(const char *data);

/**
 * @brief Sends data to the web server.
 *
 * This function forwards data to the web server for further processing or storage.
 *
 * @param data The data to be sent as a null-terminated string.
 */
void send_data_to_web_server(const char *data);

#endif // TCP_SERVER_H