#ifndef CHANNELAVAILABLEAUTOJOIN_H
#define CHANNELAVAILABLEAUTOJOIN_H

#include <WiFi.h>
#include <stdint.h>

typedef struct {
    char *channelName;
} ChannelAvailable;

uint32_t ChannelAvailable_Size(ChannelAvailable p);
void ChannelAvailable_Write(ChannelAvailable p, WiFiClient client);

#endif