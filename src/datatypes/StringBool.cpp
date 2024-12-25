#include "datatypes/StringBool.h"
#include <Arduino.h>

bool StringBoolRead(char *in) {
    Serial.printf("StringBoolRead(%s)\n", in);
    if (strcmp(in, "True") == 0) {
        return true;
    }

    return false;
}
