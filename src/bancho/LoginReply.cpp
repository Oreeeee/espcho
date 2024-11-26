#include "bancho/LoginReply.h"
#include "BanchoState.h"
#include <WiFi.h>

void LoginReply_Write(LoginReply p, BanchoState *bstate) {
    bool writing = true;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&p.response, sizeof(p.response));
            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }
}
