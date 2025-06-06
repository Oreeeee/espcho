#include "datatypes/ScoreSubData.h"
#include "datatypes/StringBool.h"
#include <Arduino.h>

ScoreSubData ParseScoreString(char *in) {
    ScoreSubData s;

    // Buffers for strings
    char fileChecksumTmp[64];
    char usernameTmp[64];
    char onlineScoreChecksumTmp[64];
    char fcTmp[64];
    char rankingTmp[64];
    char passTmp[64];

    char inputCopy[512];
    strncpy(inputCopy, in, sizeof(inputCopy));

    char *saveptr = NULL;
    char *token = strtok_r(inputCopy, ":", &saveptr);
    if (token) strncpy(fileChecksumTmp, token, sizeof(fileChecksumTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) strncpy(usernameTmp, token, sizeof(usernameTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) strncpy(onlineScoreChecksumTmp, token, sizeof(onlineScoreChecksumTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hit300 = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hit100 = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hit50 = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hitGeki = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hitKatu = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.hitMiss = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.totalScore = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.maxCombo = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) strncpy(fcTmp, token, sizeof(fcTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) strncpy(rankingTmp, token, sizeof(rankingTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.mods = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) strncpy(passTmp, token, sizeof(passTmp));

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.playMode = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.date = atoi(token);

    token = strtok_r(NULL, ":", &saveptr);
    if (token) s.version = atoi(token);

    // Copy temporary strings to the struct
    //Serial.println("Copy temporary strings to the struct");
    s.fileChecksum = (char*)calloc(strlen(fileChecksumTmp) + 1, sizeof(char));
    strncpy(s.fileChecksum, fileChecksumTmp, strlen(fileChecksumTmp));

    s.username = (char*)calloc(strlen(usernameTmp) + 1, sizeof(char));
    strncpy(s.username, usernameTmp, strlen(usernameTmp));

    s.onlineScoreChecksum = (char*)calloc(strlen(onlineScoreChecksumTmp) + 1, sizeof(char));
    strncpy(s.onlineScoreChecksum, onlineScoreChecksumTmp, strlen(onlineScoreChecksumTmp));

    s.ranking = (char*)calloc(strlen(rankingTmp) + 1, sizeof(char));
    strncpy(s.ranking, rankingTmp, strlen(rankingTmp));

    // Parse string booleans
    //Serial.println("Parse string booleans");
    s.fc = StringBoolRead(fcTmp);
    s.pass = StringBoolRead(passTmp);

    return s;
}

// Free all the strings in ScoreSubData
void FreeScoreSubData(ScoreSubData *s) {
    free(s->fileChecksum);
    free(s->username);
    free(s->onlineScoreChecksum);
    free(s->ranking);
}
