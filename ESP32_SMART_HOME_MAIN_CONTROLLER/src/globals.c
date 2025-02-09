#include "globals.h"

float temperature = 0.0;
float humidity = 0.0;
uint16_t lux = 0;
bool heater = false;
bool dehumidifier = false;

int client_sock = -1;
bool handshake_done = false;
