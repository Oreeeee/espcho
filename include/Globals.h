#ifndef GLOBALS_H
#define GLOBALS_H

#include "BanchoConnection.h"

extern BanchoConnection connections[CHO_MAX_CONNECTIONS];
extern SemaphoreHandle_t connMutex;

#endif
