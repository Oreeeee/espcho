#ifndef STATUSUPDATE_H
#define STATUSUPDATE_H

#include <stdint.h>
#include <stdbool.h>
#include "serialization/Buffer.h"

typedef struct {
    char *statusText;
    char *beatmapMD5;
    int32_t beatmapID;
    uint16_t mods;
    uint8_t status;
    bool beatmapUpdate;
    uint8_t mode;
} StatusUpdate;

void StatusUpdate_Read(Buffer* buf, StatusUpdate* p);
void StatusUpdate_Write(const StatusUpdate *p, Buffer* buf);
void StatusUpdate_Free(StatusUpdate *p);

#endif
