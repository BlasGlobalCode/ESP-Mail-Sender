#include <DHT12.h>

#include <ESP8266WiFiSTA.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiServer.h>
#include <ESP8266WiFiType.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiGeneric.h>
#include <WiFiClient.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiAP.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WiFiScan.h>
#include <WiFiUdp.h>
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServerSecure.h>
#include <ESP8266SMTP.h>

#define DHTPIN 14 //Der Sensor wird an PIN 14 angeschlossen    

#define DHTTYPE DHT11    // Es handelt sich um den DHT11 Sensor

DHT dht(DHTPIN, DHTTYPE); //Der Sensor wird ab jetzt mit „dth“ angesprochen
const char* ssid = "SSID";                // WIFI network name
const char* password = "12345";            // WIFI network password
uint8_t connection_state = 0;           // Connected to WIFI or not
uint16_t reconnect_interval = 10000;    // If not connected wait time to try again

uint8_t WiFiConnect(const char* ssID, const char* nPassword)
{
  static uint16_t attempt = 0;
  Serial.print("Connecting to ");
  Serial.print(ssID);
  WiFi.begin(ssID, nPassword);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 50) {
    delay(200);
    Serial.print(".");
  }
  ++attempt;
  Serial.println("");
  if (i == 51) {
    Serial.print(F("Connection: TIMEOUT on attempt: "));
    Serial.println(attempt);
    if (attempt % 2 == 0)
      Serial.println(F("Check if access point available or SSID and Password are correct\r\n"));
    return false;
  }
  Serial.println(F("Connection: ESTABLISHED"));
  Serial.print(F("Got IP address: "));
  Serial.println(WiFi.localIP());
  return true;
}

void Awaits(uint16_t interval)
{
  uint32_t ts = millis();
  while (!connection_state) {
    delay(50);
    if (!connection_state && millis() > (ts + interval)) {
      connection_state = WiFiConnect(ssid, password);
      ts = millis();
    }
  }
}


void setup() {
  Serial.begin(9600); //Serielle Verbindung starten
  dht.begin(); //DHT11 Sensor starten
}

void loop() {

  delay(60000); //Zwei Sekunden Vorlaufzeit bis zur Messung (der Sensor ist etwas träge)

  float Luftfeuchtigkeit = dht.readHumidity(); //die Luftfeuchtigkeit auslesen und unter „Luftfeutchtigkeit“ speichern

  float Temperatur = dht.readTemperature();//die Temperatur auslesen und unter „Temperatur“ speichern

  Serial.print("Luftfeuchtigkeit: "); //Im seriellen Monitor den Text und
  Serial.print(Luftfeuchtigkeit); //die Dazugehörigen Werte anzeigen
  Serial.println(" %");
  Serial.print("Temperatur: ");
  Serial.print(Temperatur);
  Serial.println(" Grad Celsius");


  connection_state = WiFiConnect(ssid, password);

  if (!connection_state) {        // if not connected to WIFI
    Awaits(reconnect_interval);         // constantly trying to connect
  }

  uint32_t startTime = millis();

  SMTP.setEmail("smtp@gmail.com").setPassword("yourPassword").Subject("ESP8266SMTP Gmail test").setFrom("ESP8266SMTP").setForGmail();           // simply sets port to 465 and setServer("smtp.gmail.com");
  // message text from http://www.blindtextgenerator.com/lorem-ipsum
  if (SMTP.Send("Recipient Email")) {
    Serial.println(F("Message sent"));
  } else {
    Serial.print(F("Error sending message: "));
    Serial.println(SMTP.getError());
  }

  Serial.println(millis() - startTime);
}
