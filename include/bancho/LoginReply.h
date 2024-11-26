#ifndef LOGINREPLY_H
#define LOGINREPLY_H

#include <stdint.h>
#include <WiFi.h>
#include "BanchoState.h"

typedef struct {
    uint32_t response;
} LoginReply;

void LoginReply_Write(LoginReply p, BanchoState *bstate);

#endif