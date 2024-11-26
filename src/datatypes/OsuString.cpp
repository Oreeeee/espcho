#include "datatypes/OsuString.h"
#include "datatypes/uleb128.h"
#include <Arduino.h>
#include <string.h>

int WriteOsuString(char *in, char **out) {
    int outStringSize = 1;
    int inStringSize;

    // Check if passed string is a NULL pointer
    if (in == NULL) {
        *out = (char*)calloc(1, sizeof(char));
        return outStringSize;
    }

    inStringSize = strlen(in);
    
    if (inStringSize == 0) {
        *out = (char*)calloc(1, sizeof(char));
        return outStringSize;
    }

    // Create osu! string
    *out = (char*)calloc(inStringSize + 12, sizeof(char));
    *out[0] = 0x0b; // osu! strings start with 0x0b if they are not empty
    // Don't outStringSize++ here, it's already included in the first line

    // String size
    char *ulebBuf = (char*)calloc(10, sizeof(char));
    int ulebSize = encode_uleb128(strlen(in), &ulebBuf);
    memcpy(*out + 1, ulebBuf, ulebSize);
    free(ulebBuf);
    outStringSize += ulebSize;

    // String contents
    memcpy(*out + 1 + ulebSize, in, strlen(in));
    outStringSize += strlen(in);

    return outStringSize;
}
