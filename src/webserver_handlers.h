#pragma once
#include "esp_http_server.h"

extern httpd_handle_t server;

// Declaración de la función que registra todas las rutas
void register_web_handlers(httpd_handle_t server);

// Declaración de handlers individuales
esp_err_t root_handler(httpd_req_t *req);
esp_err_t led_on_handler(httpd_req_t *req);
esp_err_t led_off_handler(httpd_req_t *req);
esp_err_t led_handler(httpd_req_t *req);
esp_err_t api_led_handler(httpd_req_t *req);
esp_err_t static_handler(httpd_req_t *req);
