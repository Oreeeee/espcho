#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

//#include <WebServer.h>
#include <esp_http_server.h>

//extern WebServer httpServer;
static httpd_handle_t httpServerC;

//void initHttpServer();
//void handle404();
esp_err_t scoreSubHandler(httpd_req_t *req);
httpd_handle_t initHttpServer_c();
void stopHttpServer(httpd_handle_t server);
//void initHttpServer();

#endif
