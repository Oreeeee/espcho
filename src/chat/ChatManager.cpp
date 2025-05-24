#include "chat/ChatManager.h"
#include <freertos/FreeRTOS.h>
#include <Arduino.h>

#include "BanchoServer.h"
#include "config.h"
#include "Globals.h"
#include "bancho/BanchoPackets.h"

QueueHandle_t ChatQueue = NULL;

// Creates a queue and task for the chat queue
void ChatInit() {
    ChatQueue = xQueueCreate(10, sizeof(ChatMessage*));
    xTaskCreate(ChatLoop, "ChatLoop", 2048, NULL, 1, NULL);
}

// Loop for handling the chat messages
void ChatLoop(void *args) {
    while (true) {
        ChatMessage *msg;
        xQueueReceive(ChatQueue, &msg, portMAX_DELAY);
        Serial.printf("Received message inside loop: %s\n", msg->message);

        // Make a copy of the username to not free the actual pointer
        size_t usernameSize = strlen(msg->sender);
        char *usernameTemp = (char*)malloc(usernameSize + 1);
        strncpy(usernameTemp, msg->sender, usernameSize);
        usernameTemp[usernameSize] = '\0';
        msg->sender = usernameTemp;

        BanchoConnection *bconn = NULL;
        if (msg->privateMessage) {
            // Private message, send only to one specific client
            bconn = GetClientByName(msg->target);
            if (bconn == NULL) {
                Serial.println("Cannot find target to send a private message");
            } else {
                SendMessage(msg, bconn);
            }
        } else {
            // Public message, send to all clients
            for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
                bconn = &connections[i];
                // Send messages only to relevant clients
                if (bconn->client.connected() && bconn->userId != msg->senderId) {
                    SendMessage(msg, bconn);
                }
            }
        }

        ChatMessage_Free(msg);
    }
}

// Send message to a specified client
void SendMessage(ChatMessage *msg, BanchoConnection *bconn) {
    Buffer buf;
    CreateBuffer(&buf);
    ChatMessage_Serialize(&buf, msg);
    SendBanchoPacket(bconn->bstate, CHOPKT_SERVER_SEND_MESSAGE, &buf);
    BufferFree(&buf);
}

void EnqueueMessage(ChatMessage *message) {
    xQueueSend(ChatQueue, &message, portMAX_DELAY);
}