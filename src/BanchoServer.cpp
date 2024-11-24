#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "constants.h"
#include "bancho/LoginPacket.h"
#include "bancho/LoginReply.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"

LoginPacket getConnectionInfo(WiFiClient client) {
    LoginPacket lp;

    lp.username = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));
    lp.password = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));
    lp.clientInfo = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));

    client.readBytesUntil('\r', lp.username, CHO_MAX_LOGIN_STR);
    client.read(); // \n
    client.readBytesUntil('\r', lp.password, CHO_MAX_LOGIN_STR);
    client.read(); // \n
    client.readBytesUntil('\r', lp.clientInfo, CHO_MAX_LOGIN_STR);
    client.read(); // \n
    
    return lp;
}

bool authenticateChoUser(WiFiClient client, char *login, char *password) {
    BanchoHeader h;
    h.packetId = CHO_PACKET_LOGINREPLY;
    h.compression = false;
    h.size = sizeof(LoginReply);

    if (strcmp(CHO_APPROVED_USER, login) == 0 && strcmp(CHO_APPROVED_PASSWORD, password) == 0) {
        LoginReply p;
        p.response = CHO_APPROVED_USERID;
        
        BanchoHeader_Write(h, client);
        LoginReply_Write(p, client);
        client.flush();
        return true;
    }

    LoginReply p;
    p.response = LOGIN_WRONG_PASS;

    BanchoHeader_Write(h, client);
    LoginReply_Write(p, client);
    client.flush();

    return false;
}
