#include "../../include/http/osuDirect.h"
#include <esp_http_client.h>

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include <esp_log.h>
static const char *TAG = "osuDirect";

// Handles the searching of beatmaps inside of osu!Direct menu
esp_err_t directSearchHandler(httpd_req_t *req) {
    ESP_LOGI(TAG, "GET /web/osu-search.php");

    // The getting the request from the client part

    char query[512];
    char searchQuery[128];
    char displayMode[2];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        ESP_LOGW(TAG, "Query string empty");
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    if (httpd_query_key_value(query, "q", searchQuery, sizeof(searchQuery)) == ESP_OK) {
        ESP_LOGD(TAG, "Got search query: %s", searchQuery);
    }

    if (httpd_query_key_value(query, "r", displayMode, sizeof(displayMode)) == ESP_OK) {
        ESP_LOGD(TAG, "Got display mode: %s", displayMode);
    }

    // The proxying the request to catboy.best part

    char targetRequest[256];
    snprintf(targetRequest, sizeof(targetRequest), "https://catboy.best/api/search/raw?r=%s&q=%s", displayMode, searchQuery);

    ESP_LOGI(TAG, "Sending request to %s", targetRequest);

    esp_http_client_config_t config = {};
    config.method = HTTP_METHOD_GET;
    config.url = targetRequest;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t openErr = esp_http_client_open(client, 0);
    if (openErr != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send request: %s", esp_err_to_name(openErr));
        esp_http_client_cleanup(client);
        httpd_resp_send_500(req);
        return openErr;
    }

    int wlen = esp_http_client_write(client, NULL, 0);
    ESP_LOGD(TAG, "Wrote %d bytes in request", wlen);

    int contentLength = esp_http_client_fetch_headers(client);
    int statusCode = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "Got response with code %d and Content Length of %d", statusCode, contentLength);
    if (statusCode != 200) {
        ESP_LOGE(TAG, "Status code is not 200");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_hdr(req, "Transfer-Encoding", "chunked");

    char buf[128];
    int readLen;

    ESP_LOGI(TAG, "Streaming the response to the client");

    // Stream back the response back to the client
    while ((readLen = esp_http_client_read(client, buf, sizeof(buf))) > 0) {
        esp_err_t err = httpd_resp_send_chunk(req, buf, readLen);
        if (err != ESP_OK) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            ESP_LOGE(TAG, "Failed to send chunk");
            return err;
        }
    }

    httpd_resp_send_chunk(req, NULL, 0); // final chunk

    ESP_LOGD(TAG, "Response streamed back");

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    ESP_LOGI(TAG, "Finished sending osu!direct search data");
    return ESP_OK;
}

// Handles beatmap download requests
esp_err_t directDownloadHandler(httpd_req_t *req) {
    ESP_LOGI(TAG, "GET %s", req->uri);

    const char *filename = req->uri + 3; // offset by length of /b/
    char *end;
    const int beatmapId = strtol(filename, &end, 10);
    if (beatmapId <= 0) {
        ESP_LOGW(TAG, "Invalid beatmap ID (%d)", beatmapId);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Downloading beatmap %d to client", beatmapId);

    // Proxy the beatmap download to catboy.best
    char targetRequest[64];
    snprintf(targetRequest, sizeof(targetRequest), "https://catboy.best/d/%d", beatmapId);
    ESP_LOGD(TAG, "Sending request to %s", targetRequest);

    esp_http_client_config_t config = {};
    config.method = HTTP_METHOD_GET;
    config.url = targetRequest;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t openErr = esp_http_client_open(client, 0);
    if (openErr != ESP_OK) {
        ESP_LOGE(TAG, "[HTTP] Failed to send request: %s", esp_err_to_name(openErr));
        esp_http_client_cleanup(client);
        httpd_resp_send_500(req);
        return openErr;
    }

    int wlen = esp_http_client_write(client, NULL, 0);
    ESP_LOGD(TAG, "Wrote %d bytes in request", wlen);

    int contentLength = esp_http_client_fetch_headers(client);
    int statusCode = esp_http_client_get_status_code(client);
    ESP_LOGD(TAG, "Got response with code %d and Content Length of %d", statusCode, contentLength);
    if (statusCode != 200) {
        ESP_LOGE(TAG, "Status code is not 200");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    char contentLengthStr[16];
    snprintf(contentLengthStr, sizeof(contentLengthStr), "%d", contentLength);
    httpd_resp_set_hdr(req, "Transfer-Encoding", "chunked");
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_set_hdr(req, "Content-Length", contentLengthStr);

    char *buf = (char*)calloc(OSU_DIRECT_MAP_CHUNK_SIZE, sizeof(char));
    if (buf == NULL) {
        ESP_LOGE(TAG, "Couldn't allocate osu!Direct map chunk of size %d", OSU_DIRECT_MAP_CHUNK_SIZE);
        httpd_resp_send_500(req);
    }
    int readLen;

    ESP_LOGI(TAG, "Streaming the response to the client");

    // Stream back the response back to the client
    while ((readLen = esp_http_client_read(client, buf, OSU_DIRECT_MAP_CHUNK_SIZE)) > 0) {
        //Serial.printf("[HTTP] Sending chunk with size of %d\n", readLen);
        esp_err_t err = httpd_resp_send_chunk(req, buf, readLen);
        if (err != ESP_OK) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            ESP_LOGE(TAG, "Failed to send chunk");
            return err;
        }
    }

    httpd_resp_send_chunk(req, NULL, 0); // final chunk
    free(buf);
    buf = NULL;

    ESP_LOGD(TAG, "Response streamed back");

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    ESP_LOGI(TAG, "Finished sending beatmap data");

    return ESP_OK;
}
