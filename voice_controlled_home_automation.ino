#include<ESP8266WiFi.h>
char *ssid = "Lakshay";
char *password = "Sunnybh@ti@1996";
String host = "maker.ifttt.com";
String url = "/trigger/pir/with/key/cQ35HjVZ2EMwzOm3um0XwHwm7q6ZHB-mx-bAfydm7o6";//pir

//temp sensor
#define ThermistorPin A0
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
const char auth[]="e393702ca56342259e63bc9b76c4639f";
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
String apiWriteKey="WVSOG37O3RANR7NI";
const char* server="api.thingspeak.com";
//float resolution ="3.3/1023";



//pir snsor
#define pirPin D7
int calibrationTime = 30;
long unsigned int lowIn;
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int PIRValue = 0;
WiFiClient client;  





void setup() {
   Serial.begin(115200);
   pinMode(pirPin, INPUT);
   //wifi
   Serial.print("Connecting to ");
Serial.println(ssid);
/* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default, would try to act as both a client and an access-point and could cause network-issues with your other WiFi-devices on your WiFi-network. */
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected"); Serial.println("IP address: "); 
Serial.println(WiFi.localIP()); 
//blynk
Blynk.begin(auth, ssid, password);




}

// We now create a URI for the request
//this url contains the informtation we want to send to the server
//if esp8266 only requests the website, the url is empty

void loop() {
   Blynk.run();
   PIRSensor();

//temp sensor::::
Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature: "); 
  Serial.print(T);
  Serial.println(" F"); 
  if (client.connect(server,80))
  {  
    String tsData = apiWriteKey;
           tsData +="&field1=";
           tsData += String(T);
           tsData += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+String(apiWriteKey)+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(tsData.length());
     client.print("\n\n");  // the 2 carriage returns indicate closing of Header fields & starting of data
     client.print(tsData);
 
     Serial.print("Temperature: ");
     Serial.print(T);
     Serial.println("uploaded to Thingspeak server....");
  }
  client.stop();
 
  Serial.println("Waiting to upload next reading...");
  Serial.println();
  // thingspeak needs minimum 1.5 sec delay between updates
  delay(1500);

   
}







void PIRSensor() {
  
WiFiClient client;
const int httpPort = 80;
if (!client.connect(host, httpPort)) {
Serial.println("connection failed");
return;}
   if(digitalRead(pirPin) == HIGH) {
      if(lockLow) {
         PIRValue = 1;
         lockLow = false;
         //run trigger
         client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
         
         Serial.println("Motion detected.");
         delay(50);
      }
      takeLowTime = true;
   }
   if(digitalRead(pirPin) == LOW) {
      if(takeLowTime){
         lowIn = millis();takeLowTime = false;
      }
      if(!lockLow && millis() - lowIn > pause) {
         PIRValue = 0;
         lockLow = true;
         Serial.println("Motion ended.");
         delay(50);
      }
   }
}
