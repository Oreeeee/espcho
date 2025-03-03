#ifndef USERSTATS_H
#define USERSTATS_H

#include <stdint.h>
#include <WiFi.h>
#include "datatypes/StatusUpdate.h"
#include "BanchoState.h"

/* TODO: Optimise completness */

typedef struct {
    /* StatusOnly */

    int32_t userId;
    uint8_t completness;
    StatusUpdate statusUpdate;

    /* Statistics */

    int64_t rankedScore;
    float accuracy;
    int32_t playcount;
    int64_t totalScore;
    int32_t rank;

    /* Full */

    char *username;
    char *avatarFilename;
    uint8_t timezone;
    char *city;
    uint8_t permissions;
    float longitude;
    float latitude;
} UserStats;

uint32_t UserStats_Size(UserStats p, uint16_t version);
void UserStats_Write(UserStats p, BanchoState *bstate, uint16_t version);

#endif