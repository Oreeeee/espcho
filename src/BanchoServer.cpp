#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "constants.h"
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"
#include "bancho/UserStats.h"
#include "BanchoState.h"
#include "Globals.h"
#include "Pinger.h"
#include "bancho/ChatMessage.h"
#include "chat/ChatManager.h"
#include "serialization/Buffer.h"
#include "serialization/Readers.h"
#include "serialization/Writers.h"

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

int getClientVersion(LoginPacket lp) {
    int version;
    if (sscanf(lp.clientInfo, "b%d", &version) != 1) {
        Serial.println("Failed to get client's version! Assuming b1596");
        return 1596;
    }
    Serial.printf("Got client's version: b%d\n", version);
    return version;
}

void SendBanchoPacket(BanchoState* bstate, uint16_t packetId, const Buffer* buf) {
    bool writing = true;
    uint8_t compression = false;
    uint32_t size = 0;
    while (writing) {
        if (!bstate->writeLock) {
            bstate->writeLock = true;
            bstate->client.write((char*)&packetId, sizeof(packetId));
            bstate->client.write((char*)&compression, sizeof(compression));
            if (buf == NULL) {
                bstate->client.write((char*)&size, sizeof(size));
            } else {
                size = buf->pos;
                bstate->client.write((char*)&size, sizeof(size));
                bstate->client.write(buf->data, size);
            }
            bstate->client.flush();
            bstate->writeLock = false;
            writing = false;
        }
    }
}

void sendUserStats(BanchoState *bstate, uint32_t userId, char *username, StatusUpdate statusUpdate, uint8_t completness, uint16_t version) {
    UserStats p;
    p.userId = userId;
    p.completness = completness;
    p.statusUpdate = statusUpdate;

    if (completness >= CHO_STATS_STATISTICS) {
        p.rankedScore = 20000;
        p.accuracy = 0.9998;
        p.playcount = 0xFFFFFFFF;
        p.totalScore = 200000;
        p.rank = 1;
    }

    if (completness == CHO_STATS_FULL) {
        p.username = (char*)malloc(strlen(username) + 1);
        strncpy(p.username, username, strlen(username) + 1);
        p.avatarFilename = (char*)calloc(2, sizeof(char));
        p.avatarFilename[0] = ' ';
        p.city = "peppy's hidden bunker";
    }

    Serial.println("Buffer buf;");
    Buffer buf;
    Serial.println("CreateBuffer(&buf);");
    CreateBuffer(&buf);

    Serial.println("UserStats_Write(p, bstate, &buf, version);");
    UserStats_Write(p, bstate, &buf, version);
    Serial.println("SendBanchoPacket(bstate, CHO_PACKET_USER_STATS, &buf);");
    SendBanchoPacket(bstate, CHOPKT_USER_STATS, &buf);
}

void sendChannelJoin(BanchoState *bstate, const char *channelName, int packetType) {
    // This packet needs to be of type ChannelAvailable or ChannelAvailableAutojoin
    switch (packetType) {
        case CHOPKT_CHANNEL_AVAILABLE:
        case CHOPKT_CHANNEL_AVAILABLE_AUTOJOIN:
        case CHOPKT_CHANNEL_JOIN_SUCCESS:
            break;
        default:
            Serial.println("Attempted to use wrong type for sendChannelJoin()!");
            return;
    }

    Buffer buf;
    CreateBuffer(&buf);
    BufferWriteOsuString(&buf, channelName);
    SendBanchoPacket(bstate, packetType, &buf);
}

