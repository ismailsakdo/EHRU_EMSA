#include <MQUnifiedsensor.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "Adafruit_PM25AQI.h"
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(18, 19);

//initialize
Adafruit_HTU21DF htu = Adafruit_HTU21DF();
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();


#define         Board                   ("Arduino NANO")
#define         Pin135                   (34)  //Analog input 2 of your arduino
#define         Pin9                     (35)  //Analog input 7 of your arduino
 
#define         RatioMQ135CleanAir        (3.6) //RS / R0 = 10 ppm 
#define         RatioMQ9CleanAir          (9.6) //RS / R0 = 9.6 ppm 
#define         ADC_Bit_Resolution        (12) // 10 bit ADC 
#define         Voltage_Resolution        (5) // Volt resolution to calc the voltage
#define         Type                      ("Arduino NANO") //Board used

//Declare Sensor
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin135, Type);
MQUnifiedsensor MQ9(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin9, Type);

float co2; float fg; float t; float h; int pm05;
int pm10; int pm25; int pm100;

void setup() {
  Serial.begin(115200);           
  pmSerial.begin(9600);
  MQ135.init();
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setR0(9.03);
  MQ9.init();
  MQ9.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ9.setR0(13.93);
  delay(1000);


  //HTU
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }

  //PMS
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");
}  

void loop() {

//HTU
  t = htu.readTemperature();
  h = htu.readHumidity();
  Serial.print("Temperature:      "); Serial.println(t);
  Serial.print("Humidity:       "); Serial.println(h);
  Serial.println("--------------------------------------------------------");

//MQ sensor
  MQ135.update();  
  MQ9.update();
  MQ135.setA(110.47); MQ135.setB(-2.862); //CO2 
  float co2 = MQ135.readSensor(); 
  MQ9.setA(1000.5); MQ9.setB(-2.186); //flamable gas
  float fg = MQ9.readSensor();

  Serial.print("CO2:      "); Serial.println(co2);
  Serial.print("FG:       "); Serial.println(fg);
  Serial.println("--------------------------------------------------------");
  delay(500);

  //PMS
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");
  pm05 = data.pm10_env;
  pm10 = data.pm25_env;
  pm25 = data.pm100_env; 
  pm100 = data.particles_05um;

  Serial.print("PM0.5:      "); Serial.println(pm05);
  Serial.print("PM1.0:       "); Serial.println(pm10);
  Serial.print("PM2.5:       "); Serial.println(pm25);
  Serial.print("PM10:       "); Serial.println(pm100);
  Serial.println("--------------------------------------------------------");
  delay(500);
}
