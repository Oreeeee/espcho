#include <Arduino.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include "config.h"
#include "BanchoServer.h"
#include "Globals.h"
#include "ThreadingUtils.h"
#include "HttpServer.h"
#include <lwip/sockets.h>

#ifdef CHO_DISABLE_BROWNOUT
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif
#include <WebServer.h>

#include "Pinger.h"
#include "chat/ChatManager.h"

//WiFiServer server(CHO_PORT);
int sockfd = 0;

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

  Serial.println("Creating chat thread");
  ChatInit();

  // Create pinger task
  TaskHandle_t pingerTask;
  xTaskCreate(
      PingClient,
      "Pinger",
      2048,
      NULL,
      1,
      &pingerTask
  );

  Serial.printf("Starting TCP server on port %d\n", CHO_PORT);

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(CHO_PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    Serial.println("Failed to create socket");
    return;
  }

  if ((bind(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0) {
    Serial.println("Failed to bind");
    return;
  }

  if (listen(sockfd, 10) < 0) {
    Serial.println("Failed to listen");
    return;
  }

  Serial.println("Listening...");

  httpServerC = initHttpServer_c();
}

void loop() {
  struct sockaddr_in client;
  socklen_t clientAddressLen = 0;
  int clientSock;
  if ((clientSock = accept(sockfd, (struct sockaddr *)&client, &clientAddressLen)) < 0) {
    Serial.println("Failed to accept connection");
    return;
  }

  Serial.printf("Acepted connection from %s:%d\n", inet_ntoa(client.sin_addr), client.sin_port);
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
  // close(clientSock);
  // if (client) {
  //   Serial.printf("Accepted connection from %s:%d\n", client.remoteIP().toString(), client.remotePort());
  //   Serial.println("Trying to start Bancho task");
  //
  //   int freeConnIndex = getFreeConnectionIndex();
  //
  //   if (freeConnIndex == -1) {
  //     Serial.println("No free connection slots left, dropping connection");
  //     client.stop();
  //     return;
  //   }
  //
  //   BanchoConnection bconn;
  //   bconn.client = client;
  //   bconn.index = freeConnIndex;
  //   bconn.active = true;
  //
  //   connections[freeConnIndex] = bconn;
  //
  //   Serial.println("Starting Bancho task");
  //   xTaskCreate(
  //     banchoTask,
  //     "Bancho",
  //     4096,
  //     &connections[freeConnIndex],
  //     1,
  //     &bconn.task
  //   );
  //   Serial.println("Started Bancho task");
  // }
  //httpServer.handleClient();
}
