#ifndef CHATMANAGER_H
#define CHATMANAGER_H
#include "BanchoConnection.h"
#include "bancho/ChatMessage.h"

void ChatInit();
void ChatLoop(void *args);
void EnqueueMessage(ChatMessage *message);
void SendMessage(ChatMessage *msg, BanchoConnection *bconn);

#endif //CHATMANAGER_H
