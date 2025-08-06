#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <esp_http_server.h>
#include "osuDirect.h"
#include "scoring.h"

static httpd_handle_t httpServerC;

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

httpd_handle_t initHttpServer_c();
void stopHttpServer(httpd_handle_t server);

#endif
