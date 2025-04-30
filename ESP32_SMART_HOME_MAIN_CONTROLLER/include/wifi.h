/**
 * @file wifi.h
 * @brief Header file for Wi-Fi initialization functionality.
 *
 * This file contains the declaration of the function used to initialize
 * the Wi-Fi module for the ESP32 Smart Home Main Controller.
 *
 * @note Ensure that the appropriate Wi-Fi configuration is set up
 *       before calling the initialization function.
 */

/**
 * @brief Initializes the Wi-Fi module for the ESP32 Smart Home Main Controller.
 *
 * This function sets up the Wi-Fi module with the predefined configuration
 * and establishes a connection to the specified network.
 *
 * @param ssid The SSID of the Wi-Fi network to connect to.
 * @param password The password for the Wi-Fi network.
 */
#ifndef WIFI_H
#define WIFI_H

// Function declaration for Wi-Fi initialization
void wifi_init(void);

#endif // WIFI_H
