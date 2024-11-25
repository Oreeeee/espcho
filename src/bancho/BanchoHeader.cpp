#include "bancho/BanchoHeader.h"
#include <WiFi.h>
#include <stdint.h>

uint32_t BanchoHeader_Size(BanchoHeader p) {
    return sizeof(p);
}

char* BanchoHeader_Serialize(BanchoHeader p) {
    char *buf = (char*)malloc(sizeof(p.packetId) + sizeof(p.compression) + sizeof(p.size));

    memcpy(buf, &p.packetId, sizeof(p.packetId));
    memcpy(buf + sizeof(p.packetId), &p.compression, sizeof(p.compression));
    memcpy(buf + sizeof(p.packetId) + sizeof(p.compression), &p.size, sizeof(p.size));

    return buf;
}

void BanchoSendPacket(uint32_t packetType, char *input, WiFiClient client) {
    /* Init header */
    Serial.println("BanchoSendPacket(): Init header");
    BanchoHeader h;
    h.packetId = packetType;
    h.compression = false;
    h.size = strlen(input);

    /* Serialize header */
    Serial.println("BanchoSendPacket(): Serialize header");
    char *headerBuf = BanchoHeader_Serialize(h);

    /* Concat header and data together */
    Serial.println("BanchoSendPacket(): Concat header and data together");
    Serial.printf("headerBuf=%s\n", headerBuf);
    Serial.printf("input=%s\n", input);
    strlen(headerBuf); Serial.println("headerBuf");
    strlen(input); Serial.println("input");
    char finalData[strlen(headerBuf) + strlen(input)]; Serial.println("BanchoSendPacket(): 32");
    strncat(finalData, headerBuf, strlen(headerBuf)); Serial.println("BanchoSendPacket(): 33");
    strncat(finalData, input, strlen(input)); Serial.println("BanchoSendPacket(): 34");

    /* Send all of this to client */
    Serial.println("BanchoSendPacket(): Send all of this to client");
    client.write(finalData, strlen(headerBuf) + strlen(input));
    client.flush();

    free(headerBuf);
}

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
