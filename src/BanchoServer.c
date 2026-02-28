#include <lwip/sockets.h>

#include "config.h"
#include "constants.h"
#include "bancho/LoginPacket.h"
#include "bancho/BanchoHeader.h"
#include "bancho/BanchoPackets.h"
#include "bancho/UserStats.h"
#include "BanchoState.h"
#include "Globals.h"
#include "bancho/ChatMessage.h"
#include "bancho/Permissions.h"
#include "chat/ChatManager.h"
#include "serialization/Buffer.h"
#include "serialization/Readers.h"
#include "serialization/Writers.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char* TAG = "BanchoServer";

ssize_t recv_until(int sock, char terminator, char *buffer, size_t maxlen) {
    size_t i = 0;
    char c;

    while (i < maxlen) {
        ssize_t n = recv(sock, &c, 1, 0);
        if (n <= 0) {
            return (n == 0 && i > 0) ? i : -1;
        }

        buffer[i++] = c;
        if (c == terminator) {
            buffer[--i] = '\0'; // Do not include a terminator in there
            break;
        }
    }

    return i;
}

LoginPacket getConnectionInfo(int clientSock) {
    LoginPacket lp;

    lp.username = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));
    lp.password = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));
    lp.clientInfo = (char*)calloc(CHO_MAX_LOGIN_STR, sizeof(char));
    char dummy;

    recv_until(clientSock, '\r', lp.username, CHO_MAX_LOGIN_STR);
    recv(clientSock, &dummy, 1, 0); // \n
    recv_until(clientSock, '\r', lp.password, CHO_MAX_LOGIN_STR);
    recv(clientSock, &dummy, 1, 0); // \n
    recv_until(clientSock, '\r', lp.clientInfo, CHO_MAX_LOGIN_STR);
    recv(clientSock, &dummy, 1, 0); // \n
    
    return lp;
}

int getClientVersion(LoginPacket lp) {
    int version;
    if (sscanf(lp.clientInfo, "b%d", &version) != 1) {
        ESP_LOGE(TAG, "Failed to get client's version! Assuming b1596");
        return 1596;
    }
    ESP_LOGD(TAG, "Got client's version: b%d", version);
    return version;
}

void SendBanchoPacket(BanchoState* bstate, uint16_t packetId, const Buffer* buf) {
    uint8_t compression = false;
    uint32_t size = 0;

    xSemaphoreTake(bstate->writeLock, portMAX_DELAY);

    send(bstate->clientSock, &packetId, sizeof(packetId), 0);
    send(bstate->clientSock, &compression, sizeof(compression), 0);
    if (buf == NULL) {
        send(bstate->clientSock, &size, sizeof(size), 0);
    } else {
        size = buf->pos;
        send(bstate->clientSock, &size, sizeof(size), 0);
        send(bstate->clientSock, buf->data, size, 0);
    }
    //bstate->client.flush();

    xSemaphoreGive(bstate->writeLock);
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
        p.username = (char*)calloc(strlen(username) + 1, sizeof(char));
        p.permissions = PERM_ALL;
        strncpy(p.username, username, strlen(username));
        p.avatarFilename = (char*)calloc(2, sizeof(char));
        p.avatarFilename[0] = ' ';
        p.city = "peppy's hidden bunker";
        p.timezone = 0;
    }

    Buffer buf;
    CreateBuffer(&buf);

    UserStats_Write(&p, bstate, &buf, version);
    SendBanchoPacket(bstate, CHOPKT_USER_STATS, &buf);
    //UserStats_Free(&p); // TODO
}

