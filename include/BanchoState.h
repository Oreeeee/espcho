#ifndef BANCHOSTATE_H
#define BANCHOSTATE_H

#include <WiFi.h>
#include "lwip/sockets.h"

typedef struct {
    WiFiClient client;
    int clientSock;
    bool writeLock;
    bool alive;
} BanchoState;

#endif
