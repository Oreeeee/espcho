#include "bancho/UserStats.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"
#include <WiFi.h>

uint32_t UserStats_Size(UserStats p) {
    return 
        sizeof(p.userId) +
        sizeof(p.completness) +
        sizeof(p.status) +
        sizeof(p.beatmapUpdate) +
        strlen(p.statusText) +
        strlen(p.beatmapMD5) +
        sizeof(p.mods);
}

void UserStats_Write(UserStats p, WiFiClient client) {
    client.write((char*)&p.userId, sizeof(p.userId));
    client.write((char*)&p.completness, sizeof(p.completness));
    client.write((char*)&p.status, sizeof(p.status));
    client.write((char*)&p.beatmapUpdate, sizeof(p.beatmapUpdate));
    client.write("\x00", 1); // TODO
    client.write("\x00", 1); // TODO
    client.write((char*)&p.mods, sizeof(p.mods));
}

char* UserStats_Serialize(UserStats p) {
    char *buf = (char*)malloc(
        sizeof(p.userId) +
        sizeof(p.completness) +
        sizeof(p.status) +
        sizeof(p.beatmapUpdate) +
        sizeof(char) +
        sizeof(char) +
        sizeof(p.mods)
    );

    memcpy(buf, &p.userId, sizeof(p.userId));
    memcpy(buf + sizeof(p.userId), &p.completness, sizeof(p.completness));
    memcpy(buf + sizeof(p.userId) + sizeof(p.completness), &p.status, sizeof(p.status));
    memcpy(buf + sizeof(p.userId) + sizeof(p.completness) + sizeof(p.status), &p.beatmapUpdate, sizeof(p.beatmapUpdate));
    memcpy(buf + sizeof(p.userId) + sizeof(p.completness) + sizeof(p.status) + sizeof(p.beatmapUpdate), "\x00", sizeof(char));
    memcpy(buf + sizeof(p.userId) + sizeof(p.completness) + sizeof(p.status) + sizeof(p.beatmapUpdate) + sizeof(char), "\x00", sizeof(char));
    memcpy(buf + sizeof(p.userId) + sizeof(p.completness) + sizeof(p.status) + sizeof(p.beatmapUpdate) + sizeof(char) + sizeof(char), &p.mods, sizeof(p.mods));

    return buf;
}

void UserStats_Send(UserStats p, WiFiClient client) {
    Serial.println("UserStats_Serialize()");
    char *buf = UserStats_Serialize(p);
    Serial.println("BanchoSendPacket()");
    BanchoSendPacket(CHO_PACKET_USER_STATS, buf, client);
    free(buf);
}
