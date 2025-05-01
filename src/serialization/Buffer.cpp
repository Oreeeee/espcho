#include "serialization/Buffer.h"
#include "constants.h"

void CreateBuffer(Buffer *buf) {
    buf->pos = 0;
    buf->capacity = BUFFER_SIZE;
}
