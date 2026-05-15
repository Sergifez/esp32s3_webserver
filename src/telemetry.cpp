#include "telemetry.h"

float sim_speed = 0;
float sim_battery = 100;
float sim_distance = 0;
float sim_rivalDist = 10;
int sim_lap = 1;

unsigned long lastSimUpdate = 0;

void telemetry_init() {
    sim_speed = 0;
    sim_battery = 100;
    sim_distance = 0;
    sim_rivalDist = 10;
    sim_lap = 1;
    lastSimUpdate = 0;
}

void telemetry_update() {
    unsigned long now = millis();
    if (now - lastSimUpdate < 100) return;
    lastSimUpdate = now;

    sim_speed += (random(-5, 6) * 0.2);
    if (sim_speed < 0) sim_speed = 0;
    if (sim_speed > 40) sim_speed = 40;

    sim_distance += sim_speed * 0.1 * 0.1;

    if (sim_distance > sim_lap * 500) sim_lap++;

    sim_rivalDist += (random(-3, 4) * 0.1);
    if (sim_rivalDist < 0) sim_rivalDist = 0;
    if (sim_rivalDist > 20) sim_rivalDist = 20;

    sim_battery -= 0.002;
    if (sim_battery < 0) sim_battery = 0;
}

String telemetry_json() {
    String json = "{";
    json += "\"speed\":"     + String(sim_speed, 1)     + ",";
    json += "\"battery\":"   + String(sim_battery, 1)   + ",";
    json += "\"lap\":"       + String(sim_lap)          + ",";
    json += "\"distance\":"  + String(sim_distance, 1)  + ",";
    json += "\"rivalDist\":" + String(sim_rivalDist, 1);
    json += "}";
    return json;
}
