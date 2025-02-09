#ifndef TCP_SERVER_H
#define TCP_SERVER_H

void tcp_server_task(void *pvParameters);
void send_tcp_message(const char *message);
void send_tcp_message_with_ack(const char *message);
void send_setpoints_with_ack(const char *setpoints);
void handle_received_data(const char *data);
void send_data_to_web_server(const char *data); // Add this declaration

#endif // TCP_SERVER_H
