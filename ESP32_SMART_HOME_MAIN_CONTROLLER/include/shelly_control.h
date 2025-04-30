/**
 * @file shelly_control.h
 * @brief Header file for controlling Shelly devices via HTTP requests.
 *
 * This file provides the function declarations and external variables
 * for sending HTTP requests to control Shelly devices such as heaters
 * and humidifiers.
 */

/**
 * @brief Sends an HTTP request to control a Shelly device.
 *
 * @param deviceIP The IP address of the Shelly device.
 * @param turnOn A boolean indicating whether to turn the device on (true) or off (false).
 */
void send_http_request(const char* deviceIP, bool turnOn);

/**
 * @brief The IP address of the heater device.
 */
extern const char* heaterIP;

/**
 * @brief The IP address of the humidifier device.
 */
extern const char* humidifierIP;