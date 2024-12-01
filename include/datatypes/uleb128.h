#ifndef ULEB128_H
#define ULEB128_h

int encode_uleb128(int value, char **buffer);
int decode_uleb128(char *buffer, int *bytes_read);

#endif
