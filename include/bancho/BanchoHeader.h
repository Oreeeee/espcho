#ifndef BANCHOHEADER_H
#define BANCHOHEADER_H

#include <stdint.h>
#include <WiFi.h>

typedef struct {
    uint16_t packetId;
    bool compression;
    uint32_t size;
} BanchoHeader;

void BanchoHeader_Serialize(BanchoHeader p, char *buf);
void BanchoSendPacket(uint32_t packetType, char *input, WiFiClient client);
BanchoHeader BanchoHeader_Read(WiFiClient client);
void BanchoHeader_Write(BanchoHeader p, WiFiClient client);

#endif