bool authenticateChoUser(BanchoState *bstate, char *login, char *password, BanchoConnection *bconn) {
    Buffer buf;
    CreateBuffer(&buf);

    #ifdef CHO_DISABLE_AUTH
    if (true) {
    #else
    if (strcmp(CHO_APPROVED_USER, login) == 0 && strcmp(CHO_APPROVED_PASSWORD, password) == 0) {
    #endif
        #ifdef CHO_DISABLE_AUTH
        srand(time(NULL));
        uint32_t userId = rand() % 20000 + 1;
        BufferWriteU32(&buf, userId);
        bconn->userId = userId;
        #else
        BufferWriteU32(&buf, CHO_APPROVED_USERID);
        #endif

        SendBanchoPacket(bstate, CHOPKT_LOGINREPLY, &buf);
        BufferFree(&buf);
        return true;
    }

    BufferWriteU32(&buf, LOGIN_WRONG_PASS);
    SendBanchoPacket(bstate, CHOPKT_LOGINREPLY, &buf);
    BufferFree(&buf);

    return false;
}

void setEmptyStatus(BanchoConnection *bconn) {
    bconn->statusUpdate.status = 0;
    bconn->statusUpdate.beatmapUpdate = true;
    bconn->statusUpdate.statusText = NULL;
    bconn->statusUpdate.beatmapMD5 = NULL;
    bconn->statusUpdate.mods = 0;
    bconn->statusUpdate.mode = 0;
    bconn->statusUpdate.beatmapID = 0;
}

void banchoTask(void *arg) {
    Serial.println("Hello from task");
    BanchoConnection *bconn = (BanchoConnection*)arg;
    Serial.println("Got variables");

    if (bconn->client) {
        Serial.println("Trying to get connection info...");
        LoginPacket lp = getConnectionInfo(bconn->client);
        Serial.printf("Username: %s\nPassword: %s\nClient info: %s\n", lp.username, lp.password, lp.clientInfo);

        BanchoState bstate;
        bstate.client = bconn->client;
        bstate.writeLock = false;
        bstate.alive = true;
        bconn->bstate = &bstate;

        Serial.println("Verifying login");
        if (!authenticateChoUser(&bstate, lp.username, lp.password, bconn)) {
            Serial.println("Authentication failed! Server dropping conenction");
            bstate.alive = false;
            bconn->client.stop();
            bconn->active = false;
            free(bconn);

            vTaskDelete(NULL);
        }
        Serial.println("Authentication successful!");
        bconn->version = getClientVersion(lp);
        bconn->username = (char*)malloc(strlen(lp.username) + 1);
        strncpy(bconn->username, lp.username, strlen(lp.username) + 1);

        // Create pinger task
        TaskHandle_t pingerTask;
        xTaskCreate(
            PingClient,
            "Pinger",
            1536,
            &bstate,
            1,
            &pingerTask
        );

        // Send channel list to the client
        for (int i = 0; i < CHANNEL_LIST_AUTOJOIN_LEN; i++) {
            sendChannelJoin(&bstate, ChannelListAutojoin[i], CHOPKT_CHANNEL_AVAILABLE_AUTOJOIN);
        }
        for (int i = 0; i < CHANNEL_LIST_LEN; i++) {
            sendChannelJoin(&bstate, ChannelList[i], CHOPKT_CHANNEL_AVAILABLE);
        }

        // Make client join #osu
        Serial.println("Sending join #osu to client");
        sendChannelJoin(&bstate, "#osu", CHOPKT_CHANNEL_JOIN_SUCCESS);
        Serial.println("Sent #osu request");

        // Set own status
        Serial.println("Setting empty status for current user");
        setEmptyStatus(bconn);

        // Initial user stats send, for some reason osu! doesn't request them when using "Remember password"
        Serial.println("Sending stats on login");
        sendUserStats(&bstate, bconn->userId, bconn->username, bconn->statusUpdate, CHO_STATS_STATISTICS, bconn->version);

        while (bconn->client.connected()) {
            if (bconn->client.available()) {
                Buffer buf;
                CreateBuffer(&buf);

                BanchoHeader h;
                bconn->client.readBytes((char*)&h.packetId, sizeof(h.packetId));
                bconn->client.readBytes((char*)&h.compression, sizeof(h.compression));
                bconn->client.readBytes((char*)&h.size, sizeof(h.size));

                if (h.size >= buf.capacity) {
                    Serial.println("Received more data than buffer can accept!");
                    // TODO Handle that case
                    continue;
                }

                bconn->client.readBytes(buf.data, h.size);

                switch (h.packetId) {
                    case CHOPKT_CHANGE_STATUS:
                        Serial.printf("%d is changing status\n", bconn->userId);
                        StatusUpdate p;
                        StatusUpdate_Read(&buf, &p);
                        bconn->statusUpdate = p;
                        break;
                    case CHOPKT_CLIENT_SEND_MESSAGE:
                        Serial.println("Received message from client");
                        ChatMessage m;
                        ChatMessage_Deserialize(&buf, &m);
                        m.senderId = bconn->userId;
                        m.sender = bconn->username;
                        EnqueueMessage(&m);
                        break;
                    case CHOPKT_CLIENT_MESSAGE_PRIVATE:
                        Serial.println("Received private message from client");
                        ChatMessage pm;
                        ChatMessage_Deserialize(&buf, &pm);
                        pm.senderId = bconn->userId;
                        pm.sender = bconn->username;
                        pm.privateMessage = true;
                        EnqueueMessage(&pm);
                        break;
                    case CHOPKT_REQUEST_STATUS:
                        Serial.println("Received RequestStatus");
                        sendUserStats(&bstate, bconn->userId, bconn->username, bconn->statusUpdate, CHO_STATS_STATISTICS, bconn->version);
                        break;
                    case CHOPKT_RECEIVE_UPDATES:
                        Serial.println("Client wants to receive status updates");
                        for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                            BanchoConnection user = connections[i];
                            if (user.active) {
                                sendUserStats(&bstate, user.userId, user.username, user.statusUpdate, CHO_STATS_FULL, bconn->version);
                            }
                        }
                        break;
                    case CHOPKT_CHANNEL_JOIN:
                        char* channelName;
                        BufferReadOsuString(&buf, &channelName);
                        Serial.printf("Received channel join for %s\n", channelName);
                        /*
                        * TODO: Maybe add the channel name to the list of channels for the connection
                            to not send messages to the clients that are not in that specific channel
                        */
                        sendChannelJoin(&bstate, channelName, CHOPKT_CHANNEL_JOIN_SUCCESS);
                        free(channelName);
                        break;
                    case CHOPKT_PONG:
                        break;
                    default:
                        Serial.printf("Unknown packet received: %d\n", h.packetId);
                }

                //if (buf != NULL)
                BufferFree(&buf);
            }
        }

        Serial.println("Dropping connection!");
        bstate.alive = false;
        Serial.println("Stopping client");
        bconn->client.stop();
        Serial.println("Marking connection as free");
        bconn->active = false;
        Serial.println("Freeing username");
        free(bconn->username);
        // Serial.println("Freeing bconn");
        // free(bconn);

        Serial.println("Exitting task");
        vTaskDelete(NULL);
    }
}
/*
* Gets BanchoConnection by the name
*/
BanchoConnection* GetClientByName(char *name) {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        BanchoConnection *user = &connections[i];
        // TODO: Use safe strncmp
        if (user->client.connected() && strcmp(name, user->username) == 0) {
            return &connections[i];
        }
    }
    return NULL;
}
