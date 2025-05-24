#ifndef BANCHOHEADER_H
#define BANCHOHEADER_H

#include <stdint.h>

typedef struct {
    uint16_t packetId;
    bool compression;
    uint32_t size;
} BanchoHeader;

#endif
