#include "datatypes/uleb128.h"

int encode_uleb128(int value, char **buffer) {
    int count = 0;
    do {
        char byte = value & 0x7F;
        value >>= 7;
        if (value != 0) {
            byte |= 0x80;
        }
        *buffer[count++] = byte;
    } while (value != 0);
    return count;
}
