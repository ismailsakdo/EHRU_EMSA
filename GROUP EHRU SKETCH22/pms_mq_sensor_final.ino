
//library
#include <Adafruit_PM25AQI.h>
#include <Wire.h>
#include <MQUnifiedsensor.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include <SoftwareSerial.h>

//initialize PMS
SoftwareSerial pmSerial(12, 14);
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

//INTERNET
#define SECRET_SSID "realme 5s"    // replace MySSID with your WiFi network name
#define SECRET_PASS "5b81043e7ff0"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 1884250     // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "E1UIOCDUNNOHV2BL"   // replace XYZ with your channel write API Key

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
String myStatus = "";
//=======

//intialize
#define         Board                   ("Arduino NANO")
#define         Pin135                   (34)  //Analog input 2 of your arduino
#define         Pin7                     (35)  //Analog input 3 of your arduino

//setup of sensor
#define         RatioMQ135CleanAir        (3.6) //RS / R0 = 10 ppm 
#define         RatioMQ7CleanAir          (27.5) //RS / R0 = 27.5 ppm  
#define         ADC_Bit_Resolution        (12) // 10 bit ADC 
#define         Voltage_Resolution        (5) // Volt resolution to calc the voltage
#define         Type                      ("Arduino NANO") //Board used

//Initialize MQ
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin135, Type);
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin7, Type);

float co; float co2; int pm10; int pm25; int pm100; int pm10env; int pm25env; int pm100env;


void setup() {
  Serial.begin(115200);

  //Hidupkan Sensor
  MQ135.init();
  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.setR0(9.03);

  MQ7.init();
  MQ7.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ7.setR0(5.90);

  //hidupkan sensor PMS
  pmSerial.begin(9600);
  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }
  Serial.println("PM25 found!");

  //hidupkan Wifi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak

}

void loop() {

  //MQ Sensor
  MQ135.update();
  MQ7.update();

  MQ135.setA(110.47); MQ135.setB(-2.862); //CO2
  float CO2 = MQ135.readSensor();

  MQ7.setA(99.042); MQ7.setB(-1.518); //CO
  float co = MQ7.readSensor();

  Serial.print("CO2:      "); Serial.println(CO2);
  Serial.print("CO:       "); Serial.println(co);
  Serial.println("--------------------------------------------------------");

  //PMS Sensor
  PM25_AQI_Data data;

  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");

  int pm10 = data.pm10_standard;
  int pm25 = data.pm25_standard;
  int pm100 = data.pm100_standard;
  int pm10env = data.pm10_env;
  int pm25env = data.pm25_env;
  int pm100env = data.pm100_env;

  Serial.print("PM1:      "); Serial.println(pm10);
  Serial.print("PM2.5:       "); Serial.println(pm25);
  Serial.print("PM10:       "); Serial.println(pm100);
  Serial.print("PM1 Env:       "); Serial.println(pm10env);
  Serial.print("PM2.5 Env:       "); Serial.println(pm25env);
  Serial.print("PM10 Env:       "); Serial.println(pm100env);
  Serial.println("--------------------------------------------------------");


  //ThingSpeak
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ThingSpeak.setStatus(myStatus);

  // write to the ThingSpeak channel - utk tahu status hantar data
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // set the fields with the values
  ThingSpeak.setField(1, co2);
  ThingSpeak.setField(2, co);
  ThingSpeak.setField(3, pm10);
  ThingSpeak.setField(4, pm25);
  ThingSpeak.setField(5, pm100);
  ThingSpeak.setField(6, pm10env);
  ThingSpeak.setField(7, pm25env);
  ThingSpeak.setField(8, pm100env);

  delay(15000); // Wait 20 seconds to update the channel again
}
