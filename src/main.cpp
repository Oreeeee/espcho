#include <Arduino.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include "config.h"

WiFiServer server(CHO_PORT);

void setup() {
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
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.printf("Accepted connection from %s:%d\n", client.remoteIP().toString(), client.remotePort());

    while (client.connected()) {
      if (client.available()) {
        char *req = (char*)calloc(64, sizeof(char));
        client.readBytes(req, 64);

        Serial.printf("Received from client: %s\n", req);
        free(req);
      }
    }

    client.stop();
    Serial.println("Dropping connection!");
  }
}
