#include "datatypes/StatusUpdate.h"
#include "datatypes/OsuString.h"
#include <string.h>

int StatusUpdate_Size(StatusUpdate p) {
    return sizeof(p.status) +
        sizeof(p.beatmapUpdate) +
        OsuStringSize(p.statusText) +
        OsuStringSize(p.beatmapMD5) +
        sizeof(p.mods) +
        sizeof(p.mode) +
        sizeof(p.beatmapID);
}

void StatusUpdate_Serialize(StatusUpdate p, char *buf) {
    char *statusText;
    int statusTextSize = WriteOsuString(p.statusText, &statusText);
    char *beatmapMD5;
    int beatmapMD5Size = WriteOsuString(p.beatmapMD5, &beatmapMD5);

    int offset = 0;
    memcpy(buf + offset, &p.status, sizeof(p.status));
    offset += sizeof(p.status);
    memcpy(buf + offset, &p.beatmapUpdate, sizeof(p.beatmapUpdate));
    offset += sizeof(p.beatmapUpdate);
    memcpy(buf + offset, statusText, statusTextSize);
    offset += statusTextSize;
    memcpy(buf + offset, beatmapMD5, beatmapMD5Size);
    offset += beatmapMD5Size;
    memcpy(buf + offset, &p.mods, sizeof(p.mods));
    offset += sizeof(p.mods);
    memcpy(buf + offset, &p.mode, sizeof(p.mode));
    offset += sizeof(p.mode);
    memcpy(buf + offset, &p.beatmapID, sizeof(p.beatmapID));
    offset += sizeof(p.beatmapID);

    free(statusText);
    free(beatmapMD5);
}
