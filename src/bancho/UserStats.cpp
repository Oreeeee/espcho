#include "bancho/UserStats.h"
#include "constants.h"
#include "BanchoState.h"
#include "datatypes/StatusUpdate.h"

#include "serialization/Buffer.h"
#include "serialization/Writers.h"

void UserStats_Write(const UserStats &p, BanchoState *bstate, Buffer* buf, const uint16_t version) {
    BufferWriteS32(buf, p.userId);
    BufferWriteU8(buf, p.completness);
    StatusUpdate_Write(p.statusUpdate, buf);

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

void UserStats_Free(UserStats *p) {
    free(p->username);
    free(p->avatarFilename);
    free(p->city);
}
