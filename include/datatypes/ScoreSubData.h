#ifndef SCORE_SUB_DATA_H
#define SCORE_SUB_DATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint64_t date;
    char *fileChecksum;
    char *username;
    char *onlineScoreChecksum;
    char *ranking;
    uint32_t totalScore;
    uint32_t mods;
    uint16_t hit300;
    uint16_t hit100;
    uint16_t hit50;
    uint16_t hitGeki;
    uint16_t hitKatu;
    uint16_t hitMiss;
    uint16_t maxCombo;
    uint16_t version;
    bool fc;
    bool pass;
    uint8_t playMode;
} ScoreSubData;

ScoreSubData ParseScoreString(char *in);
void FreeScoreSubData(ScoreSubData *s);

#endif
