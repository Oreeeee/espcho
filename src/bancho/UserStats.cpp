#include "bancho/UserStats.h"
#include "constants.h"
#include "BanchoState.h"
#include "datatypes/OsuString.h"
#include "datatypes/StatusUpdate.h"
#include <WiFi.h>

uint32_t UserStats_Size(UserStats p) {
    int packetSize = sizeof(p.userId) +
        sizeof(p.completness) +
        StatusUpdate_Size(p.statusUpdate);
    
    // Get size of osu! strings
    if (p.completness == CHO_STATS_FULL) {
        packetSize += OsuStringSize(p.username) + OsuStringSize(p.avatarFilename) + OsuStringSize(p.city);
    }

    if (p.completness >= CHO_STATS_STATISTICS) {
        packetSize += sizeof(p.rankedScore) +
            sizeof(p.accuracy) +
            sizeof(p.playcount) +
            sizeof(p.totalScore) +
            sizeof(p.rank);
    }

    if (p.completness == CHO_STATS_FULL) {
        packetSize += sizeof(p.timezone) +
            sizeof(p.permissions) +
            sizeof(p.longitude) +
            sizeof(p.longitude);
    }

    return packetSize;
}

void UserStats_Write(UserStats p, BanchoState *bstate) {
    char *statusUpdate = (char*)malloc(StatusUpdate_Size(p.statusUpdate));
    char *username;
    char *avatarFilename;
    char *city;

    StatusUpdate_Serialize(p.statusUpdate, statusUpdate);

    if (p.completness == CHO_STATS_FULL) {
        WriteOsuString(p.username, &username);
        WriteOsuString(p.avatarFilename, &avatarFilename);
        WriteOsuString(p.city, &city);
    }

    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&p.userId, sizeof(p.userId));
            Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.userId));
            bstate->client.write((char*)&p.completness, sizeof(p.completness));
            Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.completness));
            bstate->client.write(statusUpdate, StatusUpdate_Size(p.statusUpdate));
            Serial.printf("UserStats: Wrote %d bytes\n", StatusUpdate_Size(p.statusUpdate));

            if (p.completness >= CHO_STATS_STATISTICS) {
                bstate->client.write((char*)&p.rankedScore, sizeof(p.rankedScore));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.rankedScore));
                bstate->client.write((char*)&p.accuracy, sizeof(p.accuracy));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.accuracy));
                bstate->client.write((char*)&p.playcount, sizeof(p.playcount));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.playcount));
                bstate->client.write((char*)&p.totalScore, sizeof(p.totalScore));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.totalScore));
                bstate->client.write((char*)&p.rank, sizeof(p.rank));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.rank));
            }

            if (p.completness == CHO_STATS_FULL) {
                bstate->client.write(username, strlen(username));
                Serial.printf("UserStats: Wrote %d bytes\n", strlen(username));
                bstate->client.write(avatarFilename, strlen(avatarFilename));
                Serial.printf("UserStats: Wrote %d bytes\n", strlen(avatarFilename));
                bstate->client.write((char*)&p.timezone, sizeof(p.timezone));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.timezone));
                bstate->client.write(city, strlen(city));
                Serial.printf("UserStats: Wrote %d bytes\n", strlen(city));
                bstate->client.write((char*)&p.permissions, sizeof(p.permissions));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.permissions));
                bstate->client.write((char*)&p.longitude, sizeof(p.longitude));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.longitude));
                bstate->client.write((char*)&p.latitude, sizeof(p.latitude));
                Serial.printf("UserStats: Wrote %d bytes\n", sizeof(p.latitude));
            }

            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }

    free(statusUpdate);
    if (p.completness == CHO_STATS_FULL) {
        free(username);
        free(avatarFilename);
        free(city);
    }
}
