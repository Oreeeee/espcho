#include "serialization/Readers.h"

#include <cstring>
#include <stdint.h>
#include <stddef.h>

int BufferReadFloat(Buffer* buf, float* dest) {
    if (buf->pos + 4 > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], sizeof(float));
    buf->pos += sizeof(float);
    return 0;
}

int BufferReadU64(Buffer* buf, uint64_t* dest) {
    if (buf->pos + 8 > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], sizeof(uint64_t));
    buf->pos += sizeof(uint64_t);
    return 0;
}

int BufferReadU32(Buffer* buf, uint32_t* dest) {
    if (buf->pos + 4 > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], sizeof(uint32_t));
    buf->pos += sizeof(uint32_t);
    return 0;
}

int BufferReadU16(Buffer* buf, uint16_t* dest) {
    if (buf->pos + 2 > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], sizeof(uint16_t));
    buf->pos += sizeof(uint16_t);
    return 0;
}

int BufferReadU8(Buffer* buf, uint8_t* dest) {
    if (buf->pos + 1 > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], sizeof(uint8_t));
    buf->pos += sizeof(uint8_t);
    return 0;
}

int BufferReadBool(Buffer* buf, bool* dest) {
    return BufferReadU8(buf, (uint8_t*)dest);
}

int BufferReadS64(Buffer* buf, int64_t* dest) {
    return BufferReadU64(buf, (uint64_t*)dest);
}

int BufferReadS32(Buffer* buf, int32_t* dest) {
    return BufferReadU32(buf, (uint32_t*)dest);
}

int BufferReadS16(Buffer* buf, int16_t* dest) {
    return BufferReadU16(buf, (uint16_t*)dest);
}

int BufferReadS8(Buffer* buf, int8_t* dest) {
    return BufferReadU8(buf, (uint8_t*)dest);
}

int BufferReadBytes(Buffer *buf, void *dest, size_t len) {
    if (buf->pos + len > buf->capacity) return -1;
    memcpy(dest, &buf->data[buf->pos], len);
    buf->pos += len;
    return 0;
}

int BufferReadOsuString(Buffer* buf, char** dest) {
    uint8_t firstByte;
    BufferReadU8(buf, &firstByte);

    if (firstByte == '\0') {
        // No string to read
        return 0;
    } else if (firstByte != '\x0b') {
        // What the fuck
        return -2;
    }

    int length;
    BufferReadUleb128(buf, &length);

    *dest = (char*)malloc(length);
    strncpy(*dest, (const char*)&buf->data[buf->pos], length);
    buf->pos += length;
    return 0;
}

int BufferReadUleb128(Buffer* buf, int* dest) {
    int value = 0;
    int count = 0;
    char byte;

    do {
        byte = buf->data[count];
        value |= (byte & 0x7F) << (7 * count);
        count++;
    } while (byte & 0x80);

    buf->pos += count;
    *dest = value;
    return 0;
}
