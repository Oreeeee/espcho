#ifndef WRITERS_H
#define WRITERS_H

#include <stdint.h>
#include <stddef.h>
#include "serialization/Buffer.h"

int BufferWriteFloat(Buffer* buf, float value);

int BufferWriteU64(Buffer* buf, uint64_t value);
int BufferWriteU32(Buffer* buf, uint32_t value);
int BufferWriteU16(Buffer* buf, uint16_t value);
int BufferWriteU8(Buffer* buf, uint8_t value);

int BufferWriteS64(Buffer* buf, int64_t value);
int BufferWriteS32(Buffer* buf, int32_t value);
int BufferWriteS16(Buffer* buf, int16_t value);
int BufferWriteS8(Buffer* buf, int8_t value);

int BufferWriteBytes(Buffer* buf, const char* value, size_t len);
int BufferWriteOsuString(Buffer* buf, const char* text);
int BufferWriteUleb128(Buffer* buf, int value);

#endif //WRITERS_H
