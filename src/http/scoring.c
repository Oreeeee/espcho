#include "../../include/http/scoring.h"
#include "datatypes/ScoreSubData.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
static const char *TAG = "Scoring";

esp_err_t scoreSubHandler(httpd_req_t *req) {
    ESP_LOGI(TAG, "POST /web/osu-submit-modular.php");

    char query[1024];
    char value[512];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        ESP_LOGW(TAG, "Query string empty");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    if (httpd_query_key_value(query, "pass", value, sizeof(value)) == ESP_OK) {
        ESP_LOGD(TAG, "Got pass: %s", value);
    } else {
        ESP_LOGW(TAG, "No pass data");
        httpd_resp_send(req, "error: pass", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    strcpy(value, ""); // Ensure that query buffer is empty

    if (httpd_query_key_value(query, "score", value, sizeof(value)) == ESP_OK) {
        ESP_LOGI(TAG, "Got score data: %s", value);

        ESP_LOGD(TAG, "Parsing score...");
        ScoreSubData s = ParseScoreString(value);
        ESP_LOGI(TAG, "Score submitted by %s", s.username);
        FreeScoreSubData(&s);
    } else {
        ESP_LOGW(TAG, "No score data");
        httpd_resp_send(req, "error: ban", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
