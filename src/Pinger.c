#include "BanchoState.h"
#include "BanchoServer.h"
#include "bancho/BanchoPackets.h"
#include "config.h"

#ifdef CHO_LOG_PINGER
#define LOG_LOCAL_LEVEL LOG_LEVEL_DEBUG
#include "esp_log.h"
static const char *TAG = "Pinger";
#endif

#include "Globals.h"

void PingClient(void *arg) {
    BanchoConnection *bconn;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Wait for every 10 seconds to ping

        // Ping every alive client
        xSemaphoreTake(connMutex, portMAX_DELAY);
        for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
            bconn = &connections[i];
            if (bconn->active) {
#ifdef CHO_LOG_PINGER
                ESP_LOGD(TAG, "[PINGER] Pinging client %d\n", i);
#endif
                SendBanchoPacket(bconn->bstate, CHOPKT_PING, NULL);
            }
        }
        xSemaphoreGive(connMutex);
    }
}
