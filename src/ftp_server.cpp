#include <Arduino.h>
#include <SimpleFTPServer.h>
#include <LittleFS.h>

FtpServer ftpSrv;

void _callback(FtpOperation ftpOperation, uint32_t freeSpace, uint32_t totalSpace){
  switch (ftpOperation) {
    case FTP_CONNECT:
      Serial.println(F("FTP: Connected!"));
      break;
    case FTP_DISCONNECT:
      Serial.println(F("FTP: Disconnected!"));
      break;
    case FTP_FREE_SPACE_CHANGE:
      Serial.printf("FTP: Free space change, free %lu of %lu!\n", (unsigned long)freeSpace, (unsigned long)totalSpace);
      break;
    default:
      break;
  }
};
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, uint32_t transferredSize){
  switch (ftpOperation) {
    case FTP_UPLOAD_START:
      Serial.println(F("FTP: Upload start!"));
      break;
    case FTP_UPLOAD:
      Serial.printf("FTP: Upload of file %s byte %lu\n", name, (unsigned long)transferredSize);
      break;
    case FTP_TRANSFER_STOP:
      Serial.println(F("FTP: Finish transfer!"));
      break;
    case FTP_TRANSFER_ERROR:
      Serial.println(F("FTP: Transfer error!"));
      break;
    default:
      break;
  }

  /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */
};


void startFTP() {
    Serial.println("Iniciando servidor FTP...");


    /////FTP Setup, ensure SPIFFS is started before ftp;  /////////

    /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
    #ifdef ESP32       //esp32 we send true to format spiffs if cannot mount
    if (LittleFS.begin(true)) {
    #elif defined ESP8266
    if (LittleFS.begin()) {
    #endif
        ftpSrv.setCallback(_callback);
        ftpSrv.setTransferCallback(_transferCallback);

        Serial.println("LittleFS opened!");
        ftpSrv.begin("user","password");    //username, password for ftp.   (default 21, 50009 for PASV)
    }

    // Usuario y contraseña
    ftpSrv.begin("esp32","esp32");
    
    Serial.println("FTP listo:");
    Serial.println("  Usuario: esp32");
    Serial.println("  Password: esp");
    Serial.println("  Puerto: 21");
}

void handleFTP() {
    ftpSrv.handleFTP();
}
