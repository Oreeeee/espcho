#include "bancho/UserStats.h"
#include "constants.h"
#include "BanchoState.h"
#include "datatypes/OsuString.h"
#include <WiFi.h>

uint32_t UserStats_Size(UserStats p) {
    int packetSize = sizeof(p.userId) +
        sizeof(p.completness) +
        sizeof(p.status) +
        sizeof(p.beatmapUpdate) +
        sizeof(p.mods) +
        sizeof(p.mode) +
        sizeof(p.beatmapID);
    
    // Get size of osu! strings
    char *statusText;
    char *beatmapMD5;
    packetSize += WriteOsuString(p.statusText, &statusText) +
        WriteOsuString(p.beatmapMD5, &beatmapMD5);
    
    free(statusText);
    free(beatmapMD5);

    if (p.completness == CHO_STATS_STATISTICS) {
        packetSize += sizeof(p.rankedScore) +
            sizeof(p.accuracy) +
            sizeof(p.playcount) +
            sizeof(p.totalScore) +
            sizeof(p.rank);
    }

    return packetSize;
}

void UserStats_Write(UserStats p, BanchoState *bstate) {
    char *statusText;
    char *beatmapMD5;
    WriteOsuString(p.statusText, &statusText);
    WriteOsuString(p.beatmapMD5, &beatmapMD5);

    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&p.userId, sizeof(p.userId));
            bstate->client.write((char*)&p.completness, sizeof(p.completness));
            bstate->client.write((char*)&p.status, sizeof(p.status));
            bstate->client.write((char*)&p.beatmapUpdate, sizeof(p.beatmapUpdate));
            if (p.beatmapUpdate) {
                bstate->client.write(statusText, strlen(statusText));
                bstate->client.write(beatmapMD5, strlen(beatmapMD5));
            }
            bstate->client.write((char*)&p.mods, sizeof(p.mods));
            bstate->client.write((char*)&p.mode, sizeof(p.mode));
            bstate->client.write((char*)&p.beatmapID, sizeof(p.beatmapID));

            if (p.completness == CHO_STATS_STATISTICS) {
                bstate->client.write((char*)&p.rankedScore, sizeof(p.rankedScore));
                bstate->client.write((char*)&p.accuracy, sizeof(p.accuracy));
                bstate->client.write((char*)&p.playcount, sizeof(p.playcount));
                bstate->client.write((char*)&p.totalScore, sizeof(p.totalScore));
                bstate->client.write((char*)&p.rank, sizeof(p.rank));
            }

            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }

    free(statusText);
    free(beatmapMD5);
}
