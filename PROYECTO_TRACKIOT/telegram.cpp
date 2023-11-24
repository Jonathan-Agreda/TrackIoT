#include <Arduino.h>
#include "vartelegram.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "time.h"
#include "sntp.h"
#include "gps.h"

//Variables para obtener la horade la red
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 0;

int ledPin = 13;
bool ledState = LOW;
int controlAlarma = 0;

// Checks for new messages every 1 second.
unsigned long lastTimeBotRan;
unsigned long lastmensaje;
unsigned long lastalarma;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

String fechaSntp;
String horaSntp;

void printLocalTime();
void timeavailable(struct timeval *t);
void handleNewMessages(int numNewMessages);

void setupTelegram() {
  Serial.println("TELEGRAM");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  sntp_set_time_sync_notification_cb( timeavailable );
  sntp_servermode_dhcp(1); 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void rutinaTelegram() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    printLocalTime();
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Hay mensajes pendientes...");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  if ((millis() > periodoMensaje + lastmensaje) && ledState == 0 )  {
    lastmensaje = millis();
    Serial.println("Enviando mensaje recurrente...");
    String mensajeUbicacion = "<b>TrackIoT</b>⚡!!.\n\n";
    mensajeUbicacion += "Te enviamos la <b>ubicacion</b> periodica del dispositivo:\n\n";
    mensajeUbicacion += "<b>Bus: </b> <i>"+bus+"</i> 🚌\n";   
    mensajeUbicacion += "<b>Velocidad: </b> <i>"+String(velocidad)+"</i> 🏎\n\n";    
    String link = "https://maps.google.com/?q=" + String(latitude, 5) + "," + String(longitude, 5);
    mensajeUbicacion += "<a href='"+link+"'>ubicacion</a> 🗺\n\n";
    mensajeUbicacion += "<i>Gracias por preferirnos </i>🛰\n";
    bot.sendChatAction(CHAT_ID, "typing");
    bot.sendMessage(CHAT_ID, mensajeUbicacion, {parse_mode : "HTML"});
    bot.sendChatAction(CHAT_ID_2, "typing");
    bot.sendMessage(CHAT_ID_2, mensajeUbicacion, {parse_mode : "HTML"});
  }

  if ((millis() > periodoAlarma + lastalarma) && ledState == 1 )  {
    lastalarma = millis();
    Serial.println("Alarma: Boton de Panico Activado!!!...");
    String mensajeUbicacion = "<b>TrackIoT ALARMA</b>🔥!!.\n\n";
    mensajeUbicacion += "Te enviamos la <b>ubicacion</b> del dispositivo:\n\n";
    mensajeUbicacion += "<b>Bus: </b> <i>"+bus+"</i> 🚌\n";   
    mensajeUbicacion += "<b>Velocidad: </b> <i>"+String(velocidad)+"</i> 🏎\n\n";    
    String link = "https://maps.google.com/?q=" + String(latitude, 5) + "," + String(longitude, 5);
    mensajeUbicacion += "<a href='"+link+"'>ubicacion</a> 🗺\n\n";
    mensajeUbicacion += "Por favor atiende la <b>🚨🚓🚔Alarma: Boton de Panico activado 🚨!!!!</b>\n\n";
    mensajeUbicacion += "<i>Gracias por preferirnos </i>🛰\n";
    bot.sendChatAction(CHAT_ID, "typing");
    bot.sendMessage(CHAT_ID, mensajeUbicacion, {parse_mode : "HTML"});
    bot.sendChatAction(CHAT_ID_2, "typing");
    bot.sendMessage(CHAT_ID_2, mensajeUbicacion, {parse_mode : "HTML"});
  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("El tiempo no esta habilitado todavia.");
    return;
  }
  char fechaRed[12];
  char horaRed[9];
  strftime(fechaRed, sizeof(fechaRed), "%d/%m/%Y", &timeinfo);
  strftime(horaRed, sizeof(horaRed), "%H:%M:%S", &timeinfo);
  fechaSntp = (String) fechaRed;
  fechaGps = fechaSntp;
  horaSntp = (String) horaRed;
  horaGps = horaSntp;
  Serial.print("Fecha: ");
  Serial.println(fechaSntp);
  Serial.print("Hora: ");
  Serial.println(horaSntp);
  Serial.println();
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Ajuste de tiempo obtenido por NTP!");
  printLocalTime();
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("Manejar mensaje Nuevo: ");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if ( (chat_id != CHAT_ID) && (chat_id != CHAT_ID_2) ){
      String from_name = bot.messages[i].from_name;
      String mensaje_no_autorizado = "Hola, <i>" + from_name + "</i>.\n";
      mensaje_no_autorizado += "Soy 📡<b>TrackIoT</b>🛰!!! \n\n";
      mensaje_no_autorizado += "Lo siento, no estas autorizado 🛑 para usar la Plataforma!\n\n";
      mensaje_no_autorizado += "Contactanos pronto y te ayudaremos ⚡!...\n";
      bot.sendChatAction(chat_id, "typing");
      bot.sendMessage(chat_id, mensaje_no_autorizado, {parse_mode : "HTML"});
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String inicio = "*Bienvenido*, <b>" + from_name + "</b>!!.\n";
      inicio += "Usa los siguientes comandos para controlar 📡<b>TrackIoT</b>🛰.\n\n";
      inicio += "<b>/ubicacion</b> - Para obtener link de ubicacion del Dispositivo 🗺\n";
      inicio += "<b>/estadopanico</b> - Para ver el estado del Boton de Panico ❓\n";
      inicio += "<b>/activarpanico</b> - Para activar Boton de Panico 🚨\n";
      inicio += "<b>/desarctivarpanico</b> - Para desactivar Boton de Panico 📴\n";
      bot.sendChatAction(chat_id, "typing");
      bot.sendMessage(chat_id, inicio, {parse_mode : "HTML"});
    }

    if (text == "/ubicacion") {
      bot.sendChatAction(chat_id, "typing");
      String mensajeUbicacion = "<b>TrackIoT</b>⚡!!.\n\n";
      mensajeUbicacion += "Te enviamos la <b>ubicacion</b> solicitada del dispositivo:\n\n";
      mensajeUbicacion += "<b>Bus: </b> <i>"+bus+"</i> 🚌\n";   
      mensajeUbicacion += "<b>Velocidad: </b> <i>"+String(velocidad)+"</i> 🏎\n\n";    
      String link = "https://maps.google.com/?q=" + String(latitude, 5) + "," + String(longitude, 5);
      mensajeUbicacion += "<a href='"+link+"'>ubicacion</a> 🗺\n\n";
      mensajeUbicacion += "<i>Gracias por preferirnos </i>🛰\n";
      bot.sendMessage(chat_id, mensajeUbicacion, {parse_mode : "HTML"});
    }

    if (text == "/estadopanico") {
      bot.sendChatAction(chat_id, "typing");
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "Boton de Panico esta <b>activado</b> 🚨", {parse_mode : "HTML"});
      }
      else{
        bot.sendMessage(chat_id, "Boton de Panico esta <b>desactivado</b> 📴", {parse_mode : "HTML"});
      }
    }
    
    if (text == "/activarpanico") {
      bot.sendChatAction(chat_id, "typing");
      bot.sendMessage(chat_id, "Boton de Panico <b>activado</b> 🚨🚨🚨!", {parse_mode : "HTML"});
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      controlAlarma = 1;
    }

    if (text == "/desarctivarpanico") {
      bot.sendChatAction(chat_id, "typing");
      bot.sendMessage(chat_id, "Boton de Panico <b>desactivado</b> 📴📴📴", {parse_mode : "HTML"});
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      controlAlarma = 2;
    }
    
  }
}