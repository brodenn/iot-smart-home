#ifndef SHELLY_CONTROL_H
#define SHELLY_CONTROL_H

void send_http_request(const char* deviceIP, bool turnOn);

extern const char* heaterIP;
extern const char* humidifierIP;

#endif // SHELLY_CONTROL_H