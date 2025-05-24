#include "bancho/ChannelAvailable.h"
#include "serialization/Buffer.h"
#include "serialization/Writers.h"

void ChannelAvailable_Write(ChannelAvailable p, Buffer* buf) {
    BufferWriteOsuString(buf, p.channelName);
}
