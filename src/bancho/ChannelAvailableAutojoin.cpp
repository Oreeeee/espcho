#include "bancho/ChannelAvailableAutojoin.h"
#include "datatypes/OsuString.h"
#include <WiFi.h>
#include <stdint.h>

uint32_t ChannelAvailableAutojoin_Size(ChannelAvailableAutojoin p) {
    int packetSize = 0;
    char *channelName;
    packetSize += WriteOsuString(p.channelName, &channelName);
    free(channelName);
    return packetSize;
}

void ChannelAvailableAutojoin_Write(ChannelAvailableAutojoin p, WiFiClient client) {
    char *channelName;
    WriteOsuString(p.channelName, &channelName);
    client.write(channelName, strlen(channelName));
    free(channelName);
}
