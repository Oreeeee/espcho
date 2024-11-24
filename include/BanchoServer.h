#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"

LoginPacket getConnectionInfo(WiFiClient client);
bool authenticateChoUser(WiFiClient client, char *login, char *password);

#endif
