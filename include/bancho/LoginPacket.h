#ifndef LOGINPACKET_H
#define LOGINPACKET_H

typedef struct {
    char *username;
    char *password;
    char *clientInfo;
} LoginPacket;

#endif
