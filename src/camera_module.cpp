#include "camera_module.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include <Arduino.h>

// Traemos el server global
extern httpd_handle_t server;

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// PINOUT FREENOVE ESP32-S3 WROOM
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13

camera_config_t cam_cfg = {
    .pin_pwdn       = PWDN_GPIO_NUM,
    .pin_reset      = RESET_GPIO_NUM,
    .pin_xclk       = XCLK_GPIO_NUM,
    .pin_sscb_sda   = SIOD_GPIO_NUM,
    .pin_sscb_scl   = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,

    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href  = HREF_GPIO_NUM,
    .pin_pclk  = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,
    .frame_size   = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count     = 2,
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .grab_mode    = CAMERA_GRAB_WHEN_EMPTY
};

esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t jpg_len = 0;
    uint8_t *jpg_buf = NULL;
    char part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) return res;

    while (true) {

        fb = esp_camera_fb_get();
        if (!fb) {
            res = ESP_FAIL;
            break;
        }

        // Si ya viene en JPEG → NO convertir
        if (fb->format == PIXFORMAT_JPEG) {
            jpg_buf = fb->buf;
            jpg_len = fb->len;
        } 
        else {
            // Convertir solo si es necesario
            if (!frame2jpg(fb, 80, &jpg_buf, &jpg_len)) {
                esp_camera_fb_return(fb);
                res = ESP_FAIL;
                break;
            }
        }

        // Enviar cabecera
        size_t hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, jpg_len);
        res = httpd_resp_send_chunk(req, part_buf, hlen);
        if (res != ESP_OK) break;

        // Enviar frame
        res = httpd_resp_send_chunk(req, (const char *)jpg_buf, jpg_len);
        if (res != ESP_OK) break;

        // Enviar boundary
        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if (res != ESP_OK) break;

        // Liberar memoria correctamente
        if (fb->format != PIXFORMAT_JPEG) {
            free(jpg_buf);
        }
        esp_camera_fb_return(fb);

        jpg_buf = NULL;
        fb = NULL;
    }

    if (fb) esp_camera_fb_return(fb);
    if (jpg_buf && fb == NULL) free(jpg_buf);

    return res;
}


void camera_init() {
    esp_err_t err = esp_camera_init(&cam_cfg);
    if (err != ESP_OK) {
        Serial.printf("❌ Error cámara: 0x%x\n", err);
    } else {
        Serial.println("📸 Cámara ienicializada");
    }
}