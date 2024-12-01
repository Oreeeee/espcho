#ifndef STATUSUPDATE_H
#define STATUSUPDATE_H

#include <stdint.h>

typedef struct {
    uint8_t status;
    bool beatmapUpdate;
    char *statusText;
    char *beatmapMD5;
    uint16_t mods;
    uint8_t mode;
    int32_t beatmapID;
} StatusUpdate;

int StatusUpdate_Size(StatusUpdate p);
void StatusUpdate_Serialize(StatusUpdate p, char *buf);
StatusUpdate StatusUpdate_Deserialize(char *buf);

#endif
