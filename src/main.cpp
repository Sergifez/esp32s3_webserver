#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>

#include "fs_utils.h"
#include "esp_http_server.h"
#include "webserver_handlers.h"
#include "ftp_server.h"
#include "telemetry.h"
#include "camera_module.h"


// Import required libraries
#include <FS.h>
#include <Wire.h>

// ---------------- LED ----------------
#define LED_PIN    48
#define LED_COUNT  1
#define BRIGHTNESS 50

Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Replace with your network credentials
const char* ssid = "ESP";
const char* password = "123456789";

// Stores LED state
String ledState;

// Handle del servidor
httpd_handle_t server = nullptr;

// ----------------------
// Iniciar servidor HTTP
// ----------------------
void startWebServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) == ESP_OK) {
        register_web_handlers(server);
        Serial.println("Servidor HTTP iniciado en puerto 80");
    } else {
        Serial.println("Error iniciando servidor HTTP");
    }
}

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(led.getPixelColor(0) == 0){
      ledState = "OFF";
    }
    else{
      ledState = "ON";
    }
    Serial.println(ledState);
    return ledState;
  }
  return String();
}

// ----------------------
// Iniciar WiFi en modo AP
// ----------------------
void iniciarWiFiAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32_TEST", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

// ----------------------
// SETUP
// ----------------------
void setup(){
  Serial.begin(115200);   // Serial port for debugging purposes
  delay(300);

  // --- Inicializar LED ---
    led.begin();
    led.setBrightness(BRIGHTNESS);
    led.clear();
    led.show();

  // --- Inicializar LittleFS ---
  if (!fs_init()) {
      Serial.println("❌ Error inicializando LittleFS");
      return;
  }

  // --- Listar contenido ---
  fs_list();

  // Connect to Wi-Fi
  iniciarWiFiAP();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Start the web server
  startWebServer();
  
  // FTP server
  startFTP();

  // Inicializar módulo cámara y registrar handlers ---
  camera_init();
}
 
void loop(){
    handleFTP();
}