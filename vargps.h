//Variables de GPS y Panico

String apiGps = "http://135.148.148.73:3000/api/gps/1";
String apiPanico = "http://135.148.148.73:3000/api/panico/1";
String bus = "Piloto"; 
int periodoGpsPut = 20000; unsigned long TiempoAhoraGpsPut = 0;
int periodoPanico = 40000; unsigned long TiempoAhoraPanico = 0;

String fechaGps = "dd/mm/aaaa";
String horaGps = "hh:mm:ss" ;