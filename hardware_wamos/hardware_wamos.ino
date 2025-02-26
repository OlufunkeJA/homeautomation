// IMPORT ALL REQUIRED LIBRARIES
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <NewPing.h>

#include <PubSubClient.h>

#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <Adafruit_ILI9341.h>

#include <SoftwareSerial.h>

#include <math.h>

#ifndef _WIFI_H 
#include <WiFi.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#ifndef ARDUINO_H
#include <Arduino.h>
#endif 
 
#ifndef ARDUINOJSON_H
#include <ArduinoJson.h>
#endif
   
//**********ENTER IP ADDRESS OF SERVER******************//

#define HOST_IP     "www.yanacreations.com"       // REPLACE WITH IP ADDRESS OF SERVER ( IP ADDRESS OF COMPUTER THE BACKEND IS RUNNING ON) 
#define HOST_PORT   "8080"            // REPLACE WITH SERVER PORT (BACKEND FLASK API PORT)
#define route       "api/update"      // LEAVE UNCHANGED 
#define idNumber    "620162688"       // REPLACE WITH YOUR ID NUMBER 

// WIFI CREDENTIALS
#define SSID        "MonaConnect"      // "REPLACE WITH YOUR WIFI's SSID"   
#define password    ""  // "REPLACE WITH YOUR WiFi's PASSWORD" 

#define stay        100
 
//**********PIN DEFINITIONS******************//

 
#define espRX         10
#define espTX         11
#define espTimeout_ms 300
#define trigPin  12
#define echoPin  13 
 
/* Declare your functions below */
double getRadar();
double getWaterHeight(double radar);
double getReserve(double radar);
double getPer(double waterHeight,double max); 
 

SoftwareSerial esp(espRX, espTX); 

void setup(){

  Serial.begin(115200); 
  // Configure GPIO pins here

  espInit();  
 
}

void loop(){ 
   
  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’

  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  radar = getRadar();
  waterHeight = getWaterHeight(radar);
  max = 77.763;

  JsonDocument doc;
  char message[1100] = {0};
  doc["id"] = "620162688";
  doc["type"] = "ultrasonic";
  doc["radar"] = radar;
  doc["waterheight"] = waterHeight;
  doc["reserve"] = getReserve(radar);
  doc["percentage"] = getPer(waterHeight,max);

  espUpdate(serializeJson(doc,message));

  delay(1000);  
}
 
void espSend(char command[] ){   
    esp.print(command); // send the read character to the esp    
    while(esp.available()){ Serial.println(esp.readString());}    
}

void espUpdate(char mssg[]){ 
    char espCommandString[50] = {0};
    char post[290]            = {0};

    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT); 
    espSend(espCommandString);    //starts the connection to the server
    delay(stay);

    // GET REQUEST 
    // snprintf(post,sizeof(post),"GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);

    // POST REQUEST
    snprintf(post,sizeof(post),"POST /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);
  
    snprintf(espCommandString, sizeof(espCommandString),"AT+CIPSEND=%d\r\n", strlen(post));
    espSend(espCommandString);    //sends post length
    delay(stay);
    Serial.println(post);
    espSend(post);                //sends POST request with the parameters 
    delay(stay);
    espSend("AT+CIPCLOSE\r\n");   //closes server connection
   }

void espInit(){
    char connection[100] = {0};
    esp.begin(115200); 
    Serial.println("Initiallizing");
    esp.println("AT"); 
    delay(1000);
    esp.println("AT+CWMODE=1");
    delay(1000);
    while(esp.available()){ Serial.println(esp.readString());} 

    snprintf(connection, sizeof(connection),"AT+CWJAP=\"%s\",\"%s\"\r\n",SSID,password);
    esp.print(connection);

    delay(3000);  //gives ESP some time to get IP

    if(esp.available()){   Serial.print(esp.readString());}
    
    Serial.println("\nFinish Initializing");    
   
}

//***** Design and implement all util functions below ******
double getRadar(){
  return ((pulseIn(echoPin,HIGH) * 0.343) /2)/2.54; //read value from sensor and convert to inches
}

double getWaterHeight(radar){
  return radar - 94.5;
}

double getReserve(radar){
  gal = 77.763/1000;
  return gal * radar;
}

double getPer(waterHeight,max){
  return (waterHeight/max)*100;
}
