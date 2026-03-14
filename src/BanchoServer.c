#include <lwip/sockets.h>

#include "config.h"
#include "constants.h"
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

int getClientVersion(const char* clientInfo) {
    int version;
    if (sscanf(clientInfo, "b%d", &version) != 1) {
        ESP_LOGE(TAG, "Failed to get client's version! Assuming b1596");
        return 1596;
    }
    ESP_LOGD(TAG, "Got client's version: b%d", version);
    return version;
}

bool initializeConnection(BanchoConnection* bconn) {
    int sock = bconn->clientSock;

    char username[CHO_MAX_LOGIN_STR];
    char password[CHO_MAX_LOGIN_STR];
    char clientInfo[CHO_MAX_LOGIN_STR];
    char dummy;

    recv_until(sock, '\r', username, CHO_MAX_LOGIN_STR);
    recv(sock, &dummy, 1, 0); // \n
    recv_until(sock, '\r', password, CHO_MAX_LOGIN_STR);
    recv(sock, &dummy, 1, 0); // \n
    recv_until(sock, '\r', clientInfo, CHO_MAX_LOGIN_STR);
    recv(sock, &dummy, 1, 0); // \n

    ESP_LOGD(TAG, "Username: %s\nPassword: %s\nClient info: %s", username, password, clientInfo);

#ifdef CHO_DISABLE_AUTH
    if (true) {
#else
    if (strcmp(CHO_APPROVED_USER, username) == 0 && strcmp(CHO_APPROVED_PASSWORD, password) == 0) {
#endif
#ifdef CHO_DISABLE_AUTH
        uint32_t userId = esp_random() % 20000 + 1;
        bconn->userId = userId;
#else
        bconn->userId = CHO_APPROVED_USERID;
#endif
    } else {
        ESP_LOGI(TAG, "User authentication failed!");
        return false;
    }

    bconn->version = getClientVersion(clientInfo);
    bconn->username = malloc(strlen(username) + 1);
    strncpy(bconn->username, username, strlen(username) + 1);
    
    return true;
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

void sendUserStats(BanchoState *bstate, uint32_t userId, const char *username, StatusUpdate statusUpdate, uint8_t completness, uint16_t version) {
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
    CreateBuffer(&buf, 0);

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
    CreateBuffer(&buf, 64);
    BufferWriteOsuString(&buf, channelName);
    SendBanchoPacket(bstate, packetType, &buf);
    BufferFree(&buf);
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

/*
 * Broadcasts status updates to all clients that stated they want
 * to receive status updates.
 */
void broadcactStatusUpdate(uint32_t userId, const char *username, const StatusUpdate *update, uint8_t completeness) {
    ESP_LOGI(TAG, "Broadcasting status update of %s", username);

    xSemaphoreTake(connMutex, portMAX_DELAY);
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        if (!(connections[i].clientFlags & CHO_CONN_FLAG_RECEIVE_STATUSES)) {
            continue;
        }
        ESP_LOGI(TAG, "Broadcasting to %s", connections[i].username);
        sendUserStats(connections[i].bstate, userId, username, *update, completeness, connections[i].version);
    }
    xSemaphoreGive(connMutex);
}

/*
 * Sends all statuses of every user to the bconn
 */
void getAllUserStats(const BanchoConnection *bconn) {
    xSemaphoreTake(connMutex, portMAX_DELAY);

    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        BanchoConnection *different = &connections[i];
        if (!(different->clientFlags & CHO_CONN_FLAG_ACTIVE)) {
            continue;
        }

        sendUserStats(bconn->bstate, different->userId, different->username, different->statusUpdate, CHO_STATS_STATUS, bconn->version);
    }

    xSemaphoreGive(connMutex);
}

BanchoConnection* GetClientById(uint32_t id) {
    for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
        BanchoConnection *user = &connections[i];
        if (user->clientFlags & CHO_CONN_FLAG_ACTIVE && user->userId == id) {
            return &connections[i];
        }
    }
    return NULL;
}

