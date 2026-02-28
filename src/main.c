#include "config.h"
#include "BanchoServer.h"
#include "Globals.h"
#include "ThreadingUtils.h"
#include "http/HttpServer.h"
#include <lwip/sockets.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "wifi.h"

#ifdef CHO_DISABLE_BROWNOUT
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char *TAG = "main";

#include "Pinger.h"
#include "chat/ChatManager.h"

//WiFiServer server(CHO_PORT);
int sockfd = 0;

void app_main(void) {
  #ifdef CHO_DISABLE_BROWNOUT
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  #endif

  // System initialization
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_LOGI(TAG, "espcho says hello!");

  ESP_LOGI(TAG, "Connecting to WiFi");
  wifi_init_sta();

  ESP_LOGI(TAG, "Creating chat thread");
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

  ESP_LOGI(TAG, "Starting TCP server on port %d", CHO_PORT);

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(CHO_PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    ESP_LOGE(TAG, "Failed to create socket");
    return;
  }

  if ((bind(sockfd, (struct sockaddr *)&server, sizeof(server))) < 0) {
    ESP_LOGE(TAG, "Failed to bind");
    return;
  }

  if (listen(sockfd, 10) < 0) {
    ESP_LOGE(TAG, "Failed to listen");
    return;
  }

  ESP_LOGI(TAG, "Listening...");

  httpServerC = initHttpServer_c();

  while (1) {
    struct sockaddr_in client;
    socklen_t clientAddressLen = 0;
    int clientSock;
    if ((clientSock = accept(sockfd, (struct sockaddr *)&client, &clientAddressLen)) < 0) {
      ESP_LOGE(TAG, "Failed to accept connection");
      continue;
    }

    ESP_LOGI(TAG, "Acepted connection from %s:%d", inet_ntoa(client.sin_addr), client.sin_port);
    ESP_LOGD(TAG, "Trying to start Bancho task");

    int freeConnIndex = getFreeConnectionIndex();

    if (freeConnIndex == -1) {
      ESP_LOGE(TAG, "No free connection slots left, dropping connection");
      close(clientSock);
      continue;
    }

    BanchoConnection bconn;
    bconn.clientSock = clientSock;
    bconn.index = freeConnIndex;
    bconn.active = true;

    connections[freeConnIndex] = bconn;

    ESP_LOGI(TAG, "Starting Bancho task");
    xTaskCreate(
      banchoTask,
      "Bancho",
      4096,
      &connections[freeConnIndex],
      1,
      &bconn.task
      );
    ESP_LOGD(TAG, "Started Bancho task");
  }
}
