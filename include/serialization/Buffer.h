#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct {
    uint16_t pos;
    uint16_t capacity;
    uint8_t* data;
} Buffer;

void CreateBuffer(Buffer *buf);
void BufferFree(Buffer *buf);

#endif //BUFFER_H
