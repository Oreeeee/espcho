#ifndef BANCHO_CONNECTION_H
#define BANCHO_CONNECTION_H

#include <stdint.h>
#include "datatypes/StatusUpdate.h"
#include "BanchoState.h"

typedef struct {
    int clientSock;
    TaskHandle_t task;
    int index;
    bool active;
    char *username;
    uint32_t userId;
    StatusUpdate statusUpdate;
    BanchoState *bstate;
    uint16_t version; // uint16_t is enough for pre-2012 clients
} BanchoConnection;

#endif
