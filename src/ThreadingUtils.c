#include "config.h"
#include "constants.h"
#include "Globals.h"

int getFreeConnectionIndex() {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        if (!(connections[i].clientFlags & CHO_CONN_FLAG_ACTIVE)) {
            return i;
        }
    }

    return -1;
}
