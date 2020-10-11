#include "DHTesp.h"
#include <ESP8266WiFi.h>       // WiFi library
#include "config.h"            // přihlašovací údaje k wifi síti
//obsah souboru config.h
//----------------------------------
// #define WIFI_SSID "jmenosite"
// #define WIFI_PASSWORD "heslo"
//----------------------------------


// Define settings
// konstatnty WIFI_.... definovány v souboru config.h
// údaje lze zapsat přímo: ...ssid[] = "jmenosite" 
const char ssid[]     = WIFI_SSID; // WiFi SSID
const char pass[]     = WIFI_PASSWORD; // WiFi password
const char domain[]   = "ko-fikmves";  // domain.tmep.cz
const char guid[]     = "koupelna"; // mojemereni
const byte sleep      = 1; // How often send data to the server. In minutes
DHTesp dht;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Humidity (%)\tTemperature (C)");
  dht.setup(5, DHTesp::DHT11); // Connect DHT sensor to D1

  // Connect to the WiFi
  Serial.print(F("Connecting to ")); Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW); delay(100); digitalWrite(LED_BUILTIN, HIGH); // Blinking with LED during connecting to WiFi
    delay(500);
    //digitalWrite(2, HIGH); // Blinking with LED during connecting to WiFi
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: ")); Serial.println(WiFi.localIP());
  Serial.println();
}

void loop()
{
  delay(dht.getMinimumSamplingPeriod());

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.println(temperature, 1);
  //Serial.print("\t\t");
  //Serial.println(dht.computeHeatIndex(temperature, humidity, false), 1);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);

 // Connect to the HOST and send data via GET method
  WiFiClient client; // Use WiFiClient class to create TCP connections
 
  char host[50];            // Joining two chars is little bit difficult. Make new array, 50 bytes long
  strcpy(host, domain);     // Copy /domain/ in to the /host/
  strcat(host, ".tmep.cz"); // Add ".tmep.cz" at the end of the /host/. /host/ is now "/domain/.tmep.cz"
 
  Serial.print(F("Connecting to ")); Serial.println(host);
  if (!client.connect(host, 80)) {
    // If you didn't get a connection to the server
    Serial.println(F("Connection failed"));
    // Blink 3 times when host connection error
    digitalWrite(2, LOW); delay(100); digitalWrite(2, HIGH); delay(100); digitalWrite(2, LOW); delay(100); digitalWrite(2, HIGH); delay(100); digitalWrite(2, LOW); delay(100); digitalWrite(2, HIGH);
    delay(1000);
    return;
  }
  Serial.println(F("Client connected"));
 
  // Make an url. We need: /?guid=t
  String url = "/?";
         url += guid;
         url += "=";
         url += temperature;
         url += "&humV=";
         url += humidity;
  Serial.print(F("Requesting URL: ")); Serial.println(url);
 
  // Make a HTTP GETrequest.
 
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
 // Workaroud for timeout
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }              


 
  // Blik 1 time when send OK
  Serial.println("OK");
  digitalWrite(2, LOW); delay(100); digitalWrite(2, HIGH);
    // Wait for another round
  delay(sleep*60000);

}
