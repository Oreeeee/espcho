#include "BanchoState.h"
#include "BanchoServer.h"
#include "bancho/BanchoPackets.h"
#include "config.h"
#include <Arduino.h>

#include "Globals.h"

void PingClient(void *arg) {
    BanchoConnection *bconn;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Wait for every 10 seconds to ping

        // Ping every alive client
        for (int i = 0; i < CHO_MAX_CONNECTIONS; i++) {
            bconn = &connections[i];
            if (bconn->active) {
#ifdef CHO_LOG_PINGER
                Serial.printf("[PINGER] Pinging client %d\n", i);
#endif
                SendBanchoPacket(bconn->bstate, CHOPKT_PING, NULL);
            }
        }
    }
}
