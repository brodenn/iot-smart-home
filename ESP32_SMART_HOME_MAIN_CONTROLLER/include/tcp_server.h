#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <stdbool.h>  // ✅ Fix: Include the bool type
#include <stdint.h>   // ✅ Ensures correct type usage in ESP32

void tcp_server_task(void *pvParameters);
void send_tcp_message(const char *message);
void send_tcp_message_with_ack(const char *message, bool *ackReceived);
void send_setpoints_with_ack(const char *setpoints);
void handle_received_data(const char *data);
void send_data_to_web_server(const char *data);

#endif // TCP_SERVER_H
