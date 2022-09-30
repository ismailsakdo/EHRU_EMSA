//Board selection and libraries
#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

//Thingspeak Account Information
#define SECRET_SSID "USERNAME"    // replace MySSID with your WiFi network name ENSU: ENSU ARUBA // AndroidAP961b
#define SECRET_PASS "PASSWORD"  // replace MyPassword with your WiFi password ENSU: 601112749996 //abc43210

#define SECRET_CH_ID 000000    // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "THINGSPEAKAPI"   // replace XYZ with your channel write API Key

#include<Wire.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

//Insert Library (if any)
#include "ClosedCube_HDC1080.h"

//Create instances for using library
ClosedCube_HDC1080 hdc1080;

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
String myStatus = "";

void setup() {
  Serial.begin(115200);  // Initialize serial
  Wire.begin();
  hdc1080.begin(0x40);

  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX); // 0x5449 ID of Texas Instruments
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); // 0x1050 ID of the device
 

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // change the values
  float nt = hdc1080.readTemperature();
  float nh = hdc1080.readHumidity();
  if (isnan(nt) || isnan(nh))

  {
    Serial.println("Failed to read from HTU21 sensor!");
    return;
  }

  //Calibration (if needed) - adjusted data based on superficial ideas
  //float at = nt + 0.23;
  //float ah = nh - 10;

  //final calibration based on algorithm
  float t = (((1.0081*nt) - (0.3202))+1.6); //Rsquare = 0.9999999125 
  float h = (((1.0305*nh) - (0.7127))-16.54); // Rqsuare = 0.9996

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

  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

  // set the status
  ThingSpeak.setStatus(myStatus);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celsius, Humidity (%): ");
  Serial.print(h);
  Serial.println("Sending data to Thingspeak");

  delay(15000); // Wait 15 seconds
}
