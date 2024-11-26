#ifndef BANCHOHEADER_H
#define BANCHOHEADER_H

#include <stdint.h>
#include <WiFi.h>
#include "BanchoState.h"

typedef struct {
    uint16_t packetId;
    bool compression;
    uint32_t size;
} BanchoHeader;

BanchoHeader BanchoHeader_Read(WiFiClient client);
void BanchoHeader_Write(BanchoHeader p, BanchoState *bstate);

#endif
