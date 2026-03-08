#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct {
    uint16_t pos;
    uint16_t capacity;
    uint8_t* data;
} Buffer;

/*
 * Initializes a heap-allocated buffer of specific capacity.
 * Takes in a pointer to an uninitialized buffer.
 * If the capacity is 0, then the value of BUFFER_SIZE is used
 * as the buffer capacity.
 */
void CreateBuffer(Buffer *buf, uint16_t capacity);
void BufferFree(Buffer *buf);

#endif //BUFFER_H
