
//This code works with Seeedstudio Temp&Humidity sensor v1.2 
//nodeMCU v1.0 (black) with Arduino IDE
//nodemcu pinout https://github.com/esp8266/Arduino/issues/584

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
//Requires the DHT_sensor_library

#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 5     // what digital pin we're connected to

//Def
#define DHTPIN 5     // what digital pin we're connected to, this is pin D1 on NodeMCU board
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define myPeriodic 15 //in sec | Thingspeak pub is 15sec

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);


float prevTemp = 0;
const char* server = "api.thingspeak.com";
String apiKey = "48WOFFFBMO8USN6B";
const char* MY_SSID = "NYUSHIMA";
const char* MY_PWD = "Vlc5k$zf";
int sent = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("DHTxx test!");

  dht.begin();
  connectWifi();
}

void loop() {
  delay(2000);
  float temp  = dht.readTemperature();
  float hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  //  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  //  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, hum);
  //  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, hum, false);
  //
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  Serial.print(String(sent) + " Temperature: ");
  Serial.println(temp);

  //String humout = dtostrf(hum, 4, 1, buffer);//handled in sendTemp()

  sendTeperatureTS(temp,hum);
   
  //int count = myPeriodic;
  //while (count--)
    delay(1000);
}


void connectWifi()
{
  Serial.print("Connecting to " + *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected");
  Serial.println("");
}//end connect

void sendTeperatureTS(float temp, float hum)
{
  WiFiClient client;

  if (client.connect(server, 80)) { // use ip 184.106.153.149 or api.thingspeak.com
    Serial.println("WiFi Client connected ");

    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "&field2=";
    postStr += String(hum);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    delay(1000);

  }//end if
  sent++;
  client.stop();
}//end send

