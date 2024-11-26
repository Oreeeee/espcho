#include "bancho/ChannelAvailable.h"
#include "datatypes/OsuString.h"
#include <WiFi.h>
#include <stdint.h>

uint32_t ChannelAvailable_Size(ChannelAvailable p) {
    int packetSize = 0;
    char *channelName;
    packetSize += WriteOsuString(p.channelName, &channelName);
    free(channelName);
    return packetSize;
}

void ChannelAvailable_Write(ChannelAvailable p, WiFiClient client) {
    char *channelName;
    WriteOsuString(p.channelName, &channelName);
    client.write(channelName, strlen(channelName));
    free(channelName);
}