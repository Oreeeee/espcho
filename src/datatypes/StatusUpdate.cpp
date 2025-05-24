#include "datatypes/StatusUpdate.h"
#include <string.h>
#include <Arduino.h>

#include "serialization/Buffer.h"
#include "serialization/Readers.h"
#include "serialization/Writers.h"

void StatusUpdate_Write(const StatusUpdate &p, Buffer* buf) {
    BufferWriteU8(buf, p.status);
    BufferWriteU8(buf, p.beatmapUpdate);
    if (p.beatmapUpdate) {
        BufferWriteOsuString(buf, p.statusText);
        BufferWriteOsuString(buf, p.beatmapMD5);
        BufferWriteU16(buf, p.mods);
        BufferWriteU8(buf, p.mode);
        BufferWriteS32(buf, p.beatmapID);
    }
}

void StatusUpdate_Read(Buffer* buf, StatusUpdate* p) {
    BufferReadU8(buf, &p->status);
    BufferReadBool(buf, &p->beatmapUpdate);
    BufferReadOsuString(buf, &p->statusText);
    BufferReadOsuString(buf, &p->beatmapMD5);
    BufferReadU16(buf, &p->mods);
    BufferReadU8(buf, &p->mode);
    BufferReadS32(buf, &p->beatmapID);
}

void StatusUpdate_Free(StatusUpdate* p) {
    free(p->statusText);
    free(p->beatmapMD5);
}
