#include "../../include/http/osuDirect.h"

#include <esp_http_client.h>
#include <HardwareSerial.h>

// Handles the searching of beatmaps inside of osu!Direct menu
esp_err_t directSearchHandler(httpd_req_t *req) {
    Serial.println("[HTTP] GET /web/osu-search.php");

    // The getting the request from the client part

    char query[512];
    char searchQuery[128];
    char displayMode[2];

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) != ESP_OK) {
        Serial.println("[HTTP] Query string empty");
        httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    if (httpd_query_key_value(query, "q", searchQuery, sizeof(searchQuery)) == ESP_OK) {
        Serial.printf("[HTTP] Got search query: %s\n", searchQuery);
    }

    if (httpd_query_key_value(query, "r", displayMode, sizeof(displayMode)) == ESP_OK) {
        Serial.printf("[HTTP] Got display mode: %s\n", displayMode);
    }

    // The proxying the request to catboy.best part

    char targetRequest[256];
    snprintf(targetRequest, sizeof(targetRequest), "https://catboy.best/api/search/raw?r=%s&q=%s", displayMode, searchQuery);

    Serial.printf("[HTTP] Sending request to %s\n", targetRequest);

    esp_http_client_config_t config = {};
    config.method = HTTP_METHOD_GET;
    config.url = targetRequest;
    config.cert_pem = mirrorCaPem;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t openErr = esp_http_client_open(client, 0);
    if (openErr != ESP_OK) {
        Serial.printf("[HTTP] Failed to send request: %s\n", esp_err_to_name(openErr));
        esp_http_client_cleanup(client);
        httpd_resp_send_500(req);
        return openErr;
    }

    int wlen = esp_http_client_write(client, NULL, 0);
    Serial.printf("[HTTP] Wrote %d bytes in request\n", wlen);

    int contentLength = esp_http_client_fetch_headers(client);
    int statusCode = esp_http_client_get_status_code(client);
    Serial.printf("[HTTP] Got response with code %d and Content Length of %d\n", statusCode, contentLength);
    if (statusCode != 200) {
        Serial.println("[HTTP] Status code is not 200");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_hdr(req, "Transfer-Encoding", "chunked");

    char buf[128];
    int readLen;

    Serial.println("[HTTP] Streaming the response to the client");

    // Stream back the response back to the client
    while ((readLen = esp_http_client_read(client, buf, sizeof(buf))) > 0) {
        esp_err_t err = httpd_resp_send_chunk(req, buf, readLen);
        if (err != ESP_OK) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            Serial.println("[HTTP] Failed to send chunk");
            return err;
        }
    }

    httpd_resp_send_chunk(req, NULL, 0); // final chunk

    Serial.println("[HTTP] Response streamed back");

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    Serial.println("[HTTP] Finished sending osu!direct search data");
    return ESP_OK;
}

// Handles beatmap download requests
esp_err_t directDownloadHandler(httpd_req_t *req) {
    Serial.printf("[HTTP] GET %s\n", req->uri);

    const char *filename = req->uri + 3; // offset by length of /b/
    char *end;
    const int beatmapId = strtol(filename, &end, 10);
    if (beatmapId <= 0) {
        Serial.printf("[HTTP] Invalid beatmap ID (%d)\n", beatmapId);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    Serial.printf("[HTTP] Downloading beatmap %d to client\n", beatmapId);

    // Proxy the beatmap download to catboy.best
    char targetRequest[64];
    snprintf(targetRequest, sizeof(targetRequest), "https://catboy.best/d/%d", beatmapId);
    Serial.printf("[HTTP] Sending request to %s\n", targetRequest);

    esp_http_client_config_t config = {};
    config.method = HTTP_METHOD_GET;
    config.url = targetRequest;
    config.cert_pem = mirrorCaPem;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t openErr = esp_http_client_open(client, 0);
    if (openErr != ESP_OK) {
        Serial.printf("[HTTP] Failed to send request: %s\n", esp_err_to_name(openErr));
        esp_http_client_cleanup(client);
        httpd_resp_send_500(req);
        return openErr;
    }

    int wlen = esp_http_client_write(client, NULL, 0);
    Serial.printf("[HTTP] Wrote %d bytes in request\n", wlen);

    int contentLength = esp_http_client_fetch_headers(client);
    int statusCode = esp_http_client_get_status_code(client);
    Serial.printf("[HTTP] Got response with code %d and Content Length of %d\n", statusCode, contentLength);
    if (statusCode != 200) {
        Serial.println("[HTTP] Status code is not 200");
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
        Serial.printf("[HTTP] Couldn't allocate osu!Direct map chunk of size %d\n", OSU_DIRECT_MAP_CHUNK_SIZE);
        httpd_resp_send_500(req);
    }
    int readLen;

    Serial.println("[HTTP] Streaming the response to the client");

    // Stream back the response back to the client
    while ((readLen = esp_http_client_read(client, buf, OSU_DIRECT_MAP_CHUNK_SIZE)) > 0) {
        //Serial.printf("[HTTP] Sending chunk with size of %d\n", readLen);
        esp_err_t err = httpd_resp_send_chunk(req, buf, readLen);
        if (err != ESP_OK) {
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            Serial.println("[HTTP] Failed to send chunk");
            return err;
        }
    }

    httpd_resp_send_chunk(req, NULL, 0); // final chunk
    free(buf);
    buf = NULL;

    Serial.println("[HTTP] Response streamed back");

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    Serial.println("[HTTP] Finished sending beatmap data");

    return ESP_OK;
}
