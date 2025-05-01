#include "serialization/Writers.h"

#include <string.h>
#include <stddef.h>

int BufferWriteFloat(Buffer *buf, float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    return BufferWriteU32(buf, bits);
}

int BufferWriteU64(Buffer* buf, uint64_t value) {
    if (buf->pos + 8 > buf->capacity) return -1;
    buf->data[buf->pos++] = value & 0xFF;
    buf->data[buf->pos++] = (value >> 8) & 0xFF;
    buf->data[buf->pos++] = (value >> 16) & 0xFF;
    buf->data[buf->pos++] = (value >> 24) & 0xFF;
    buf->data[buf->pos++] = (value >> 32) & 0xFF;
    buf->data[buf->pos++] = (value >> 40) & 0xFF;
    buf->data[buf->pos++] = (value >> 48) & 0xFF;
    buf->data[buf->pos++] = (value >> 56) & 0xFF;
    return 0;
}

int BufferWriteU32(Buffer* buf, uint32_t value) {
    if (buf->pos + 4 > buf->capacity) return -1;
    buf->data[buf->pos++] = value & 0xFF;
    buf->data[buf->pos++] = (value >> 8) & 0xFF;
    buf->data[buf->pos++] = (value >> 24) & 0xFF;
    return 0;
}

int BufferWriteU16(Buffer* buf, uint16_t value) {
    if (buf->pos + 2 > buf->capacity) return -1;
    buf->data[buf->pos++] = value & 0xFF;
    buf->data[buf->pos++] = (value >> 8) & 0xFF;
    return 0;
}

int BufferWriteU8(Buffer* buf, uint8_t value) {
    if (buf->pos + 1 > buf->capacity) return -1;
    buf->data[buf->pos++] = value;
    return 0;
}

int BufferWriteS64(Buffer* buf, int64_t value) {
    return BufferWriteU64(buf, (uint64_t)value);
}

int BufferWriteS32(Buffer* buf, int32_t value) {
    return BufferWriteU32(buf, (uint32_t)value);
}

int BufferWriteS16(Buffer* buf, int16_t value) {
    return BufferWriteU16(buf, (uint16_t)value);
}

int BufferWriteS8(Buffer* buf, int8_t value) {
    return BufferWriteU8(buf, (uint8_t)value);
}

int BufferWriteBytes(Buffer *buf, const char *value, size_t len) {
    if (buf->pos + len > buf->capacity) return -1;
    memcpy(buf->data + buf->pos, value, len);
    buf->pos += len;
    return 0;
}

int BufferWriteOsuString(Buffer* buf, const char* text) {
    if (buf->pos + 1 > buf->capacity) return -1;

    // String is empty
    if (text == NULL) {
        BufferWriteU8(buf, '\0'); // 0x00 == no string
        return 0;
    }

    size_t inStringSize = strlen(text);
    BufferWriteU8(buf, '\x20'); // 0x20 == expect string

    // Write the Uleb128 length
    int writeUlebRes = BufferWriteUleb128(buf, inStringSize);
    if (writeUlebRes < 0) return writeUlebRes;

    // Write the string contents
    int writeBytesRes = BufferWriteBytes(buf, text, inStringSize);
    if (writeBytesRes < 0) return writeBytesRes;

    return 0;
}

int BufferWriteUleb128(Buffer* buf, int value) {
    // TODO: Calculate data size before writing to avoid buffer overflows
    do {
        char byte = value & 0x7F;
        value >>= 7;
        if (value != 0) {
            byte |= 0x80;
        }
        buf->data[buf->pos++] = byte;
    } while (value != 0);
    return 0;
}
