#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "constants.h"
#include "bancho/LoginPacket.h"
#include "bancho/LoginReply.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"
#include "bancho/UserStats.h"
#include "bancho/ChannelAvailableAutojoin.h"

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

BanchoHeader readBanchoPacket(WiFiClient client, char *buf) {
    BanchoHeader h = BanchoHeader_Read(client);
    Serial.printf("Received header: %d, %x, %d\n", h.packetId, h.compression, h.size);
    buf = (char*)malloc(h.size); // Doing this makes it not require checking if buf != NULL before free(buf)
    if (h.size) {
        client.readBytes(buf, h.size);
        Serial.printf("Received data: %s\n", buf);
    }
    return h;
}

void sendUserStats(WiFiClient client) {
    UserStats p;
    p.userId = CHO_APPROVED_USERID;
    p.completness = 0;
    p.status = 0;
    p.beatmapUpdate = false;
    //p.statusText = (char*)calloc(1, sizeof(char)); // TODO
    //p.beatmapMD5 = (char*)calloc(1, sizeof(char)); // TODO
    p.mods = 0;

    BanchoHeader h;
    h.packetId = CHO_PACKET_USER_STATS;
    h.compression = false;
    //h.size = UserStats_Size(p);
    h.size = 11; // TODO : Calculate this automatically, dynamic calculations cause crashes

    BanchoHeader_Write(h, client);
    UserStats_Write(p, client);
    client.flush();
}

void sendChannelAutojoin(WiFiClient client, char channelName[]) {
    // ChannelAvailableAutojoin p;
    // p.channelName = (char*)malloc(strlen(channelName) + 1);
    // strncpy(p.channelName, channelName, strlen(channelName) + 1);

    // BanchoHeader h;
    // h.packetId = CHO_PACKET_CHANNEL_AVAILABLE_AUTOJOIN;
    // h.compression = false;
    // //h.size = ChannelAvailableAutojoin_Size(p);
    // h.size = 4;

    // BanchoHeader_Write(h, client);
    // ChannelAvailableAutojoin_Write(p, client);
    // client.flush();
    char *dupa = "\x44\x00\x00\x06\x00\x00\x00\x0b\x04\x23\x6f\x73\x75";
    client.write(dupa);
    client.flush();
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
