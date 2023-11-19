#include <Arduino.h>
#include <SoftwareSerial.h>//incluimos SoftwareSerial
#include <TinyGPS.h>//incluimos TinyGPS
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "panico.h"
#include "vargps.h"

//Variables Pinex RX y TX LORA32
int pinRxGps = 16;
int pinTxGps = 17;

TinyGPS gps;//Declaramos el objeto gps
SoftwareSerial serialgps(pinRxGps,pinTxGps);//Declaramos el pin 4 Tx y 3 Rx

//Declaramos la variables para la obtención de datos
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;
float latitude, longitude;
int velocidad;
String velocidadString;

void putGps (String busGps, float latitudGps,float longitudGps, String fechaGps, String horaGps, int velocidadGps, String apiDevicesGps);
void postGps (String busGps, float latitudGps,float longitudGps, String fechaGps, String horaGps, int velocidadGps, String apiGps);

void setupGps() {
  setupPanico();
  serialgps.begin(9600);//Iniciamos el puerto serie del gps
  //Imprimimos:
  Serial.println("");
  Serial.println("GPS: GY-NEO6MV2");
  Serial.println(" ---Buscando senal--- ");
  Serial.println("");
}

void rutinaGps() {
  while(serialgps.available()) 
  {
    rutinaPanico(bus, fechaGps, horaGps, apiPanico, periodoPanico, TiempoAhoraPanico, apiDevicesPanico);

    int c = serialgps.read();
    if(gps.encode(c))  
    {
      gps.f_get_position(&latitude, &longitude);
      Serial.print("Latitud/Longitud: "); 
      Serial.print(latitude,5); 
      Serial.print(", "); 
      Serial.println(longitude,5);


      gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

      int minuto = minute + 0;
      int hora = hour;
      int dia = day;
      int timeZoneMinuto = 0;
      int timeZoneHora = 5;
      String minutoCorregido;
      String horaCorregida;
      minuto = minuto + timeZoneMinuto;
      if (minuto > 59) {
        minuto = minuto - 60;
        hora = hora + 1;
      }
      else {
        if (minuto < 0) {
          minuto = minuto + 60;
          hora = hora - 1;
        }
      }
      hora = hora - timeZoneHora;
      if (hora > 23) {
        hora = hora - 24;
      }
      else {
        if (hora < 0) {
          hora = hora + 24;
        }
      }
      if (hora >=19) {
          dia = dia - 1;
      }
      if (minuto <= 9) {
          minutoCorregido = "0" + (String) minuto;
      } else {
          minutoCorregido = (String) minuto;
      }
      if (hora <= 9) {
          horaCorregida = "0" + (String) hora;
      } else {
          horaCorregida = (String) hora;
      }
      fechaGps = (String) dia + "/" + (String)  month + "/"+ (String) year;
      horaGps = horaCorregida + ":" + minutoCorregido + ":" + (String) second;
      Serial.print("Fecha: ");
      Serial.println(fechaGps);
      Serial.print(" Hora: ");
      Serial.println(horaGps);
      Serial.print("Altitud (metros): ");
      Serial.println(gps.f_altitude()); 
      Serial.print("Rumbo (grados): "); Serial.println(gps.f_course()); 
      Serial.print("Velocidad(kmph): ");
      velocidad = gps.f_speed_kmph();
      velocidadString= (String) velocidad;
      Serial.println(velocidadString);
      Serial.print("Satelites: "); Serial.println(gps.satellites());
      Serial.println();
      gps.stats(&chars, &sentences, &failed_checksum);
    }  
    
    if(millis() > TiempoAhoraGpsPut + periodoGpsPut) {
      TiempoAhoraGpsPut = millis();
      putGps (bus, latitude, longitude, fechaGps, horaGps, velocidad, apiDevicesGps);
      postGps (bus, latitude, longitude, fechaGps, horaGps, velocidad, apiGps);
      //delay(500);
    }
  }
}

void putGps (String busGps, float latitudGps,float longitudGps, String fechaGps, String horaGps, int velocidadGps, String apiDevicesGps) {
  Serial.println();
  Serial.print("Enviando datos de /Device-GPS a TrackIoT: ");
  HTTPClient http;
  http.begin(apiDevicesGps);
  http.addHeader("Content-Type", "application/json");
  //crear JSON object
  DynamicJsonDocument docGps(2048);
  docGps["bus"] = busGps;
  docGps["latitud"] = latitudGps;
  docGps["longitud"] = longitudGps;
  docGps["fecha"] = fechaGps;
  docGps["hora"] = horaGps;
  docGps["velocidad"] = velocidadGps;
  // Serialize JSON document
  String jsonGps;
  serializeJson(docGps, jsonGps);
  Serial.println(jsonGps);
  int httpResponseCode = http.PUT(jsonGps);
  Serial.print("HTTP PUT Response code: ");
  Serial.println(httpResponseCode);
  Serial.println();
  http.end();
}

void postGps (String busGps, float latitudGps,float longitudGps, String fechaGps, String horaGps, int velocidadGps, String apiGps) {
  Serial.println();
  Serial.print("Enviando datos de /GPS a TrackIoT: ");
  HTTPClient http;
  http.begin(apiGps);
  http.addHeader("Content-Type", "application/json");
  //crear JSON object
  DynamicJsonDocument docGps(2048);
  docGps["bus"] = busGps;
  docGps["latitud"] = latitudGps;
  docGps["longitud"] = longitudGps;
  docGps["fecha"] = fechaGps;
  docGps["hora"] = horaGps;
  docGps["velocidad"] = velocidadGps;
  // Serialize JSON document
  String jsonGps;
  serializeJson(docGps, jsonGps);
  Serial.println(jsonGps);
  int httpResponseCode = http.POST(jsonGps);
  Serial.print("HTTP POST Response code: ");
  Serial.println(httpResponseCode);
  Serial.println();
  http.end();
}