void sendChannelJoin(BanchoState *bstate, const char *channelName, int packetType) {
    // This packet needs to be of type ChannelAvailable or ChannelAvailableAutojoin
    switch (packetType) {
        case CHOPKT_CHANNEL_AVAILABLE:
        case CHOPKT_CHANNEL_AVAILABLE_AUTOJOIN:
        case CHOPKT_CHANNEL_JOIN_SUCCESS:
            break;
        default:
            ESP_LOGE(TAG, "Attempted to use wrong type for sendChannelJoin()!");
            return;
    }

    Buffer buf;
    CreateBuffer(&buf);
    BufferWriteOsuString(&buf, channelName);
    SendBanchoPacket(bstate, packetType, &buf);
    BufferFree(&buf);
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
        BufferWriteS32(&buf, userId);
        bconn->userId = userId;
        #else
        BufferWriteU32(&buf, CHO_APPROVED_USERID);
        #endif

        SendBanchoPacket(bstate, CHOPKT_LOGINREPLY, &buf);
        BufferFree(&buf);
        return true;
    }

    BufferWriteS32(&buf, LOGIN_WRONG_PASS);
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
    BanchoConnection *bconn = (BanchoConnection*)arg;

    ESP_LOGD(TAG, "Trying to get connection info...");
    LoginPacket lp = getConnectionInfo(bconn->clientSock);
    ESP_LOGD(TAG, "Username: %s\nPassword: %s\nClient info: %s", lp.username, lp.password, lp.clientInfo);

    BanchoState bstate;
    //bstate.client = bconn->client;
    bstate.clientSock = bconn->clientSock;
    bstate.writeLock = xSemaphoreCreateMutex();
    bstate.alive = true;
    bconn->bstate = &bstate;

    ESP_LOGD(TAG, "Verifying login");
    if (!authenticateChoUser(&bstate, lp.username, lp.password, bconn)) {
        ESP_LOGI(TAG, "Authentication failed! Server dropping conenction");
        bstate.alive = false;
        close(bconn->clientSock);
        bconn->active = false;
        free(bconn);

        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Authentication successful!");
    bconn->version = getClientVersion(lp);
    bconn->username = (char*)malloc(strlen(lp.username) + 1);
    strncpy(bconn->username, lp.username, strlen(lp.username) + 1);

    LoginPacket_Free(&lp);

    // Send channel list to the client
    for (int i = 0; i < CHANNEL_LIST_AUTOJOIN_LEN; i++) {
        sendChannelJoin(&bstate, ChannelListAutojoin[i], CHOPKT_CHANNEL_AVAILABLE_AUTOJOIN);
    }
    for (int i = 0; i < CHANNEL_LIST_LEN; i++) {
        sendChannelJoin(&bstate, ChannelList[i], CHOPKT_CHANNEL_AVAILABLE);
    }

    // Make client join #osu
    ESP_LOGD(TAG, "Sending join #osu to client");
    sendChannelJoin(&bstate, "#osu", CHOPKT_CHANNEL_JOIN_SUCCESS);
    ESP_LOGD(TAG, "Sent #osu request");

    // Set own status
    ESP_LOGD(TAG, "Setting empty status for current user");
    setEmptyStatus(bconn);

    // Initial user stats send, for some reason osu! doesn't request them when using "Remember password"
    ESP_LOGD(TAG, "Sending stats on login");
    sendUserStats(&bstate, bconn->userId, bconn->username, bconn->statusUpdate, CHO_STATS_STATISTICS, bconn->version);

    ESP_LOGD(TAG, "Sending permissions");
    Buffer permBuf;
    CreateBuffer(&permBuf);
    BufferWriteU32(&permBuf, PERM_ALL);
    SendBanchoPacket(&bstate, CHOPKT_USER_PERMISSIONS, &permBuf);
    free(permBuf.data);

    /* Create out packet recv buffer */
    Buffer buf;
    CreateBuffer(&buf);

    while (bconn->active) {
        BanchoHeader h;
        ssize_t idRecvSize = recv(bconn->clientSock, &h.packetId, sizeof(h.packetId), 0);
        if (idRecvSize <= 0) {
            bconn->active = false;
        }
        recv(bconn->clientSock, &h.compression, sizeof(h.compression), 0);
        recv(bconn->clientSock, &h.size, sizeof(h.size), 0);

        if (h.size >= buf.capacity) {
            ESP_LOGW(TAG, "Received more data than buffer can accept!");
            // TODO Handle that case
            //continue;
            goto cleanup_connection;
        }

        recv(bconn->clientSock, buf.data, h.size, 0);

        switch (h.packetId) {
            case CHOPKT_CHANGE_STATUS:
                ESP_LOGI(TAG, "%d is changing status", bconn->userId);
                StatusUpdate p;
                StatusUpdate_Read(&buf, &p);
                bconn->statusUpdate = p;
                break;
            case CHOPKT_CLIENT_EXIT:
                bconn->active = false;
                break;
            case CHOPKT_CLIENT_SEND_MESSAGE:
                ESP_LOGI(TAG, "Received message from client");
                ChatMessage m;
                ChatMessage_Deserialize(&buf, &m);
                m.senderId = bconn->userId;
                m.sender = bconn->username;
                EnqueueMessage(&m);
                break;
            case CHOPKT_CLIENT_MESSAGE_PRIVATE:
                ESP_LOGI(TAG, "Received private message from client");
                ChatMessage pm;
                ChatMessage_Deserialize(&buf, &pm);
                pm.senderId = bconn->userId;
                pm.sender = bconn->username;
                pm.privateMessage = true;
                EnqueueMessage(&pm);
                break;
            case CHOPKT_REQUEST_STATUS:
                ESP_LOGI(TAG, "Received RequestStatus");
                sendUserStats(&bstate, bconn->userId, bconn->username, bconn->statusUpdate, CHO_STATS_STATISTICS, bconn->version);
                break;
            case CHOPKT_RECEIVE_UPDATES:
                ESP_LOGI(TAG, "Client wants to receive status updates");
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
                ESP_LOGI(TAG, "Received channel join for %s", channelName);
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
                ESP_LOGE(TAG, "Unknown packet received: %d", h.packetId);
        }

        buf.pos = 0;
    }

    cleanup_connection:
    ESP_LOGI(TAG, "Dropping connection!");
    bstate.alive = false;
    ESP_LOGI(TAG, "Stopping client");
    close(bconn->clientSock);
    ESP_LOGI(TAG, "Marking connection as free");
    bconn->active = false;
    ESP_LOGI(TAG, "Freeing username");
    free(bconn->username);
    ESP_LOGI(TAG, "Freeing status");
    StatusUpdate_Free(&bconn->statusUpdate);
    ESP_LOGI(TAG, "Freeing buffer");
    BufferFree(&buf);

    ESP_LOGI(TAG, "Exitting task");
    vTaskDelete(NULL);
}
/*
* Gets BanchoConnection by the name
*/
BanchoConnection* GetClientByName(char *name) {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        BanchoConnection *user = &connections[i];
        // TODO: Use safe strncmp
        if (user->active && strcmp(name, user->username) == 0) {
            return &connections[i];
        }
    }
    return NULL;
}
