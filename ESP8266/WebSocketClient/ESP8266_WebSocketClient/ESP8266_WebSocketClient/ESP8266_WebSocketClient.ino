/*
 *Netmedias
 *
 *  Created on: 24.05.2015
 *  
 */
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>


// @@@@@@@@@@@@@@@ You only need to midify modify wi-fi and domain info @@@@@@@@@@@@@@@@@@@@
const char* ssid     = "enter your ssid"; //enter your ssid/ wi-fi(case sensitiv) router name - 2.4 Ghz only
const char* password = "enter ssid password";     // enter ssid password (case sensitiv)
char host[] = "alexaskillsiot.herokuapp.com"; //enter your Heroku domain name like "espiot.herokuapp.com" 
int sensor_distance_from_door = 5;
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//Firstly the connections of ultrasonic Sensor.Connect +5v and GND normally and trigger pin to 5 & echo pin to 4. 

#define trigPin 5
#define echoPin 4
int duration, distance;

int port = 80;
char path[] = "/ws"; 
ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
const int relayPin = 16;
DynamicJsonBuffer jsonBuffer;
String currState, oldState, message;
String jsonResponse = "{\"version\": \"1.0\",\"sessionAttributes\": {},\"response\": {\"outputSpeech\": {\"type\": \"PlainText\",\"text\": \"<text>\"},\"shouldEndSession\": false}}";
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) { //uint8_t *


    switch(type) {
        case WStype_DISCONNECTED:
            //USE_SERIAL.printf("[WSc] Disconnected!\n");
           Serial.println("Disconnected! ");
            break;
            
        case WStype_CONNECTED:
            {
             Serial.println("Connected! ");
			    // send message to server when Connected
				    webSocket.sendTXT("Connected");
            }
            break;
            
        case WStype_TEXT:
            Serial.println("Got data");
              //data = (char*)payload;
           processWebScoketRequest((char*)payload);
            break;
            
        case WStype_BIN:

            hexdump(payload, length);
            Serial.print("Got bin");
            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

void setup() {
    Serial.begin(115200);

    pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT);

    
    Serial.setDebugOutput(true);
    
    pinMode(relayPin, OUTPUT);
    
      for(uint8_t t = 4; t > 0; t--) {
          delay(1000);
      }
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    
    //Serial.println(ssid);
    WiFiMulti.addAP(ssid, password);

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected to wi-fi");
    webSocket.begin(host, port, path);
    webSocket.onEvent(webSocketEvent);

}

void loop() {
    //Serial.println("looping...");
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    
    if (distance <= sensor_distance_from_door ){
      //Serial.print(distance);
      currState = "open";
      //Serial.println(currState);
    }else{
      //Serial.println(currState);
      currState = "close";
    }

    
    webSocket.loop();
    delay(1000);
}


void processWebScoketRequest(String data){

            JsonObject& req = jsonBuffer.parseObject(data);
            String buff = req["request"]["intent"]["slots"];
            JsonObject& slots = jsonBuffer.parseObject(buff);

            String instance = (const char*)slots["instance"]["value"];
            String state = (const char*)slots["state"]["value"];
            String question = (const char*)slots["question"]["value"];
            String message;
            Serial.println(data);
            Serial.println(state);
            
            if(state == "open" || state == "close"){ //if query check state
              Serial.println("Recieved command!");
                   if(state != currState){
                         if(currState == "close"){
                            message = "opening";
                          }else{
                            message = "closing";
                          }
                          digitalWrite(relayPin, HIGH);
                          delay(1000);
                          digitalWrite(relayPin, LOW);
                   }else{
                          if(currState == "close"){
                            message = "already closed";
                          }else{
                            message = "already open";
                          }
                    }
                  digitalWrite(relayPin, HIGH);
                  delay(5);
                  digitalWrite(relayPin, LOW);
                  jsonResponse.replace("<text>", "Garge door " + instance + " is " + message );

            }else if(question == "is" || question == "what"){ //if command then execute
              Serial.println("Recieved query!");
                 if(currState=="open"){
                      message = "open";
                    }else{
                      message = "closed";
                    }
                   jsonResponse.replace("<text>", "Garge door " + instance + " is " + message );
            }else{//can not recognized the command
                    Serial.println("Command is not recognized!");
                   jsonResponse.replace("<text>", "Command is not recognized");
            }
            Serial.print("Sending response back");
            Serial.println(jsonResponse);
                  // send message to server
                  webSocket.sendTXT(jsonResponse);
                  if(state == "open" || state == "close"){webSocket.sendTXT(jsonResponse);}
}



