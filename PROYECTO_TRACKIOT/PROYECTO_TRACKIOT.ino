#include "wifi.h"
#include "gps.h"

void setup() {
  setupWifi();
  setupGps();
}

void loop() {
  rutinaGps();
}
