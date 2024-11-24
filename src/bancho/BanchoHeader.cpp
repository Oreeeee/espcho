#include "bancho/BanchoHeader.h"
#include <WiFi.h>

BanchoHeader BanchoHeader_Read(WiFiClient client) {
    BanchoHeader h;
    client.readBytes((char*)&h.packetId, sizeof(h.packetId));
    client.readBytes((char*)&h.compression, sizeof(h.compression));
    client.readBytes((char*)&h.size, sizeof(h.size));
    return h;
}

void BanchoHeader_Write(BanchoHeader p, WiFiClient client) {
    client.write((char*)&p.packetId, sizeof(p.packetId));
    client.write((char*)&p.compression, sizeof(p.compression));
    client.write((char*)&p.size, sizeof(p.size));
}
