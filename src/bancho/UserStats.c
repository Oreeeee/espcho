#include "bancho/UserStats.h"
#include "constants.h"
#include "BanchoState.h"
#include "datatypes/StatusUpdate.h"

#include "serialization/Buffer.h"
#include "serialization/Writers.h"

void UserStats_Write(const UserStats *p, BanchoState *bstate, Buffer* buf, const uint16_t version) {
    BufferWriteS32(buf, p->userId);
    BufferWriteU8(buf, p->completness);
    StatusUpdate_Write(&p->statusUpdate, buf);

    if (p->completness >= CHO_STATS_STATISTICS) {
        BufferWriteS64(buf, p->rankedScore);
        BufferWriteFloat(buf, p->accuracy);
        BufferWriteS32(buf, p->playcount);
        if (version < 659 && p->totalScore >= 17705429348) {
            // Versions between b365 and b659 have an overflow bug when the score is too high.
            // We have to clamp it down for older versions.
            BufferWriteS64(buf, 17705429348);
        } else {
            BufferWriteS64(buf, p->totalScore);
        }

        if (version < 695) {
            BufferWriteU16(buf, p->rank);
        } else {
            BufferWriteS32(buf, p->rank);
        }
    }

    if (p->completness == CHO_STATS_FULL) {
        BufferWriteOsuString(buf, p->username);
        BufferWriteOsuString(buf, p->avatarFilename);
        BufferWriteU8(buf, p->timezone);
        BufferWriteOsuString(buf, p->city);
        BufferWriteU8(buf, p->permissions);

        if (version >= 1183) { // Clients before b1183 don't send long and lat
            BufferWriteFloat(buf, p->longitude);
            BufferWriteFloat(buf, p->longitude);
        }
    }
}

void UserStats_Free(UserStats *p) {
    free(p->username);
    free(p->avatarFilename);
    free(p->city);
}
