#include "wifi.h"
#include "gps.h"
#include "telegram.h"


void setup() {
  setupWifi();
  setupGps();
  setupTelegram();
}

void loop() {
  rutinaGps();
  rutinaTelegram();
}