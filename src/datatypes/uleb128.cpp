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

int decode_uleb128(char *buffer, int *bytes_read) {
    int value = 0;
    int count = 0;
    char byte;

    do {
        byte = buffer[count];
        value |= (byte & 0x7F) << (7 * count);
        count++;
    } while (byte & 0x80);

    if (bytes_read) {
        *bytes_read = count;
    }
    return value;
}
