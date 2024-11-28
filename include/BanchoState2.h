#ifndef BANCHOSTATE2_H
#define BANCHOSTATE2_H

#include <stdint.h>

typedef struct {
    bool authenticated;
    char *username;
    char *password;
    char *clientInfo;
    uint8_t currentlyReadingLine;
} BanchoState2;

#endif
