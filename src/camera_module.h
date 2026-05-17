#pragma once
#include "esp_http_server.h"

// Handler del stream (lo usas en webserver_handlers.cpp)
esp_err_t stream_handler(httpd_req_t *req);

// Inicializa la cámara
void camera_init();
