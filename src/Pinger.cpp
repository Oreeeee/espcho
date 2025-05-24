#include "BanchoState.h"
#include "BanchoServer.h"
#include "bancho/BanchoPackets.h"
#include "config.h"
#include <Arduino.h>

void PingClient(void *arg) {
    BanchoState *bstate = (BanchoState*)arg;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Wait for every 10 seconds to ping
        if (!bstate->alive) {
            // TODO: Pinger doesn't always end
            #ifdef CHO_LOG_PINGER
            Serial.println("[PINGER] Quitting");
            #endif

            vTaskDelete(NULL);
        }
        #ifdef CHO_LOG_PINGER
        Serial.println("[PINGER] Pinging");
        #endif
        
        SendBanchoPacket(bstate, CHOPKT_PING, NULL);
    }
}
