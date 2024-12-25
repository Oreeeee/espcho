#include "HttpServer.h"
#include "datatypes/ScoreSubData.h"
#include <esp_http_server.h>
#include <Arduino.h>

httpd_uri_t uri_score_sub = {
    .uri = "/web/osu-submit-modular.php",
    .method = HTTP_POST,
    .handler = scoreSubHandler,
    .user_ctx = NULL
};

esp_err_t scoreSubHandler(httpd_req_t *req) {
    Serial.println("[HTTP] POST /web/osu-submit-modular.php");

    char query[1024];
    char value[512];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        Serial.println("[HTTP] Query string empty");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    if (httpd_query_key_value(query, "pass", value, sizeof(value)) == ESP_OK) {
        Serial.printf("[HTTP] Got pass: %s\n", value);
    } else {
        Serial.println("[HTTP] No pass data!");
        httpd_resp_send(req, "error: pass", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    strcpy(value, ""); // Ensure that query buffer is empty

    if (httpd_query_key_value(query, "score", value, sizeof(value)) == ESP_OK) {
        Serial.printf("[HTTP] Got score data: %s\n", value);

        Serial.println("Parsing score...");
        ScoreSubData s = parseScoreString(value);
        Serial.printf("Score submitted by %s\n", s.username);
    } else {
        Serial.println("[HTTP] No score data!?!?!??!??!!!");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }


    const char resp[] = "error: ban";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t initHttpServer_c() {
    Serial.println("Starting HTTP server on port 80...");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpServerC = NULL;

    if (httpd_start(&httpServerC, &config) == ESP_OK) {
        httpd_register_uri_handler(httpServerC, &uri_score_sub);
    }

    Serial.println("HTTP Server started on port 80");
    return httpServerC;
}

void stopHttpServer(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}
