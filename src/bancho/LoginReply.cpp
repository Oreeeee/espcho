#include "bancho/LoginReply.h"
#include <WiFi.h>

void LoginReply_Write(LoginReply p, WiFiClient client) {
    client.write((char*)&p.response, sizeof(p.response));
}
