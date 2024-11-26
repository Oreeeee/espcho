#ifndef USERSTATS_H
#define USERSTATS_H

#include <stdint.h>
#include <WiFi.h>
#include "BanchoState.h"

typedef struct {
    int32_t userId;
    uint8_t completness;
    uint8_t status;
    bool beatmapUpdate;
    char *statusText;
    char *beatmapMD5;
    uint16_t mods;
} UserStats;

uint32_t UserStats_Size(UserStats p);
void UserStats_Write(UserStats p, BanchoState *bstate);

#endif