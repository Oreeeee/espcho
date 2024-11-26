#include "bancho/UserStats.h"
#include "BanchoState.h"
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

void UserStats_Write(UserStats p, BanchoState *bstate) {
    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&p.userId, sizeof(p.userId));
            bstate->client.write((char*)&p.completness, sizeof(p.completness));
            bstate->client.write((char*)&p.status, sizeof(p.status));
            bstate->client.write((char*)&p.beatmapUpdate, sizeof(p.beatmapUpdate));
            bstate->client.write("\x00", 1); // TODO
            bstate->client.write("\x00", 1); // TODO
            bstate->client.write((char*)&p.mods, sizeof(p.mods));
            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }
}
