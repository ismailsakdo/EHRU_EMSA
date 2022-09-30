//Store your credentials
#define SECRET_SSID "WIFI_NAME"    // replace MySSID with your WiFi network name
#define SECRET_PASS "WIFI_PASS" // replace MyPassword with your WiFi password

#define SECRET_CH_ID 000000      // replace 0000000 with your channel number
#define SECRET_WRITE_APIKEY "KEYHERE"   // replace XYZ with your channel write API Key
//=========

//Start Coding
#define LEDPIN LED_BUILTIN         //LED brightness (PWM) writing
#define LIGHTSENSORPIN A0 //Ambient light sensor reading 
#include <ESP8266WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
float reading; float temperature; float humidity;
String myStatus = "";

void setup() {
  pinMode(LEDPIN, OUTPUT); 
  pinMode(LIGHTSENSORPIN,  INPUT);  
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  float reading = analogRead(LIGHTSENSORPIN);
  float square_ratio = reading / 1023.0;
  square_ratio = pow(square_ratio, 2.0);
  analogWrite(LEDPIN, 255.0 * square_ratio); 

  temperature = random(20,45);
  humidity = random(40,80);

  Serial.print(reading);
  Serial.print("; ");
  Serial.print(temperature);
  Serial.print("; ");
  Serial.println(humidity);
  delay(500);  

  // Connect or reconnect to WiFi
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

  // set the fields with the values
  ThingSpeak.setField(1, reading);
  ThingSpeak.setField(2, temperature);
  ThingSpeak.setField(3, humidity);
  
  // set the status
  ThingSpeak.setStatus(myStatus);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  Serial.println("Sending data to Thingspeak");
  
  delay(15000); // Wait 20 seconds to update the channel again
}
