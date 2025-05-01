#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include "constants.h"

typedef struct {
    uint16_t pos;
    uint16_t capacity;
    uint8_t data[BUFFER_SIZE];
} Buffer;

void CreateBuffer(Buffer *buf);

#endif //BUFFER_H
