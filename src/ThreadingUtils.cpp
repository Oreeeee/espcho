#include "config.h"
#include "Globals.h"

int getFreeBanchoIndex() {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        if (!clients[i].connected()) {
            return i;
        }
    }

    return -1;
}

int getFreeTaskIndex() {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        if (!taskActive[i]) {
            return i;
        }
    }

    return -1;
}
