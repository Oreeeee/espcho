#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"

BanchoHeader readBanchoPacket(WiFiClient client, char *buf);
LoginPacket getConnectionInfo(WiFiClient client);
bool authenticateChoUser(WiFiClient client, char *login, char *password);

#endif
