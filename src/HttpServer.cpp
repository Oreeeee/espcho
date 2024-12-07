#include "HttpServer.h"
#include <WebServer.h>

WebServer httpServer(80);

void handleScoreSub() {
    httpServer.send(200, "text/plain", "error: ban");
}

void handle404() {
    httpServer.send(404, "text/plain", "Not found");
}

void initHttpServer() {
    Serial.println("Starting HTTP server on port 80...");
    httpServer.on("/web/osu-submit-modular.php", handleScoreSub);
    httpServer.onNotFound(handle404);
    httpServer.begin();
    Serial.println("HTTP Server started on port 80");
}