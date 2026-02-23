#include "bancho/LoginPacket.h"
#include <stdlib.h>

void LoginPacket_Free(LoginPacket *lp) {
    free(lp->username);
    free(lp->password);
    free(lp->clientInfo);
}
