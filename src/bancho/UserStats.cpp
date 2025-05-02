#include "bancho/UserStats.h"
#include "constants.h"
#include "BanchoState.h"
#include "datatypes/OsuString.h"
#include "datatypes/StatusUpdate.h"
#include <WiFi.h>

#include "serialization/Buffer.h"
#include "serialization/Writers.h"

uint32_t UserStats_Size(UserStats p, uint16_t version) {
    int packetSize = sizeof(p.userId) +
        sizeof(p.completness) +
        StatusUpdate_Size(p.statusUpdate);
    
    // Get size of osu! strings
    if (p.completness == CHO_STATS_FULL) {
        packetSize += OsuStringSize(p.username) + OsuStringSize(p.avatarFilename) + OsuStringSize(p.city);
    }

    if (p.completness >= CHO_STATS_STATISTICS) {
        packetSize += sizeof(p.rankedScore) +
            sizeof(p.accuracy) +
            sizeof(p.playcount) +
            sizeof(p.totalScore) +
            sizeof(p.rank);
    }

    if (p.completness == CHO_STATS_FULL) {
        packetSize += sizeof(p.timezone) +
            sizeof(p.permissions);
        
        if (version >= 1183) { // Clients before b1183 don't send long and lat
            packetSize += sizeof(p.longitude) +
                        sizeof(p.longitude);
        }
    }

    return packetSize;
}

void UserStats_Write(const UserStats &p, BanchoState *bstate, Buffer* buf, const uint16_t version) {
    BufferWriteS32(buf, p.userId);
    BufferWriteU8(buf, p.completness);
    StatusUpdate_Serialize(p.statusUpdate, buf);

    if (p.completness >= CHO_STATS_STATISTICS) {
        BufferWriteS64(buf, p.rankedScore);
        BufferWriteFloat(buf, p.accuracy);
        BufferWriteS32(buf, p.playcount);
        BufferWriteS64(buf, p.totalScore);
        BufferWriteS32(buf, p.rank);
    }

    if (p.completness == CHO_STATS_FULL) {
        BufferWriteOsuString(buf, p.username);
        BufferWriteOsuString(buf, p.avatarFilename);
        BufferWriteU8(buf, p.timezone);
        BufferWriteOsuString(buf, p.city);
        BufferWriteU8(buf, p.permissions);

        if (version >= 1183) { // Clients before b1183 don't send long and lat
            BufferWriteFloat(buf, p.longitude);
            BufferWriteFloat(buf, p.longitude);
        }
    }
}
