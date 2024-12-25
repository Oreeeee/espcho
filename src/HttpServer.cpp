#include "HttpServer.h"
//#include <WebServer.h>
#include "datatypes/ScoreSubData.h"
#include <esp_http_server.h>
#include <Arduino.h>

//WebServer httpServer(80);

httpd_uri_t uri_score_sub = {
    .uri = "/web/osu-submit-modular.php",
    .method = HTTP_POST,
    .handler = scoreSubHandler,
    .user_ctx = NULL
};

/*
void handleScoreSub() {
    char *scoreData = NULL;
    char *passData = NULL;

    char pedal1[2137];
    char pedal2[2137];

    delay(5000);
    Serial.println("[HTTP] Received score submission request");


    if (httpServer.hasArg("pass")) {
        //passData = (char*)calloc(httpServer.arg("pass").size() + 1, sizeof(char));
        String passDataTmp = httpServer.arg("pass");
        // passData = (char*)calloc(passDataTmp.length() + 1, sizeof(char));
        // passDataTmp.toCharArray(scoreData, passDataTmp.length() + 1);
        //Serial.printf("[HTTP] Got pass data: %s\n", passData);

        // String passDataTmp = httpServer.arg("pass");
        // passData = (char*)calloc(passDataTmp.length() + 1, sizeof(char));
        // strcpy(passData, passDataTmp.c_str());
        //Serial.printf("[HTTP] Got pass data: %s\n", passData);
        // strncpy(pedal2, httpServer.arg("pass").c_str(), sizeof(pedal2) - 1);
        // Serial.printf("[HTTP] Got pass data: %s\n", pedal2);
        passData = (char*)malloc(passDataTmp.length() + 1);
        strncpy(passData, passDataTmp.c_str(), passDataTmp.length() - 1);
        passData[passDataTmp.length()] = '\0';
        Serial.printf("[HTTP] Got pass data: %s\n", passData);
    } else {
        Serial.println("[HTTP] No pass data!");
        httpServer.send(200, "text/plain", "error: nouser");
        return;
    }

    if (httpServer.hasArg("score")) {
        String scoreDataTmp = httpServer.arg("score");
        // scoreData = (char*)calloc(scoreDataTmp.length() + 1, sizeof(char));
        // scoreDataTmp.toCharArray(scoreData, scoreDataTmp.length() + 1);
        //Serial.printf("[HTTP] Got score data: %s\n", scoreData);

        // String scoreDataTmp = httpServer.arg("pass");
        // scoreData = (char*)calloc(scoreDataTmp.length() + 1, sizeof(char));
        // strcpy(scoreData, scoreDataTmp.c_str());
        // strncpy(pedal1, httpServer.arg("score").c_str(), sizeof(pedal1) - 1);
        // Serial.printf("[HTTP] Got score data: %s\n", pedal1);

        scoreData = (char*)malloc(scoreDataTmp.length() + 1);
        strncpy(scoreData, scoreDataTmp.c_str(), scoreDataTmp.length() - 1);
        scoreData[scoreDataTmp.length()] = '\0';
        Serial.printf("[HTTP] Got pass data: %s\n", scoreData);
    } else {
        Serial.println("[HTTP] No score data!");
        httpServer.send(200, "text/plain", "error: ban");
        return;
    }

    Serial.println("Parsing score...");
    ScoreSubData s = parseScoreString(scoreData);
    Serial.flush();
    Serial.printf("Score submitted by %s\n", s.username);

    httpServer.send(200, "text/plain", "error: ban");
}

void handle404() {
    httpServer.send(404, "text/plain", "Not found");
}
*/

esp_err_t scoreSubHandler(httpd_req_t *req) {
    Serial.println("[HTTP] POST /web/osu-submit-modular.php");
    
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

/*
void initHttpServer() {
    Serial.println("Starting HTTP server on port 80...");
    httpServer.on("/web/osu-submit-modular.php", handleScoreSub);
    httpServer.onNotFound(handle404);
    httpServer.begin();
    Serial.println("HTTP Server started on port 80");
}
*/