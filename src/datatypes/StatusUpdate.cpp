#include "datatypes/StatusUpdate.h"
#include "datatypes/OsuString.h"
#include <string.h>
#include <Arduino.h>

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
