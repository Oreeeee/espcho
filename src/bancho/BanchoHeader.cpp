#include "bancho/BanchoHeader.h"
#include "BanchoState.h"
#include <WiFi.h>

BanchoHeader BanchoHeader_Read(WiFiClient client) {
    BanchoHeader h;
    client.readBytes((char*)&h.packetId, sizeof(h.packetId));
    client.readBytes((char*)&h.compression, sizeof(h.compression));
    client.readBytes((char*)&h.size, sizeof(h.size));
    return h;
}

void BanchoHeader_Write(BanchoHeader p, BanchoState *bstate) {
    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&p.packetId, sizeof(p.packetId));
            bstate->client.write((char*)&p.compression, sizeof(p.compression));
            bstate->client.write((char*)&p.size, sizeof(p.size));
            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }
}
