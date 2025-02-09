#ifndef TCP_SERVER_H
#define TCP_SERVER_H

void tcp_server_task(void *pvParameters);
void send_tcp_message(const char *message);

#endif // TCP_SERVER_H
