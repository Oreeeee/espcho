#ifndef CHANNELAVAILABLEAUTOJOIN_H
#define CHANNELAVAILABLEAUTOJOIN_H

#include <WiFi.h>
#include <stdint.h>

typedef struct {
    char *channelName;
} ChannelAvailableAutojoin;

uint32_t ChannelAvailableAutojoin_Size(ChannelAvailableAutojoin p);
void ChannelAvailableAutojoin_Write(ChannelAvailableAutojoin p, WiFiClient client);

#endif
