#include "datatypes/StatusUpdate.h"
#include "datatypes/OsuString.h"
#include <string.h>
#include <Arduino.h>

#include "serialization/Buffer.h"
#include "serialization/Writers.h"

int StatusUpdate_Size(StatusUpdate p) {
    return sizeof(p.status) +
        sizeof(p.beatmapUpdate) +
        OsuStringSize(p.statusText) +
        OsuStringSize(p.beatmapMD5) +
        sizeof(p.mods) +
        sizeof(p.mode) +
        sizeof(p.beatmapID);
}

void StatusUpdate_Serialize(const StatusUpdate &p, Buffer* buf) {
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

StatusUpdate StatusUpdate_Deserialize(char *buf) {
    StatusUpdate p;
    int offset = 0;

    memcpy(&p.status, buf + offset, sizeof(p.status));
    offset += sizeof(p.status);
    Serial.printf("StatusUpdate: Read %d bytes. Value: %02x\n", sizeof(p.status), p.status);
    memcpy(&p.beatmapUpdate, buf + offset, sizeof(p.beatmapUpdate));
    offset += sizeof(p.beatmapUpdate);
    Serial.printf("StatusUpdate: Read %d bytes. Value: %02x\n", sizeof(p.beatmapUpdate), p.beatmapUpdate);
    if (p.beatmapUpdate) {
        int statusTextSize = ReadOsuString(buf + offset, &p.statusText);
        offset += statusTextSize;
        Serial.printf("StatusUpdate: Read %d bytes\n", statusTextSize);
        int beatmapMD5Size = ReadOsuString(buf + offset, &p.beatmapMD5);
        offset += beatmapMD5Size;
        Serial.printf("StatusUpdate: Read %d bytes\n", beatmapMD5Size);
    } else {
        p.statusText = NULL;
        p.beatmapMD5 = NULL;
    }
    memcpy(&p.mods, buf + offset, sizeof(p.mods));
    offset += sizeof(p.mods);
    Serial.printf("StatusUpdate: Read %d bytes. Value: %02x\n", sizeof(p.mods), p.mods);
    memcpy(&p.mode, buf + offset, sizeof(p.mode));
    offset += sizeof(p.mode);
    Serial.printf("StatusUpdate: Read %d bytes. Value: %02x\n", sizeof(p.mode), p.mode);
    memcpy(&p.beatmapID, buf + offset, sizeof(p.beatmapID));
    offset += sizeof(p.beatmapID);
    Serial.printf("StatusUpdate: Read %d bytes. Value: %02x\n", sizeof(p.beatmapID), p.beatmapID);

    return p;
}
