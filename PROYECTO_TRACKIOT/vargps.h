//Variables de GPS y Panico

String apiGps = "http://135.148.148.73:3000/api/gps";
String apiPanico = "http://135.148.148.73:3000/api/panico";
String apiDevicesGps = "http://135.148.148.73:3000/api/devicesgps/1";
String apiDevicesPanico = "http://135.148.148.73:3000/api/devicespanico/1";
String bus = "abcd-1234"; 
int periodoGpsPut = 20000; unsigned long TiempoAhoraGpsPut = 0;
int periodoPanico = 5000; unsigned long TiempoAhoraPanico = 0;

String fechaGps = "dd/mm/aaaa";
String horaGps = "hh:mm:ss" ;
