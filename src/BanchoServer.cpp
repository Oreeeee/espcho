#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "config.h"
#include "constants.h"
#include "bancho/LoginPacket.h"
#include "bancho/LoginReply.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"
#include "bancho/UserStats.h"
#include "bancho/ChannelAvailable.h"
#include "BanchoState.h"
#include "BanchoState2.h"

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

void sendUserStats(BanchoState *bstate, uint8_t completness) {
    UserStats p;
    p.userId = CHO_APPROVED_USERID;
    p.completness = completness;
    p.status = 0;
    p.beatmapUpdate = true;
    p.statusText = (char*)calloc(2, sizeof(char));
    p.statusText[0] = ' ';
    p.beatmapMD5 = (char*)calloc(2, sizeof(char));
    p.beatmapMD5[0] = ' ';
    p.mods = 0;

    if (completness == CHO_STATS_STATISTICS) {
        p.rankedScore = 20000;
        p.accuracy = 0.9998;
        p.playcount = 0xFFFFFFFF;
        p.totalScore = 200000;
        p.rank = 1;
    }

    BanchoHeader h;
    h.packetId = CHO_PACKET_USER_STATS;
    h.compression = false;
    h.size = UserStats_Size(p);
    //h.size = 11; // TODO : Calculate this automatically, dynamic calculations cause crashes

    BanchoHeader_Write(h, bstate);
    UserStats_Write(p, bstate);
}

void sendChannelJoin(BanchoState *bstate, char *channelName, int packetType) {
    // This packet needs to be of type ChannelAvailable or ChannelAvailableAutojoin
    switch (packetType) {
        case CHO_PACKET_CHANNEL_AVAILABLE:
        case CHO_PACKET_CHANNEL_AVAILABLE_AUTOJOIN:
        case CHO_PACKET_CHANNEL_JOIN_SUCCESS:
            break;
        default:
            Serial.println("Attempted to use wrong type for sendChannelJoin()!");
            return;
    }
    
    ChannelAvailable p;
    p.channelName = (char*)malloc(strlen(channelName) + 1);
    strncpy(p.channelName, channelName, strlen(channelName) + 1);

    BanchoHeader h;
    h.packetId = packetType;
    h.compression = false;
    h.size = ChannelAvailable_Size(p);

    BanchoHeader_Write(h, bstate);
    ChannelAvailable_Write(p, bstate);

    free(p.channelName);
}

void sendEmptyPacket(BanchoState *bstate, int packetType) {
    BanchoHeader h;
    h.packetId = packetType;
    h.compression = false;
    h.size = 0;

    BanchoHeader_Write(h, bstate);
}

bool authenticateChoUser(BanchoState *bstate, char *login, char *password) {
    BanchoHeader h;
    h.packetId = CHO_PACKET_LOGINREPLY;
    h.compression = false;
    h.size = sizeof(LoginReply);

    if (strcmp(CHO_APPROVED_USER, login) == 0 && strcmp(CHO_APPROVED_PASSWORD, password) == 0) {
        LoginReply p;
        p.response = CHO_APPROVED_USERID;
        
        BanchoHeader_Write(h, bstate);
        LoginReply_Write(p, bstate);
        return true;
    }

    LoginReply p;
    p.response = LOGIN_WRONG_PASS;

    BanchoHeader_Write(h, bstate);
    LoginReply_Write(p, bstate);

    return false;
}

void handleLoginData(void *arg, AsyncClient *client, void *data, size_t len) {
    BanchoState2 *bstate = (BanchoState2*)arg;

    if (!bstate->authenticated) {
        Serial.printf("Received data: ");
        Serial.write((char*)data, len);

        Serial.println("Copying data to *line");
        char *line = (char*)malloc(len);
        strncpy(line, (char*)data, len);
        
        // Strip data out of \r\n
        Serial.println("Stripping lines");
        line[len - 1] = '\x00';
        line[len - 2] = '\x00';

        switch (bstate->currentlyReadingLine) {
            case 0: // Username
                strncpy(bstate->username, line, len);
                break;
            case 1: // Password
                strncpy(bstate->password, line, len);
                break;
            case 2: // Client info
                strncpy(bstate->clientInfo, line, len);
                Serial.printf("Login packet: %s, %s, %s", bstate->username, bstate->password, bstate->clientInfo);

                free(bstate->username);
                free(bstate->password);
                free(bstate->clientInfo);
                break;
        }
    }
    
}

void handleBanchoConnection(void *arg, AsyncClient *client) {
    Serial.printf("Accepted connection from %s:%d\n", client->remoteIP().toString(), client->remotePort());

    BanchoState2 bstate;

    /*
    client->onError(NULL, NULL);
    client->onDisconnect(NULL, NULL);
    client->onTimeout(NULL, NULL);
    */

    client->onData(&handleLoginData, &bstate);

    /*

    LoginPacket lp = getConnectionInfo(client);
    Serial.printf("Username: %s\nPassword: %s\nClient info: %s\n", lp.username, lp.password, lp.clientInfo);

    BanchoState bstate;
    bstate.client = client;
    bstate.writeLock = false;
    bstate.alive = true;

    Serial.println("Verifying login");
    if (!authenticateChoUser(&bstate, lp.username, lp.password)) {
      Serial.println("Authentication failed! Server dropping conenction");
      bstate.alive = false;
      client.stop();
    }
    Serial.println("Authentication successful!");
    */
}