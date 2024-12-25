#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <esp_http_server.h>

static httpd_handle_t httpServerC;

esp_err_t scoreSubHandler(httpd_req_t *req);
httpd_handle_t initHttpServer_c();
void stopHttpServer(httpd_handle_t server);

#endif
