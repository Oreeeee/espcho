#include "datatypes/StringBool.h"

#define LOCAL_LOG_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
static const char *TAG = "StringBool";

bool StringBoolRead(char *in) {
    ESP_LOGV(TAG, "StringBoolRead(%s)", in);
    if (strcmp(in, "True") == 0) {
        return true;
    }

    return false;
}
