#ifndef SCORE_SUB_DATA_H
#define SCORE_SUB_DATA_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char *fileChecksum;
    char *username;
    char *onlineScoreChecksum;
    uint16_t hit300;
    uint16_t hit100;
    uint16_t hit50;
    uint16_t hitGeki;
    uint16_t hitKatu;
    uint16_t hitMiss;
    uint32_t totalScore;
    uint16_t maxCombo;
    bool fc;
    char *ranking;
    uint32_t mods;
    bool pass;
    uint8_t playMode;
    uint64_t date;
    uint16_t version;
} ScoreSubData;

ScoreSubData ParseScoreString(char *in);
void FreeScoreSubData(ScoreSubData *s);

#endif
