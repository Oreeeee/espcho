#include "bancho/BanchoHeader.h"
#include <WiFi.h>

void BanchoHeader_Write(BanchoHeader p, WiFiClient client) {
    client.write((char*)&p.packetId, sizeof(p.packetId));
    client.write((char*)&p.compression, sizeof(p.compression));
    client.write((char*)&p.size, sizeof(p.size));
}
