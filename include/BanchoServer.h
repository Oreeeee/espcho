#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "BanchoState.h"
#include "BanchoConnection.h"

BanchoHeader readBanchoPacket(WiFiClient client, char *buf);
LoginPacket getConnectionInfo(WiFiClient client);
void sendUserStats(BanchoState *bstate, uint8_t completness, uint16_t version);
void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType);
void sendEmptyPacket(BanchoState *bstate, int packetType);
bool authenticateChoUser(BanchoState *bstate, char *login, char *password, BanchoConnection *bconn);
void banchoTask(void *arg);

#endif
