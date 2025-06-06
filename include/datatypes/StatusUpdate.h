#ifndef STATUSUPDATE_H
#define STATUSUPDATE_H

#include <stdint.h>
#include "serialization/Buffer.h"

typedef struct {
    uint8_t status;
    bool beatmapUpdate;
    char *statusText;
    char *beatmapMD5;
    uint16_t mods;
    uint8_t mode;
    int32_t beatmapID;
} StatusUpdate;

void StatusUpdate_Read(Buffer* buf, StatusUpdate* p);
void StatusUpdate_Write(const StatusUpdate &p, Buffer* buf);
void StatusUpdate_Free(StatusUpdate *p);

#endif
