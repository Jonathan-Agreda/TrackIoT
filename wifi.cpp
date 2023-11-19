#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include "redAP.h"

void setupWifi() {
  WiFi.mode(WIFI_STA); // Confihrua LORA para OPERAR EN SATA O AP

    Serial.begin(115200);
    
    WiFiManager wifim;
    // wm.resetSettings(); //resetear las configuraciones wifi almacenadas en LORA

    bool res;
    res = wifim.autoConnect(ssid_AP, password_AP); // CREA AP para configuracion inicial de red WiFi

    if(!res) {
        Serial.println("");
        Serial.println("Failed to connect");
        // ESP.restart(); //reiniciar LORA
    } 
    else {
        Serial.println("");
        Serial.print("Conectado a red Wifi: ");
        Serial.println(WiFi.SSID());
        Serial.println("");
        Serial.print("Con IP Address: ");
        Serial.println(WiFi.localIP());
    }

    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
}