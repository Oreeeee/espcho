#include "http/HttpServer.h"
#include "datatypes/ScoreSubData.h"
#include <esp_http_server.h>
#include "esp_tls.h"
#include <esp_http_client.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>

#include "http/osuDirect.h"
#include "http/scoring.h"
static const char *TAG = "HttpServer";

httpd_uri_t uri_score_sub = {
    .uri = "/web/osu-submit-modular.php",
    .method = HTTP_POST,
    .handler = scoreSubHandler,
    .user_ctx = NULL
};

httpd_uri_t uri_direct_search = {
    .uri = "/web/osu-search.php",
    .method = HTTP_GET,
    .handler = directSearchHandler,
    .user_ctx = NULL
};

httpd_uri_t uri_direct_download = {
    .uri = "/d/*",
    .method = HTTP_GET,
    .handler = directDownloadHandler,
    .user_ctx = NULL
};

httpd_handle_t initHttpServer_c() {
    ESP_LOGI(TAG, "Starting HTTP server on port 80...");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8096; // TODO: should this be lowered?
    config.uri_match_fn = httpd_uri_match_wildcard;
    httpServerC = NULL;

    if (httpd_start(&httpServerC, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering handler status: %d", httpd_register_uri_handler(httpServerC, &uri_score_sub));
        ESP_LOGI(TAG, "Registering handler status: %d", httpd_register_uri_handler(httpServerC, &uri_direct_search));
        ESP_LOGI(TAG, "Registering handler status: %d", httpd_register_uri_handler(httpServerC, &uri_direct_download));
    }

    ESP_LOGI(TAG, "HTTP Server started on port 80");
    return httpServerC;
}

void stopHttpServer(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}
