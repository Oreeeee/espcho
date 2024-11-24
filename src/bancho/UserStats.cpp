#include "bancho/UserStats.h"
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
