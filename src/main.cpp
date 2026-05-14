/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-littlefs/ 
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

// Import required libraries
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

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

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


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

  void iniciarWiFiAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP("ESP32_TEST", "12345678");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  led.begin();
  led.setBrightness(BRIGHTNESS);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
    File file = LittleFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  file.close();

  // Connect to Wi-Fi
  iniciarWiFiAP();

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    led.setPixelColor(0, led.Color(255, 0, 0)); // Red
    led.show();
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    led.setPixelColor(0, led.Color(0, 0, 0)); // Off
    led.show();
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

server.on("/api/v1/led", HTTP_POST, [](AsyncWebServerRequest *request){},
NULL,
[](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

    String body = "";
    for (size_t i = 0; i < len; i++) body += (char)data[i];

    Serial.println("JSON recibido:");
    Serial.println(body);

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
    request->send(200, "application/json", "{\"message\":\"LED updated\"}");
});

  // Route to set GPIO to HIGH
  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    led.setPixelColor(0, led.Color(255, 0, 0)); // Red
    led.show();
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    led.setPixelColor(0, led.Color(0, 0, 0)); // Off
    led.show();
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  server.serveStatic("/", LittleFS, "/"); //sirve todos los archivos de la carpeta Data
  // Start server
  server.begin();
}
 
void loop(){
}