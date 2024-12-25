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
#include "lwip/sockets.h"

#ifdef CHO_DISABLE_BROWNOUT
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif
#include <WebServer.h>

WiFiServer server(CHO_PORT);
int listenSock;


void setup() {
  #ifdef CHO_DISABLE_BROWNOUT
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  #endif

  Serial.begin(115200);
  Serial.println("espcho says hello!");

  Serial.print("Connecting to WiFi ");
  WiFi.begin(CHO_WIFI_SSID, CHO_WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(50);
  }
  Serial.println();

  Serial.printf("Connected to WiFI! Got IPv4: %s\n", WiFi.localIP().toString());

  Serial.println("Creating socket");
  int listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (listenSock < 0) {
    Serial.println("Failed to create socket");
    return;
  }
  Serial.println("Socket created");

  Serial.println("Binding socket");

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htonl(CHO_PORT);

  if (bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    Serial.println("Failed to bind socket");
    close(listenSock);
    return;
  }
  Serial.printf("Socket bound to port %d\n", CHO_PORT);

  if (listen(listenSock, 1) < 0) {
    Serial.println("Failed to listen");
    close(listenSock);
    return;
  }
  Serial.println("Listening to connections");

  httpServerC = initHttpServer_c();
}

void loop() {
  // Accept incoming TCP connections
  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);
  int clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &clientLen);
  if (clientSock < 0) {
    //Serial.println("Failed to accept incoming connection");
    return;
  }
  Serial.println("Client connected");

  Serial.println("Trying to start Bancho task");

  int freeConnIndex = getFreeConnectionIndex();

  if (freeConnIndex == -1) {
    Serial.println("No free connection slots left, dropping connection");
    close(clientSock);
    return;
  }

  BanchoConnection bconn;
  bconn.clientSock = clientSock;
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

  /*
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
  */
  //httpServer.handleClient();
}
