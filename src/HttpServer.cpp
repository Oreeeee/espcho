#include "HttpServer.h"
#include <WebServer.h>
#include "datatypes/ScoreSubData.h"

WebServer httpServer(80);

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

void initHttpServer() {
    Serial.println("Starting HTTP server on port 80...");
    httpServer.on("/web/osu-submit-modular.php", handleScoreSub);
    httpServer.onNotFound(handle404);
    httpServer.begin();
    Serial.println("HTTP Server started on port 80");
}