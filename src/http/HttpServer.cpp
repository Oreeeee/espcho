#include "http/HttpServer.h"
#include "datatypes/ScoreSubData.h"
#include <esp_http_server.h>
#include <HardwareSerial.h>
#include "esp_tls.h"
#include <esp_http_client.h>

httpd_handle_t initHttpServer_c() {
    Serial.println("Starting HTTP server on port 80...");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8096; // TODO: should this be lowered?
    config.uri_match_fn = httpd_uri_match_wildcard;
    httpServerC = NULL;

    if (httpd_start(&httpServerC, &config) == ESP_OK) {
        Serial.printf("[HTTP] Registering handler status: %d\n", httpd_register_uri_handler(httpServerC, &uri_score_sub));
        Serial.printf("[HTTP] Registering handler status: %d\n", httpd_register_uri_handler(httpServerC, &uri_direct_search));
        Serial.printf("[HTTP] Registering handler status: %d\n", httpd_register_uri_handler(httpServerC, &uri_direct_download));
    }

    Serial.println("HTTP Server started on port 80");
    return httpServerC;
}

void stopHttpServer(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}
