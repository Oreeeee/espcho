#include "../../include/http/scoring.h"

#include <HardwareSerial.h>
#include "datatypes/ScoreSubData.h"

esp_err_t scoreSubHandler(httpd_req_t *req) {
    Serial.println("[HTTP] POST /web/osu-submit-modular.php");

    char query[1024];
    char value[512];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        Serial.println("[HTTP] Query string empty");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    if (httpd_query_key_value(query, "pass", value, sizeof(value)) == ESP_OK) {
        Serial.printf("[HTTP] Got pass: %s\n", value);
    } else {
        Serial.println("[HTTP] No pass data");
        httpd_resp_send(req, "error: pass", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    strcpy(value, ""); // Ensure that query buffer is empty

    if (httpd_query_key_value(query, "score", value, sizeof(value)) == ESP_OK) {
        Serial.printf("[HTTP] Got score data: %s\n", value);

        Serial.println("Parsing score...");
        ScoreSubData s = ParseScoreString(value);
        Serial.printf("Score submitted by %s\n", s.username);
        FreeScoreSubData(&s);
    } else {
        Serial.println("[HTTP] No score data");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
