#include "bancho/ChannelAvailable.h"
#include "datatypes/OsuString.h"
#include "BanchoState.h"
#include <WiFi.h>
#include <stdint.h>

#include "serialization/Buffer.h"
#include "serialization/Writers.h"

uint32_t ChannelAvailable_Size(ChannelAvailable p) {
    return OsuStringSize(p.channelName);
}

void ChannelAvailable_Write(ChannelAvailable p, Buffer* buf) {
    BufferWriteOsuString(buf, p.channelName);
}
