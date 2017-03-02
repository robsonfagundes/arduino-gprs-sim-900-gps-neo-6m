/*
  GSM and GPS code
  This sketch code to get geolocation with SIM900 and send the coordinates to API
*/

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <inetGSM.h>

InetGSM inet;
TinyGPSPlus gps;
SoftwareSerial serialGps(11, 10);

/**
    setup
*/
void setup() {
  // Serial connection.
  Serial.begin(19200);

  Serial.print(F("------------------------------------------------- \r\n"));
  Serial.println(F("Starting App Arduino"));
  Serial.print(F("------------------------------------------------- \r\n"));

  // Start configuration of shield GPS NEO-6M with baudrate.
  serialGps.begin(9600);

  // Turn on GPRS
  powerUpGprs();

  // Start configuration of shield SIM900 with baudrate and  connect to internet.
  if (gsm.begin(9600))
    Serial.println(F("\nstatus=READY"));
  else Serial.println(F("\nstatus=IDLE"));

  // Mensure and show memory usage
  Serial.println(F("\nMemory usage"));
  Serial.print(freeRam());
}

/**
   loop
*/
void loop() {
  serialGps.listen();
  while (serialGps.available() > 0)  {
    if (gps.encode(serialGps.read())) {
      // Get GPS position and send To API through inet.httpPOST()
      gsm.listen();
      // GPRS attach, put in order APN, username and password.
      if (inet.attachGPRS("zap.vivo.com.br", "vivo", "vivo"))
        Serial.println("status=ATTACHED");
      else Serial.println("status=ERROR");
      gsm.SimpleWriteln("AT+CIFSR");

      Serial.print(F("\r\n\r\n"));
      Serial.print(F("------------------------------------------------- \r\n"));
      Serial.print(F(" GET GEOLOCATION & SEND TO API \r\n"));
      Serial.print(F("------------------------------------------------- \r\n"));
      // get geolocation
      String geo;
      geo.concat("lat=" + String(gps.location.lat(), 6));
      geo.concat("&lng=" + String(gps.location.lng(), 6));
      geo.concat("&spd=" + String(gps.speed.kmph()));
      geo.concat("&uid=gb1");
      // send2Api
      Serial.print(F("\nhttp://192.168.1.1/"));
      Serial.println(geo.c_str());
      Serial.print(F("\n"));
      gsm.SimpleWriteln("AT+CIFSR");
      inet.httpPOST("192.168.1.1", 3000, "/geolocations", geo.c_str(), "", 100); // set url and port
    }
  }
}

/**
   mensure memory usage
*/
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/**
   Power up
*/
void powerUpGprs() {
  pinMode(9, OUTPUT);
  delay(100);
  digitalWrite(9, HIGH);
  Serial.println(F("\nPower Up SIM900!"));
}

