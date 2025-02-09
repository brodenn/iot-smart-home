#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern float temperature;
extern float humidity;
extern uint16_t lux;
extern bool heater;
extern bool dehumidifier;

extern int client_sock;
extern bool handshake_done;

#ifdef __cplusplus
}
#endif

#endif // GLOBALS_H
