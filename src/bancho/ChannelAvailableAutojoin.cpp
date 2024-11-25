#include "bancho/ChannelAvailableAutojoin.h"
#include <WiFi.h>
#include <stdint.h>

uint32_t ChannelAvailableAutojoin_Size(ChannelAvailableAutojoin p) {
    return strlen(p.channelName);
}

void ChannelAvailableAutojoin_Write(ChannelAvailableAutojoin p, WiFiClient client) {
    client.write(p.channelName, strlen(p.channelName));
}
