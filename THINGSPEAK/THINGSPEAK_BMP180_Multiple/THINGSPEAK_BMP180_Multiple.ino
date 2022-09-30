//Step 1 - Import library
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

//Step 2 - Create instances
Adafruit_BMP085 bmp;

//Step 3 - Create variable
float t;
float p;

//Step 4 - Wifi Connection

//Store your credentials
#define SECRET_SSID "pengajar2"    // replace MySSID with your WiFi network name
#define SECRET_PASS "Ilkkm@Sb" // replace MyPassword with your WiFi password

#define SECRET_CH_ID 1878775      // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "CAM92XQENSMNE3GM"   // replace XYZ with your channel write API Key
//=========

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
String myStatus = "";

void setup() 
{

// Step 1 - Serial begin - utk boleh baca di serial monitor
  Serial.begin(115200);

// Step 2 - Initialize the sensor  
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

// Step 3 - Initialize wifi (begin connection)
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak

}

void loop() 
{

// Step 1 - Baca sensor kita
// Sensor Temp & Humidity
t = bmp.readTemperature();
p = bmp.readPressure();

Serial.print(t);
Serial.print(" ");
Serial.println(p);

// Sensor CO2


// Step 2 - Connect to your WIFI
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

// Step 3 - Send to Thingspeak/ Server
  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, p);
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  Serial.println("Sending data to Thingspeak");
  
  delay(15000); // Wait 20 seconds to update the channel again

}
