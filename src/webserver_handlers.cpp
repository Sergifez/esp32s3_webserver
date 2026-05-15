#include <Arduino.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>
#include "webserver_handlers.h"

extern Adafruit_NeoPixel led;
extern httpd_handle_t server;


// ----------------------
// /
// ----------------------
esp_err_t root_handler(httpd_req_t *req) {
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        httpd_resp_send_404(req);
        return ESP_OK;
    }

    httpd_resp_set_type(req, "text/html");

    char buffer[1024];
    while (true) {
        size_t n = file.readBytes(buffer, sizeof(buffer));
        if (n <= 0) break;
        httpd_resp_send_chunk(req, buffer, n);
    }
    httpd_resp_send_chunk(req, nullptr, 0);
    file.close();
    return ESP_OK;
}

// ----------------------
// /on
// ----------------------
esp_err_t led_on_handler(httpd_req_t *req) {
    led.setPixelColor(0, led.Color(255, 0, 0));
    led.show();
    httpd_resp_sendstr(req, "LED ON");
    return ESP_OK;
}

// ----------------------
// /off
// ----------------------
esp_err_t led_off_handler(httpd_req_t *req) {
    led.setPixelColor(0, led.Color(0, 0, 0));
    led.show();
    httpd_resp_sendstr(req, "LED OFF");
    return ESP_OK;
}

// ----------------------
// /led
// ----------------------
esp_err_t led_handler(httpd_req_t *req) {
    led.setPixelColor(0, led.Color(255, 0, 0));
    led.show();
    httpd_resp_sendstr(req, "LED RED");
    return ESP_OK;
}

// ----------------------
// /api/v1/led (POST JSON)
// ----------------------
esp_err_t api_led_handler(httpd_req_t *req) {
    char buf[256];
    int len = httpd_req_recv(req, buf, sizeof(buf)-1);

    if (len <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No JSON");
        return ESP_FAIL;
    }

    buf[len] = 0;
    String body = String(buf);

    if (body.indexOf("blue") != -1) {
        led.setPixelColor(0, led.Color(0, 0, 255));
    }
    else if (body.indexOf("yellow") != -1) {
        led.setPixelColor(0, led.Color(255, 255, 0));
    }
    else if (body.indexOf("red") != -1) {
        led.setPixelColor(0, led.Color(255, 0, 0));
    }
    else if (body.indexOf("off") != -1) {
        led.setPixelColor(0, led.Color(0, 0, 0));
    }

    led.show();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"message\":\"LED updated\"}");
    return ESP_OK;
}

// ----------------------
// Archivos estáticos
// ----------------------
esp_err_t static_handler(httpd_req_t *req) {
    String path = String(req->uri);
    if (path == "/") path = "/index.html";

    if (!LittleFS.exists(path)) {
        httpd_resp_send_404(req);
        return ESP_OK;
    }

    File file = LittleFS.open(path, "r");
    if (!file) {
        httpd_resp_send_500(req);
        return ESP_OK;
    }

    String type;
    if (path.endsWith(".css")) type = "text/css";
    else if (path.endsWith(".js")) type = "application/javascript";
    else if (path.endsWith(".png")) type = "image/png";
    else if (path.endsWith(".jpg")) type = "image/jpeg";
    else type = "text/plain";

    httpd_resp_set_type(req, type.c_str());

    char buffer[1024];
    while (true) {
        size_t n = file.readBytes(buffer, sizeof(buffer));
        if (n <= 0) break;
        httpd_resp_send_chunk(req, buffer, n);
    }
    httpd_resp_send_chunk(req, nullptr, 0);
    file.close();
    return ESP_OK;
}

// ----------------------
// Registrar TODAS las rutas
// ----------------------
void register_web_handlers(httpd_handle_t server) {

    httpd_uri_t root_uri = { "/", HTTP_GET, root_handler, nullptr };
    httpd_register_uri_handler(server, &root_uri);

    httpd_uri_t on_uri = { "/on", HTTP_GET, led_on_handler, nullptr };
    httpd_register_uri_handler(server, &on_uri);

    httpd_uri_t off_uri = { "/off", HTTP_GET, led_off_handler, nullptr };
    httpd_register_uri_handler(server, &off_uri);

    httpd_uri_t led_uri = { "/led", HTTP_GET, led_handler, nullptr };
    httpd_register_uri_handler(server, &led_uri);

    httpd_uri_t api_led_uri = { "/api/v1/led", HTTP_POST, api_led_handler, nullptr };
    httpd_register_uri_handler(server, &api_led_uri);

    httpd_uri_t static_uri = { "/*", HTTP_GET, static_handler, nullptr };
    httpd_register_uri_handler(server, &static_uri);
}
