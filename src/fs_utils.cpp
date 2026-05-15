#include "fs_utils.h"
#include <LittleFS.h>

bool fs_init() {
    Serial.println("Montando LittleFS...");

    // NO formatear automáticamente
    if (!LittleFS.begin(false)) {
        Serial.println("❌ Error montando LittleFS");
        return false;
    }

    Serial.println("✔ LittleFS montado correctamente");

    // Comprobar index.html
    if (!LittleFS.exists("/index.html")) {
        Serial.println("❌ index.html NO encontrado");
        return false;
    }

    Serial.println("✔ index.html encontrado");
    return true;
}

void fs_list() {
    Serial.println("📂 Contenido de LittleFS:");

    File root = LittleFS.open("/");
    if (!root) {
        Serial.println("❌ No se pudo abrir la raíz");
        return;
    }

    File file = root.openNextFile();
    if (!file) {
        Serial.println("⚠ LittleFS está vacío");
        return;
    }

    while (file) {
        Serial.printf(" - %s (%u bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }
}

bool fs_exists(const char *path) {
    return LittleFS.exists(path);
}
