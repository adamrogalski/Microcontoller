#include <DHT.h>
#include <Wire.h>
#include <math.h>
#include <BH1750.h>
#include <LPS331.h>
#include <ESP8266WiFi.h>
 
String apiKey = "E4060MG7SIXSPCQC";// API key from thingspeak.com to Poznan
//String apiKey = "8TNFD97N42OCGG56";// API key from thingspeak.com to Inowroclaw
const char* ssid = "iPhone (Adam)"; // name of ssid wifi
const char* password = "lokowios"; // wifi password 
const char* server = "api.thingspeak.com";
 
#define DHTPIN D4 // DHT11 pin
#define DHTTYPE DHT11// type of DHT sensor
 
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
LPS331 ps;
WiFiClient client;
 
void setup()
{
  Wire.begin();
  lightMeter.begin();
  dht.begin();
  Serial.begin(115200);
  delay(10);
  dht.begin();
  
  //Pressure sensor
  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }

  ps.enableDefault();

 // Connecting to the network
  WiFi.begin(ssid, password);
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
}
 
void loop()
{  
  //Light sensor
  uint16_t light = lightMeter.readLightLevel();
  
  //Pressure sensor
  float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  float temperature2 = ps.readTemperatureC();

  // Reading the temperature and humidity of the air
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("Error reading DHT sensor data");
    return;
  }
 
  if (client.connect(server,80)) 
  {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(temperature);
    postStr +="&field2=";
    postStr += String(humidity);
    postStr +="&field3=";
    postStr += String(pressure);
    postStr +="&field4=";
    postStr += String(light);
    postStr +="&field5=";
    postStr += String(temperature2);
    postStr += "\r\n\r\n";
 
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
 
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println("Humidity: ");
    Serial.print(humidity);
    Serial.println("Send Thingspeak Data");
    Serial.print("Light: ");
    Serial.print(light);
    Serial.println(" lx");
    Serial.print("p: ");
    Serial.print(pressure);
    Serial.print(" mbar\ta: ");
    Serial.print(altitude);
    Serial.print(" m\tt: ");
    Serial.print(temperature2);
    Serial.println(" deg C");
      
  }
  client.stop();
 
  Serial.println("Waiting 1 Second to send to thingspeak.com");
  delay(1000);
}
