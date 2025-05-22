#include "serialization/Buffer.h"

#include <cstdlib>

#include "constants.h"

void CreateBuffer(Buffer *buf) {
    buf->pos = 0;
    buf->capacity = BUFFER_SIZE;
    buf->data = (uint8_t*)calloc(buf->capacity, sizeof(uint8_t));
}

void BufferFree(Buffer *buf) {
    free(buf->data);
}
