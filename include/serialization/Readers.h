#ifndef READERS_H
#define READERS_H

#include "Buffer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

int BufferReadFloat(Buffer* buf, float* dest);

int BufferReadU64(Buffer* buf, uint64_t* dest);
int BufferReadU32(Buffer* buf, uint32_t* dest);
int BufferReadU16(Buffer* buf, uint16_t* dest);
int BufferReadU8(Buffer* buf, uint8_t* dest);
int BufferReadBool(Buffer* buf, bool* dest);

int BufferReadS64(Buffer* buf, int64_t* dest);
int BufferReadS32(Buffer* buf, int32_t* dest);
int BufferReadS16(Buffer* buf, int16_t* dest);
int BufferReadS8(Buffer* buf, int8_t* dest);

int BufferReadBytes(Buffer *buf, void *dest, size_t len);
int BufferReadOsuString(Buffer* buf, char** dest);
int BufferReadUleb128(Buffer* buf, int* dest);

#endif //READERS_H
