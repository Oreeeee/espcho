#include "BanchoConnection.h"
#include "config.h"

BanchoConnection connections[CHO_MAX_CONNECTIONS];
SemaphoreHandle_t connMutex;
