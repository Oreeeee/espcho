#ifndef BANCHOSTATE_H
#define BANCHOSTATE_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

typedef struct {
    SemaphoreHandle_t writeLock;
    int clientSock;
    bool alive;
} BanchoState;

#endif
