#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include "config.h"

WiFiClient clients[CHO_MAX_CONNECTIONS];
TaskHandle_t banchoTasks[CHO_MAX_CONNECTIONS];
bool taskActive[CHO_MAX_CONNECTIONS];
