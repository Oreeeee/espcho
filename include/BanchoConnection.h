#ifndef BANCHO_CONNECTION_H
#define BANCHO_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <stdint.h>
#include "datatypes/StatusUpdate.h"

typedef struct {
    WiFiClient client;
    TaskHandle_t task;
    int index;
    bool active;
    char *username;
    uint32_t userId;
    StatusUpdate statusUpdate;
} BanchoConnection;

#endif
