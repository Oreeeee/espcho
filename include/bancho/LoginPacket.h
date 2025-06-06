#ifndef LOGINPACKET_H
#define LOGINPACKET_H

typedef struct {
    char *username;
    char *password;
    char *clientInfo;
} LoginPacket;

void LoginPacket_Free(LoginPacket *lp);

#endif
