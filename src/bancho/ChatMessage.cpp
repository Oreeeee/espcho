#include "bancho/ChatMessage.h"

#include <cstdlib>
#include <HardwareSerial.h>

#include "serialization/Buffer.h"
#include "serialization/Readers.h"


void ChatMessage_Deserialize(Buffer* buf, ChatMessage* p) {
    BufferReadOsuString(buf, &p->sender);
    BufferReadOsuString(buf, &p->message);
    BufferReadOsuString(buf, &p->target);
}

void ChatMessage_Free(ChatMessage* p) {
    free(p->sender);
    free(p->message);
    free(p->target);
}
