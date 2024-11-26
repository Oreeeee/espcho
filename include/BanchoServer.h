#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "BanchoState.h"

BanchoHeader readBanchoPacket(WiFiClient client, char *buf);
LoginPacket getConnectionInfo(WiFiClient client);
void sendUserStats(BanchoState *bstate);
void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType);
bool authenticateChoUser(BanchoState *bstate, char *login, char *password);

#endif
