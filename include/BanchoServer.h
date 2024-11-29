#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "BanchoState.h"

BanchoHeader readBanchoPacket(WiFiClient client, char *buf);
LoginPacket getConnectionInfo(WiFiClient client);
void sendUserStats(BanchoState *bstate, uint8_t completness);
void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType);
void sendEmptyPacket(BanchoState *bstate, int packetType);
bool authenticateChoUser(BanchoState *bstate, char *login, char *password);
void banchoTask(void *arg);

#endif
