#ifndef OSUDIRECT_H
#define OSUDIRECT_H
#include <esp_http_server.h>

// XXX: This might be changed if memory starts being an issue
#define OSU_DIRECT_MAP_CHUNK_SIZE 8096

esp_err_t directSearchHandler(httpd_req_t *req);
esp_err_t directDownloadHandler(httpd_req_t *req);

#endif //OSUDIRECT_H
