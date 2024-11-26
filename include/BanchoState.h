#ifndef BANCHOSTATE_H
#define BANCHOSTATE_H

#include <WiFi.h>

typedef struct {
    WiFiClient client;
    bool writeLock;
    bool alive;
} BanchoState;

#endif
