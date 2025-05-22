#ifndef CHATMANAGER_H
#define CHATMANAGER_H
#include "bancho/ChatMessage.h"

void ChatInit();
void ChatLoop(void *args);
void EnqueueMessage(ChatMessage *message);

#endif //CHATMANAGER_H
