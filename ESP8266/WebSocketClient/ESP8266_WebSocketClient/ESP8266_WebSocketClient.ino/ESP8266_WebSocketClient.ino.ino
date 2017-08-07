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
char host[] = "alexaskills.herokuapp.com"; //enter your Heroku domain name like "espiot.herokuapp.com" 
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

int port = 80;
char path[] = "/ws"; 
ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
const int relayPin = 16;
DynamicJsonBuffer jsonBuffer;
String currState;
String jsonResponse = "{\"version\": \"1.0\",\"sessionAttributes\": {},\"response\": {\"outputSpeech\": {\"type\": \"<type>\",\"text\": \"<text>\"},\"shouldEndSession\": false}}";
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
        delay(100);
    }
    Serial.println("Connected to wi-fi");
    webSocket.begin(host, port, path);
    webSocket.onEvent(webSocketEvent);

}

void loop() {
    webSocket.loop();
}

void processWebScoketRequest(String data){

            JsonObject& req = jsonBuffer.parseObject(data);
            JsonObject& slots = req["request"]["intent"]["slots"];
            //String device = (const char*)root["device"];
            //String location = (const char*)root["location"];
            
            String state = (const char*)slots["state"];
            String question = (const char*)slots["question"];
            String message;

            Serial.println(data);
            if(state == "open" || state == "close"){ //if query check state
              Serial.println("Recieved command!");

              // Ultrasonic Sensor input to physically check the garage door state  - for future  implementation
                    //if(check ultrasonic distance){
                      //currState = "close";
                      
                   //}esle{
                      //currState = "open";
                   //}

                  digitalWrite(relayPin, HIGH);
                  delay(5);
                  digitalWrite(relayPin, LOW);
                  jsonResponse.replace("<type>", "Garge door button is toggled");
                  jsonResponse.replace("<text>", "Garge door button is toggled");
                  
            }else if(question == "is" || question == "what"){ //if command then execute   
              Serial.println("Recieved query!");
              //int state = check ultrasonic distance
              // Ultrasonic Sensor input to physically check the garage door state  - for future  implementation
                    //if(check ultrasonic distance){
                      //currState = "close";
                      
                   //}esle{
                      //currState = "open";
                   //}
                   jsonResponse.replace("<type>", "Garge door is query is not implemented");
                   jsonResponse.replace("<text>", "Garge door is query is not implemented");
            }else{//can not recognized the command
                    Serial.println("Command is not recognized!");
                   jsonResponse.replace("<type>", "Command is not recognized");
                   jsonResponse.replace("<text>", "Command is not recognized");
            }
            Serial.print("Sending response back");
            Serial.println(message);
                  // send message to server
                  webSocket.sendTXT(jsonResponse);
                  webSocket.sendTXT(jsonResponse);
}

