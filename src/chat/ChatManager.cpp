#include "chat/ChatManager.h"
#include <freertos/FreeRTOS.h>
#include <Arduino.h>

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
        ChatMessage_Free(msg);
    }
}

void EnqueueMessage(ChatMessage *message) {
    xQueueSend(ChatQueue, &message, portMAX_DELAY);
}