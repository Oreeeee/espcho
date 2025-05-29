#ifndef BANCHOSTATE_H
#define BANCHOSTATE_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

typedef struct {
    SemaphoreHandle_t writeLock;
    //bool writeLock;
    bool alive;
    int clientSock;
} BanchoState;

#endif