void banchoTask(void *arg) {
    BanchoConnection *bconn = (BanchoConnection*)arg;
    Buffer buf;

    BanchoState bstate;
    bstate.clientSock = bconn->clientSock;
    bstate.writeLock = xSemaphoreCreateMutex();
    bstate.alive = true;
    bconn->bstate = &bstate;

    ESP_LOGD(TAG, "Trying to get connection info...");
    CreateBuffer(&buf, sizeof(int32_t));
    if (!initializeConnection(bconn)) {
        ESP_LOGI(TAG, "Authentication failed! Server dropping conenction");
        BufferWriteU32(&buf, LOGIN_WRONG_PASS);
        SendBanchoPacket(bconn->bstate, CHOPKT_LOGINREPLY, &buf);
        BufferFree(&buf);

        close(bconn->clientSock);
        bconn->clientFlags &= ~CHO_CONN_FLAG_ACTIVE;
        free(bconn);

        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Authentication successful!");
    BufferWriteU32(&buf, bconn->userId);
    SendBanchoPacket(bconn->bstate, CHOPKT_LOGINREPLY, &buf);
    // Buffer not freed: we're reusing it to send permissions later on

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

    // Send out statistics and every other user ID
    ESP_LOGD(TAG, "Sending stats on login");
    sendUserStats(&bstate, bconn->userId, bconn->username, bconn->statusUpdate, CHO_STATS_STATISTICS, bconn->version);
    getAllUserStats(bconn);

    // Broadcast own status to all users
    broadcactStatusUpdate(bconn->userId, bconn->username, &bconn->statusUpdate, CHO_STATS_FULL);

    ESP_LOGD(TAG, "Sending permissions");
    BufferWriteU32(&buf, PERM_ALL);
    SendBanchoPacket(&bstate, CHOPKT_USER_PERMISSIONS, &buf);
    BufferFree(&buf); // Buffer freed because 4 bytes is not enough for us later on

    // Create out packet recv buffer
    // We're reusing the previous stack Buffer object to not bloat up the stack
    CreateBuffer(&buf, 0);

    while (bconn->clientFlags & CHO_CONN_FLAG_ACTIVE) {
        BanchoHeader h;
        ssize_t idRecvSize = recv(bconn->clientSock, &h.packetId, sizeof(h.packetId), 0);
        if (idRecvSize <= 0) {
            bconn->clientFlags &= ~CHO_CONN_FLAG_ACTIVE;
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
                broadcactStatusUpdate(bconn->userId, bconn->username, &bconn->statusUpdate, CHO_STATS_STATUS);
                break;
            case CHOPKT_CLIENT_EXIT:
                bconn->clientFlags &= ~CHO_CONN_FLAG_ACTIVE;
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
                bconn->clientFlags |= CHO_CONN_FLAG_RECEIVE_STATUSES;
                for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                    BanchoConnection user = connections[i];
                    if (user.clientFlags & CHO_CONN_FLAG_ACTIVE) {
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
            case CHOPKT_START_SPECTATING:
                ;uint32_t targetPlayer = 0;
                BufferReadU32(&buf, &targetPlayer);
                ESP_LOGI(TAG, "%d wants to spectate %d", bconn->userId, targetPlayer);

                // Inform the client that it's being spectated
                BanchoConnection* targetConnection = GetClientById(targetPlayer);
                if (targetConnection == NULL) {
                    ESP_LOGE(TAG, "Couldn't find target player %d to spectate");
                    break;
                }
                Buffer outBuf;
                CreateBuffer(&outBuf, sizeof(uint32_t));
                BufferWriteU32(&outBuf, bconn->userId);
                SendBanchoPacket(targetConnection->bstate, CHOPKT_SPECTATOR_JOINED, &outBuf);

                // Inform other spectators of our presence with fellow spectator joined packet
                xSemaphoreTake(connMutex, portMAX_DELAY);
                for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                    BanchoConnection* user = &connections[i];
                    if (user->spectatingPlayer != targetPlayer) {
                        continue;
                    }
                    ESP_LOGI(TAG, "Notifying fellow spectator %d of %d's presence", user->userId, bconn->userId);
                    SendBanchoPacket(user->bstate, CHOPKT_FELLOW_SPECTATOR_JOINED, &outBuf);
                }
                xSemaphoreGive(connMutex);

                BufferFree(&outBuf);

                bconn->spectatingPlayer = targetPlayer;
                break;
            case CHOPKT_STOP_SPECTATING:
                ;uint32_t spectatedPlayer = bconn->spectatingPlayer;

                ESP_LOGI(TAG, "%d is no longer spectating %d", bconn->userId, spectatedPlayer);
                BanchoConnection* spectatedConnection = GetClientById(spectatedPlayer);
                if (spectatedConnection == NULL) {
                    ESP_LOGE(TAG, "Couldn't get target player by id!");
                    break;
                }

                // Notify the host about the spectator leaving
                Buffer outBuf1;
                CreateBuffer(&outBuf1, sizeof(uint32_t));
                BufferWriteU32(&outBuf1, bconn->userId);
                SendBanchoPacket(spectatedConnection->bstate, CHOPKT_SPECTATOR_LEFT, &outBuf1);

                // Inform other spectators about us leaving with fellow spectator left packet
                xSemaphoreTake(connMutex, portMAX_DELAY);
                for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                    BanchoConnection* user = &connections[i];
                    if (user->spectatingPlayer != targetPlayer) {
                        continue;
                    }
                    ESP_LOGI(TAG, "Notifying fellow spectator %d about %d leaving", user->userId, bconn->userId);
                    SendBanchoPacket(user->bstate, CHOPKT_FELLOW_SPECTATOR_LEFT, &outBuf);
                }
                xSemaphoreGive(connMutex);

                BufferFree(&outBuf1);

                xSemaphoreTake(connMutex, portMAX_DELAY);
                bconn->spectatingPlayer = 0;
                xSemaphoreGive(connMutex);

                break;
            case CHOPKT_SEND_FRAMES:
                ESP_LOGI(TAG, "Received spectator frames from client");
                buf.pos = h.size; // SendBanchoPacket sends buf.pos bytes and reading the bancho packet leaves the pos at 0 so we need to set it manually

                // Broadcast the frames to every client that's spectating the current client
                // TODO: Don't block the client's Bancho thread when doing this
                // TODO: Find a better way than an O(n) loop :D
                xSemaphoreTake(connMutex, portMAX_DELAY);
                for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                    BanchoConnection* user = &connections[i];
                    if (user->spectatingPlayer != bconn->userId) {
                        continue;
                    }

                    SendBanchoPacket(user->bstate, CHOPKT_SPECTATE_FRAMES, &buf);
                }
                xSemaphoreGive(connMutex);
                break;
            case CHOPKT_CANT_SPECTATE:
                ESP_LOGI(TAG, "%d can't spectate", bconn->userId);
                Buffer outBuf2;
                CreateBuffer(&outBuf2, sizeof(uint32_t));
                BufferWriteU32(&outBuf2, bconn->userId);

                xSemaphoreTake(connMutex, portMAX_DELAY);
                for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                    BanchoConnection* user = &connections[i];
                    if (user->spectatingPlayer != bconn->userId) {
                        continue;
                    }

                    SendBanchoPacket(user->bstate, CHOPKT_SPECTATOR_CANT_SPECTATE, &outBuf2);
                }
                xSemaphoreGive(connMutex);

                break;
            case CHOPKT_ERROR_REPORT:
                ESP_LOGW(TAG, "Received error report from %d", bconn->userId);
                char *error;
                BufferReadOsuString(&buf, &error);
                ESP_LOGW(TAG, "%s", error);
                free(error);
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
    xSemaphoreTake(connMutex, portMAX_DELAY);
    ESP_LOGI(TAG, "Stopping client");
    close(bconn->clientSock);
    ESP_LOGI(TAG, "Marking connection as free");
    bconn->clientFlags &= ~CHO_CONN_FLAG_ACTIVE;
    ESP_LOGI(TAG, "Freeing username");
    free(bconn->username);
    ESP_LOGI(TAG, "Freeing status");
    StatusUpdate_Free(&bconn->statusUpdate);
    xSemaphoreGive(connMutex);
    ESP_LOGI(TAG, "Freeing buffer");
    BufferFree(&buf);

    bconn->clientFlags = 0;
    bconn->spectatingPlayer = 0;

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
        if (user->clientFlags & CHO_CONN_FLAG_ACTIVE && strcmp(name, user->username) == 0) {
            return &connections[i];
        }
    }
    return NULL;
}
