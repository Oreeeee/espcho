#include "HttpServer.h"
#include "datatypes/ScoreSubData.h"
#include <esp_http_server.h>
#include <HardwareSerial.h>
#include "esp_tls.h"
#include <esp_http_client.h>

// Certificate for catboy.best, as of 2025-08-05
// XXX: If this has been changed then request will fail.
// You might need to get the newer certificate chain
// Linux command to get new cert:
// openssl s_client -showcerts -connect catboy.best:443 -servername catboy.best < /dev/null 2>/dev/null | sed -n '/-----BEGIN CERTIFICATE-----/,/-----END CERTIFICATE-----/p' | tail -n +1 > root_ca.pem
const char* mirrorCaPem =
"-----BEGIN CERTIFICATE-----\n"
"MIIDuTCCA0CgAwIBAgISBu5vthxDv1RnaP67/G3ZShSBMAoGCCqGSM49BAMDMDIx\n"
"CzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQswCQYDVQQDEwJF\n"
"NTAeFw0yNTA3MDUxOTIyNDFaFw0yNTEwMDMxOTIyNDBaMBYxFDASBgNVBAMTC2Nh\n"
"dGJveS5iZXN0MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEVumVJ7umE9uKkmAk\n"
"oM1HeQSDmtQsMW962GwuNRu18qpMBxxd356Qchea4RKbjIGu5JZRPWzVo75RSwsc\n"
"zbytK6OCAlAwggJMMA4GA1UdDwEB/wQEAwIHgDAdBgNVHSUEFjAUBggrBgEFBQcD\n"
"AQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQU8mAZr4SRMFFONxNV\n"
"oJ6DkyVwvvQwHwYDVR0jBBgwFoAUnytfzzwhT50Et+0rLMTGcIvS1w0wMgYIKwYB\n"
"BQUHAQEEJjAkMCIGCCsGAQUFBzAChhZodHRwOi8vZTUuaS5sZW5jci5vcmcvME0G\n"
"A1UdEQRGMESCEyouYmVhdG1hcHMuZG93bmxvYWSCDSouY2F0Ym95LmJlc3SCEWJl\n"
"YXRtYXBzLmRvd25sb2FkggtjYXRib3kuYmVzdDATBgNVHSAEDDAKMAgGBmeBDAEC\n"
"ATAsBgNVHR8EJTAjMCGgH6AdhhtodHRwOi8vZTUuYy5sZW5jci5vcmcvMi5jcmww\n"
"ggEFBgorBgEEAdZ5AgQCBIH2BIHzAPEAdwB9WR4S4XgqexxhZ3xe/fjQh1wUoE6V\n"
"nrkDL9kOjC55uAAAAZfcP/94AAAEAwBIMEYCIQCjTWDhdE1zloZdxoKxYD8yTY/v\n"
"LmBYsvL0MGjFYc1QYAIhAIXqs3GT2uUWjH0nFJcE7zYG21c14e8FBlOebjtUChSQ\n"
"AHYA3dzKNJXX4RYF55Uy+sef+D0cUN/bADoUEnYKLKy7yCoAAAGX3EAHbwAABAMA\n"
"RzBFAiB+y5SSWcVz0NYc1v0z6QU1N5mocmWnoEghgUkNT7/G1AIhAKWUXFCXuw4Y\n"
"639050UuLs5ofRDdGIKMUESDTwlZVRtXMAoGCCqGSM49BAMDA2cAMGQCMGxv3/uJ\n"
"QefGNRJ39/4hVppMToWGBsl/ZpATgPte+KMFpkwIOvSptJrQAnlSCL7TVgIwSEBB\n"
"on9z/DUR0gmN/PF5jE+olRr8LMCd86QLrJS1rjBz9U8CRzgk6Pft59NTA/5Y\n"
"-----END CERTIFICATE-----\n"
"-----BEGIN CERTIFICATE-----\n"
"MIIEVzCCAj+gAwIBAgIRAIOPbGPOsTmMYgZigxXJ/d4wDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\n"
"WhcNMjcwMzEyMjM1OTU5WjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
"RW5jcnlwdDELMAkGA1UEAxMCRTUwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQNCzqK\n"
"a2GOtu/cX1jnxkJFVKtj9mZhSAouWXW0gQI3ULc/FnncmOyhKJdyIBwsz9V8UiBO\n"
"VHhbhBRrwJCuhezAUUE8Wod/Bk3U/mDR+mwt4X2VEIiiCFQPmRpM5uoKrNijgfgw\n"
"gfUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcD\n"
"ATASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSfK1/PPCFPnQS37SssxMZw\n"
"i9LXDTAfBgNVHSMEGDAWgBR5tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcB\n"
"AQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0g\n"
"BAwwCjAIBgZngQwBAgEwJwYDVR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVu\n"
"Y3Iub3JnLzANBgkqhkiG9w0BAQsFAAOCAgEAH3KdNEVCQdqk0LKyuNImTKdRJY1C\n"
"2uw2SJajuhqkyGPY8C+zzsufZ+mgnhnq1A2KVQOSykOEnUbx1cy637rBAihx97r+\n"
"bcwbZM6sTDIaEriR/PLk6LKs9Be0uoVxgOKDcpG9svD33J+G9Lcfv1K9luDmSTgG\n"
"6XNFIN5vfI5gs/lMPyojEMdIzK9blcl2/1vKxO8WGCcjvsQ1nJ/Pwt8LQZBfOFyV\n"
"XP8ubAp/au3dc4EKWG9MO5zcx1qT9+NXRGdVWxGvmBFRAajciMfXME1ZuGmk3/GO\n"
"koAM7ZkjZmleyokP1LGzmfJcUd9s7eeu1/9/eg5XlXd/55GtYjAM+C4DG5i7eaNq\n"
"cm2F+yxYIPt6cbbtYVNJCGfHWqHEQ4FYStUyFnv8sjyqU8ypgZaNJ9aVcWSICLOI\n"
"E1/Qv/7oKsnZCWJ926wU6RqG1OYPGOi1zuABhLw61cuPVDT28nQS/e6z95cJXq0e\n"
"K1BcaJ6fJZsmbjRgD5p3mvEf5vdQM7MCEvU0tHbsx2I5mHHJoABHb8KVBgWp/lcX\n"
"GWiWaeOyB7RP+OfDtvi2OsapxXiV7vNVs7fMlrRjY1joKaqmmycnBvAq14AEbtyL\n"
"sVfOS66B8apkeFX2NY4XPEYV4ZSCe8VHPrdrERk2wILG3T/EGmSIkCYVUMSnjmJd\n"
"VQD9F6Na/+zmXCc=\n"
"-----END CERTIFICATE-----\n";

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
        Serial.println("[HTTP] No pass data");
        httpd_resp_send(req, "error: pass", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
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
        return ESP_OK;
    }

    httpd_resp_send(req, "", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

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

httpd_handle_t initHttpServer_c() {
    Serial.println("Starting HTTP server on port 80...");

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 8196; // TODO: should this be lowered?
    httpServerC = NULL;

    if (httpd_start(&httpServerC, &config) == ESP_OK) {
        httpd_register_uri_handler(httpServerC, &uri_score_sub);
        httpd_register_uri_handler(httpServerC, &uri_direct_search);
    }

    Serial.println("HTTP Server started on port 80");
    return httpServerC;
}

void stopHttpServer(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
    }
}
