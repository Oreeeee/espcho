#ifndef BANCHOSERVER_H
#define BANCHOSERVER_H

#include <WiFi.h>
#include "bancho/LoginPacket.h"

LoginPacket getConnectionInfo(WiFiClient client);

#endif
