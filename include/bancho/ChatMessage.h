#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "serialization/Buffer.h"
#include <stdbool.h>

typedef struct {
    char* sender;
    char* message;
    char* target;

    // Not a part of the protocol things
    uint32_t senderId;
    bool privateMessage;
} ChatMessage;

void ChatMessage_Serialize(Buffer* buf, ChatMessage* p);
void ChatMessage_Deserialize(Buffer* buf, ChatMessage* p);
void ChatMessage_Free(ChatMessage* p);

#endif //CHATMESSAGE_H
