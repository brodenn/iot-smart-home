#ifndef WIFI_HANDSHAKE_H
#define WIFI_HANDSHAKE_H

void performHandshake();
void sendTCPMessage(const char* message);
void handleSetpoints(const String& data);

#endif // WIFI_HANDSHAKE_H