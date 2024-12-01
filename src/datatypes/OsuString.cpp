#include "datatypes/OsuString.h"
#include "datatypes/uleb128.h"
#include <Arduino.h>
#include <string.h>

int ReadOsuString(char *in, char **out) {
    int offset = 1;

    if (in[0] == '\x00') {
        // Empty string
        *out = NULL;
        return offset;
    } else if (in[0] != '\x0b') {
        // Not a valid osu! string
        Serial.printf("WARNING: %02x is not a valid osu! string!\n", *in);
        *out = NULL;
        return offset;
    }

    int uleb128BytesRead = 0;
    int stringSize = decode_uleb128(in + offset, &uleb128BytesRead);
    offset += uleb128BytesRead;

    *out = (char*)calloc(stringSize + 1, sizeof(char));
    strncpy(*out, in + offset, stringSize);
    offset += stringSize;

    return offset;
}

int WriteOsuString(char *in, char **out) {
    int outStringSize = 1;
    int inStringSize;

    // String is empty
    if (in == NULL) {
        *out = (char*)calloc(1, sizeof(char));
        return outStringSize;
    }

    inStringSize = strlen(in);

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

int OsuStringSize(char *in) {
    int outStringSize = 1;

    if (in == NULL) {
        return outStringSize;
    }

    char *ulebBuf = (char*)calloc(10, sizeof(char));
    int ulebSize = encode_uleb128(strlen(in), &ulebBuf);
    free(ulebBuf);
    outStringSize += ulebSize;
    outStringSize += strlen(in);

    return outStringSize;
}
