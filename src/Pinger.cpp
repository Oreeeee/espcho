#include "BanchoState.h"
#include "BanchoServer.h"
#include "bancho/BanchoPackets.h"
#include <Arduino.h>

void PingClient(void *arg) {
    BanchoState *bstate = (BanchoState*)arg;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Wait for every 10 seconds to ping
        if (!bstate->alive) {
            // TODO: Pinger doesn't always end
            Serial.println("[PINGER] Quitting");
            vTaskDelete(NULL);
        }
        Serial.println("[PINGER] Pinging");
        sendEmptyPacket(bstate, CHO_PACKET_PING);
    }
}
