#include "bancho/ChannelAvailable.h"
#include "datatypes/OsuString.h"
#include "BanchoState.h"
#include <WiFi.h>
#include <stdint.h>

uint32_t ChannelAvailable_Size(ChannelAvailable p) {
    int packetSize = 0;
    char *channelName;
    packetSize += WriteOsuString(p.channelName, &channelName);
    free(channelName);
    return packetSize;
}

void ChannelAvailable_Write(ChannelAvailable p, BanchoState *bstate) {
    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            char *channelName;
            WriteOsuString(p.channelName, &channelName);
            bstate->client.write(channelName, strlen(channelName));
            bstate->client.flush();
            free(channelName);
            bstate->writeLock = false;
            writing = false;
        }
    }
    
}
