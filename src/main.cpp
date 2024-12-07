#include <Arduino.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include "config.h"
#include "constants.h"
#include "BanchoServer.h"
#include "bancho/BanchoPackets.h"
#include "BanchoState.h"
#include "Pinger.h"
#include "Globals.h"
#include "ThreadingUtils.h"
#include "HttpServer.h"

#ifdef CHO_DISABLE_BROWNOUT
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif
#include <WebServer.h>

WiFiServer server(CHO_PORT);


void setup() {
  #ifdef CHO_DISABLE_BROWNOUT
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  #endif

  Serial.begin(9600);
  Serial.println("espcho says hello!");

  Serial.print("Connecting to WiFi ");
  WiFi.begin(CHO_WIFI_SSID, CHO_WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(50);
  }
  Serial.println();

  Serial.printf("Connected to WiFI! Got IPv4: %s\n", WiFi.localIP().toString());

  Serial.printf("Starting TCP server on port %d\n", CHO_PORT);
  server.begin();
  Serial.println("Listening...");

  initHttpServer();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.printf("Accepted connection from %s:%d\n", client.remoteIP().toString(), client.remotePort());
    Serial.println("Trying to start Bancho task");

    int freeConnIndex = getFreeConnectionIndex();

    if (freeConnIndex == -1) {
      Serial.println("No free connection slots left, dropping connection");
      client.stop();
      return;
    }

    BanchoConnection bconn;
    bconn.client = client;
    bconn.index = freeConnIndex;
    bconn.active = true;

    connections[freeConnIndex] = bconn;

    Serial.println("Starting Bancho task");
    xTaskCreate(
      banchoTask,
      "Bancho",
      4096,
      &connections[freeConnIndex],
      1,
      &bconn.task
    );
    Serial.println("Started Bancho task");
  }
  httpServer.handleClient();
}
