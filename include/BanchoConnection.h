#ifndef BANCHO_CONNECTION_H
#define BANCHO_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>

typedef struct {
    WiFiClient client;
    TaskHandle_t task;
    int index;
    bool active;
} BanchoConnection;

#endif
