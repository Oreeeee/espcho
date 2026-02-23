#include "config.h"
#include "Globals.h"

int getFreeConnectionIndex() {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        if (!connections[i].active) {
            return i;
        }
    }

    return -1;
}
