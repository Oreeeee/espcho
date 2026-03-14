#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include "bancho/BanchoHeader.h"
#include "BanchoState.h"
#include "BanchoConnection.h"

bool performAuthentication(int clientSock);
void sendUserStats(BanchoState *bstate, uint8_t completness, uint16_t version);
void SendBanchoPacket(BanchoState* bstate, uint16_t packetId, const Buffer* buf);
void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType);
void banchoTask(void *arg);
BanchoConnection* GetClientByName(char *name);

#endif
