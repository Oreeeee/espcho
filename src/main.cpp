#include <Arduino.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include "config.h"
#include "constants.h"
#include "BanchoServer.h"

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

    LoginPacket lp = getConnectionInfo(client);
    Serial.printf("Username: %s\nPassword: %s\nClient info: %s\n", lp.username, lp.password, lp.clientInfo);

    Serial.println("Verifying login");
    if (!authenticateChoUser(client, lp.username, lp.password)) {
      Serial.println("Authentication failed! Server dropping conenction");
      client.stop();
    }
    Serial.println("Authentication successful!");

    while (client.connected()) {
      if (client.available()) {
        char *buf;
        BanchoHeader h;
        h = readBanchoPacket(client, buf);

        switch (h.packetId) {
          default:
            Serial.printf("Unknown packet received: %d\n", h.packetId);
        }

        //if (buf != NULL)
        free(buf);
      }
    }

    client.stop();
    Serial.println("Dropping connection!");
  }
}
