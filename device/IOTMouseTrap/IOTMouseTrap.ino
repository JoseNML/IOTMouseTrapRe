#include <FastLED.h> 
#include <FirebaseArduino.h>  
#include <ArduinoJson.h> 
#include <ESP8266WebServer.h>  
#include <WiFiManager.h>   
#include <Button.h> 
#include <VL53L0X.h>
#include <Wire.h>  
#include <Servo.h> 

#define FIREBASE_HOST "algebraic-rider-254615.firebaseio.com"
#define FIREBASE_AUTH "IksDsu0pvUI0nzLEtN2OpR7Hbr2yPaVQNvUiRgj3"
#define BUTTON_PIN D5
#define LED_PIN D6
#define NUM_LEDS 3 
#define SERVO_PIN D7

const String deviceId = "TRP-001";
const int CLOSE_TRAPDOOR=  0;
const int OPEN_TRAPDOOR=  180;
const int MAX_DISTANCE = 100;
boolean state = false;
boolean isCaught = false;
 
WiFiManager wifiManager; 
Button button(BUTTON_PIN);
VL53L0X sensor; 
CRGB leds[NUM_LEDS]; 
Servo trapdoor; 

void setup() {
 Serial.begin(9600);
  button.begin();  
  Wire.begin();
  FastLED.addLeds<WS2811, LED_PIN>(leds, NUM_LEDS); 
  
  trapdoor.write(CLOSE_TRAPDOOR); 
  trapdoor.attach(SERVO_PIN);
  setupWifi();
  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Connected to firebase"); 
  
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous();
  Serial.println("ToF Sensor Online..");
}

void loop() {
  if(!isCaught){
  for(int i = 0; i < NUM_LEDS; i++){
     leds[i] = CRGB::Black;
    FastLED.show();
  }
  }
  if(button.pressed()){   
    Serial.println("Trap is set manually");
    setTrap();  
  } 
  
  boolean isSetRemotely = Firebase.getBool(deviceId + "/state");
  if(isSetRemotely && state != isSetRemotely){ 
    Serial.println("Trap is set remotely");
     setTrap();  
  }
}
 

void setTrap(){ 
    trapdoor.write(OPEN_TRAPDOOR);
    leds[2] = CRGB::Green;
    FastLED.show(); 
    state = true;
    isCaught = false;
    Firebase.setBool(deviceId + "/isCaught", isCaught); 
    Firebase.setBool(deviceId + "/state", state);
    Serial.println("Trap is on, waiting for movement");
    
   while(state){     
      int distance = sensor.readRangeContinuousMillimeters();
      Serial.print("distance = " );
      Serial.print(distance );
      Serial.println(" mm" );
      
      if(distance < MAX_DISTANCE ){ 
        
    trapdoor.write(CLOSE_TRAPDOOR);
        isCaught = true; 
        state = false; 
        Serial.println("CAUGHT SOMETHING!!");  
        leds[2] = CRGB::Red;
        FastLED.show(); 
      } 
      
        if(button.pressed()){
        state = false;  
        Serial.println("Trap is turned off manually"); 
        trapdoor.write(CLOSE_TRAPDOOR);
      }  
      
      if(! (Firebase.getBool(deviceId + "/state")) ){
         state = false;  
        Serial.println("Trap is turned off remotely"); 
        
    trapdoor.write(CLOSE_TRAPDOOR);
      } 
        Firebase.setBool(deviceId + "/state", state);
        Firebase.setBool(deviceId + "/isCaught", isCaught); 
       yield();
    }
}

void setupWifi(){ 
    leds[2] = CRGB::Blue;
    FastLED.show();
 wifiManager.startConfigPortal("IOT MouseTrap"); 
     leds[2] = CRGB::Black;
     FastLED.show();
}
 void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
