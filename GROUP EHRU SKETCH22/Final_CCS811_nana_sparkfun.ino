//library
#include <Wire.h>
#include <MQUnifiedsensor.h>
#include "Adafruit_HTU21DF.h"
#include "SparkFunCCS811.h"
#include <WiFi.h>
#include "ThingSpeak.h"

//INTERNET
#define SECRET_SSID "yaya"          // replace MySSID with your WiFi network name
#define SECRET_PASS "alyazulaikha"  // replace MyPassword with your WiFi password

#define SECRET_CH_ID 1884273                    // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "9648VSSMTSOK3U2H"  // replace XYZ with your channel write API Key
#define CCS811_ADDR 0x5A

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;
String myStatus = "";
//=======


//intialize
#define Board ("Arduino NANO")
#define Pin135 (32)  //Analog input 2 of your arduino
#define Pin7 (35)    //Analog input 3 of your arduino

//setup of sensor
#define RatioMQ135CleanAir (3.6)  //RS / R0 = 10 ppm
#define RatioMQ7CleanAir (27.5)   //RS / R0 = 27.5 ppm
#define ADC_Bit_Resolution (12)   // 10 bit ADC
#define Voltage_Resolution (5)    // Volt resolution to calc the voltage
#define Type ("Arduino NANO")     //Board used

//Initialize TH
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

//Initialize MQ
MQUnifiedsensor MQ135(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin135, Type);
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin7, Type);

//initialize
CCS811 mySensor(CCS811_ADDR);

// variable
float nh4;
float co;
float t;
float h;
float co2;
float tvoc;


void setup() {
  Serial.begin(115200);

  //Hidupkan Sensor
  MQ135.init();
  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ135.setR0(9.03);

  MQ7.init();
  MQ7.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ7.setR0(5.90);

  //hidupkan sensor HTU
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1)
      ;
  }

  //hidup sensor CCS
  if (mySensor.begin() == false) {
    Serial.print("CCS811 error. Please check wiring. Freezing...");
    while (1)
      ;
  }
  //hidupkan Wifi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  //MQ Sensor
  MQ135.update();
  MQ7.update();

  MQ135.setA(102.2);
  MQ135.setB(-2.473);  //NH4
  float nh4 = MQ135.readSensor();

  MQ7.setA(99.042);
  MQ7.setB(-1.518);  //CO
  float co = MQ7.readSensor();

  Serial.print("NH4 (PPM):      ");
  Serial.println(nh4);
  Serial.print("CO (PPM):       ");
  Serial.println(co);
  Serial.println("--------------------------------------------------------");

  //Temperature Humidity
  t = htu.readTemperature();
  h = htu.readHumidity();
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" C");
  Serial.print("\t\t");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" \%");
  Serial.println("--------------------------------------------------------");

  //CCS TVOC and CO2
  if (mySensor.dataAvailable()) {
    //If so, have the sensor read and calculate the results.
    mySensor.readAlgorithmResults();
    co2 = mySensor.getCO2();
    tvoc = mySensor.getTVOC();
    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print(" PPM");
    Serial.print("\t\t");
    Serial.print("TVOC: ");
    Serial.print(tvoc);
    Serial.println(" \PPB");
    Serial.println("--------------------------------------------------------");
  }

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
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // set the fields with the values
  ThingSpeak.setField(1, nh4);
  ThingSpeak.setField(2, co);
  ThingSpeak.setField(3, t);
  ThingSpeak.setField(4, h);
  ThingSpeak.setField(5, co2);
  ThingSpeak.setField(6, tvoc);

  delay(15000);  // Wait 20 seconds to update the channel again
}