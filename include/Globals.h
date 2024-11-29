#ifndef GLOBALS_H
#define GLOBALS_H

#include <WiFiClient.h>
#include <Arduino.h>

extern WiFiClient clients[CHO_MAX_CONNECTIONS];
extern TaskHandle_t banchoTasks[CHO_MAX_CONNECTIONS];
extern bool taskActive[CHO_MAX_CONNECTIONS];

#endif