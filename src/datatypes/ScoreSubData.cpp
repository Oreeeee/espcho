#include "datatypes/ScoreSubData.h"
#include "datatypes/StringBool.h"
#include <Arduino.h>

ScoreSubData parseScoreString(char *in) {
    ScoreSubData s;

    // Buffers for strings
    // char fileChecksumTmp[17];
    // char usernameTmp[64];
    // char onlineScoreChecksumTmp[33];
    // char fcTmp[6];
    // char rankingTmp[3];
    // char passTmp[6];

    char fileChecksumTmp[64];
    char usernameTmp[64];
    char onlineScoreChecksumTmp[64];
    char fcTmp[64];
    char rankingTmp[64];
    char passTmp[64];

    // Parse the string, copy ints to their locations and string to temporary buffers
    Serial.println("sscanf()");
    sscanf(
        in,
        "%63[^:]:%63[^:]:%63[^:]:%d:%d:%d:%d:%d:%d:%d:%63[^:]:%63[^:]:%d:%63[^:]:%d:%d:%d",
        //"%s:%s:%s:%d:%d:%d:%d:%d:%d:%d:%d:%s:%s:%d:%s:%d:%d:%d",
        &fileChecksumTmp,
        &usernameTmp,
        &onlineScoreChecksumTmp,
        &s.hit300,
        &s.hit100,
        &s.hit50,
        &s.hitGeki,
        &s.hitKatu,
        &s.hitMiss,
        &s.totalScore,
        &s.maxCombo,
        &fcTmp,
        &rankingTmp,
        &s.mods,
        &passTmp,
        &s.playMode,
        &s.date,
        &s.version
    );

    // Copy temporary strings to the struct
    Serial.println("Copy temporary strings to the struct");
    s.fileChecksum = (char*)calloc(strlen(fileChecksumTmp) + 1, sizeof(char));
    strncpy(s.fileChecksum, fileChecksumTmp, strlen(fileChecksumTmp));

    s.username = (char*)calloc(strlen(usernameTmp) + 1, sizeof(char));
    strncpy(s.username, usernameTmp, strlen(usernameTmp));

    s.onlineScoreChecksum = (char*)calloc(strlen(onlineScoreChecksumTmp) + 1, sizeof(char));
    strncpy(s.onlineScoreChecksum, onlineScoreChecksumTmp, strlen(onlineScoreChecksumTmp));

    s.ranking = (char*)calloc(strlen(rankingTmp) + 1, sizeof(char));
    strncpy(s.ranking, rankingTmp, strlen(rankingTmp));

    // Parse string booleans
    Serial.println("Parse string booleans");
    s.fc = StringBoolRead(fcTmp);
    s.pass = StringBoolRead(passTmp);

    return s;
}
