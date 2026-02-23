#ifndef CHANNELAVAILABLEAUTOJOIN_H
#define CHANNELAVAILABLEAUTOJOIN_H

#include "BanchoState.h"
#include "serialization/Buffer.h"

typedef struct {
    char *channelName;
} ChannelAvailable;

uint32_t ChannelAvailable_Size(ChannelAvailable p);
void ChannelAvailable_Write(const ChannelAvailable *p, Buffer *buf);

#endif
