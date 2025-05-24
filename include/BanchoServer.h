#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "BanchoState.h"
#include "BanchoConnection.h"

LoginPacket getConnectionInfo(WiFiClient client);
void sendUserStats(BanchoState *bstate, uint8_t completness, uint16_t version);
void SendBanchoPacket(BanchoState* bstate, uint16_t packetId, const Buffer* buf);
void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType);
bool authenticateChoUser(BanchoState *bstate, char *login, char *password, BanchoConnection *bconn);
void banchoTask(void *arg);
BanchoConnection* GetClientByName(char *name);

#endif
