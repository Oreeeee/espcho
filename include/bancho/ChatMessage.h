#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "serialization/Buffer.h"

typedef struct {
    char* sender;
    char* message;
    char* target;
} ChatMessage;

void ChatMessage_Deserialize(Buffer* buf, ChatMessage* p);
void ChatMessage_Free(ChatMessage* p);

#endif //CHATMESSAGE_H
