#include <Adafruit_BMP085.h>
#include "DHT.h"

#define DHTPIN 4
#define sensorpin A0

Adafruit_BMP085 bmp;

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

float t; float h; float p; float alt; float rain;

void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }

  //hidupkan sensor TH
  dht.begin();

  //hidupkan sensor rain
  pinMode(sensorpin, INPUT);

}

void loop() {

  //Sensor Pressure and Altitude
  p = bmp.readPressure();
  alt = bmp.readAltitude();
  Serial.print("Pressure:      "); Serial.println(p);
  Serial.print("Altitude:       "); Serial.println(alt);
  
  //Temp Humidity
  t = dht.readTemperature();
  h = dht.readHumidity();
  Serial.print("Temperature:       "); Serial.println(t);
  Serial.print("Humidity:       "); Serial.println(h);
  
  //Hujan
  rain = analogRead(sensorpin); 
  Serial.print("Rain:       "); Serial.println(rain);
  Serial.println("--------------------------------------------------------");
  delay(500);
  
}
