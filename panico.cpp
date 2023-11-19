#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "gps.h"

int pinPanico = 12;
//Estructuracion boton de Panico
struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button panico = {pinPanico, 0, false};

//Funcion de interrupcion
void IRAM_ATTR isr() {
  panico.numberKeyPresses += 1;
  panico.pressed = true;
}

void putPanico (String busPanico, String fechaPanico, String horaPanico, bool botonPanico, String apiPanico);

void setupPanico() {
  pinMode(panico.PIN, INPUT_PULLUP);
  attachInterrupt(panico.PIN, isr, FALLING);  
}

void rutinaPanico(String busPanico, String fechaPanico, String horaPanico, String apiPanico, int periodoPanico, unsigned long TiempoAhoraPanico) {
  if (panico.pressed) {
    if (panico.numberKeyPresses == 1) {
      putPanico (busPanico, fechaPanico, horaPanico, 1, apiPanico);
      Serial.print("PUT HTTP REALIZADA: BOTON DE PANICO ACTIVADO\n");
      //delay(10);
    }
  }
  if ((millis() > TiempoAhoraPanico + periodoPanico) && (panico.numberKeyPresses >= 5)) {
    TiempoAhoraPanico = millis();
    putPanico (busPanico, fechaPanico, horaPanico, 0, apiPanico);
    Serial.print("PUT HTTP REALIZADA: BOTON DE PANICO DESACTIVADO\n");
    panico.numberKeyPresses=0;
    //delay(10);
  }
}

void putPanico (String busPanico, String fechaPanico, String horaPanico, bool botonPanico, String apiPanico) {
  Serial.printf("Boton de panico ha sido presionado %u veces\n", panico.numberKeyPresses);
  panico.pressed = false;
  HTTPClient http;
  http.begin(apiPanico);
  http.addHeader("Content-Type", "application/json");
  //crear JSON object
  DynamicJsonDocument docPanico(2048);
  docPanico["bus"] = busPanico;
  docPanico["fecha"] = fechaPanico;
  docPanico["hora"] = horaPanico;
  docPanico["boton"] = botonPanico;
  // Serialize JSON document
  String jsonPanico;
  serializeJson(docPanico, jsonPanico);
  Serial.println(jsonPanico);
  int httpResponseCode = http.PUT(jsonPanico);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  http.end();
}