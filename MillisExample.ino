/*
Author:  Frederik
*/


#include <FirebaseArduino.h>
#include <Firebase.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST"pdp-test-47ed0.firebaseio.com"
#define FIREBASE_AUTH"dtNxJuf4xsr78sdk1sjli83RcF3ggVvJCYiFxp6o"
#define WiFiUser"AU-Gadget"
#define WiFiPass"augadget"

long interval = 1000; //delay for communicating with database
long previousMillis = 0; //variable to hold whenever we passed our latest interval
int maxLightValue; //holds our maxLightValue in the current interval

uint8_t resultLED = D6; //pin with LED showing the result from the database
uint8_t LDR = A0; //pin with LDR sensor which senses the intensitivity of the surrounding light

//the setup function runs once upon start
void setup() {
  Serial.begin(9600);

  //setup LED and LDR-sensor
  pinMode(resultLED, OUTPUT);
  pinMode(LDR, INPUT);

  //connect to WiFi
  WiFi.begin(WiFiUser, WiFiPass);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected");
  Serial.print(WiFi.localIP());

  //begin Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

//the loop function runs over and over again until power down or reset
void loop() {

  //read light intensitivity from LDR
  int lightValue = analogRead(LDR);

  //if lightValue is the largest yet, update maxLightValue
  if (lightValue > maxLightValue) {
    maxLightValue = lightValue;
  }

  Serial.println(lightValue);

  //go into loop whenever "interval" has gone by
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    //update previousMillis so we can count to next interval
    previousMillis = currentMillis;

    //get value from database, switch LED based on this
    int value = Firebase.getInt("LED");
    switchLED(value);

    //read light intensitivity from LDR and reset maxLightValue
    updateDatabase(maxLightValue);
    maxLightValue = 0;
  }
}

//turns LED on/off depending on brightness
void switchLED(int value) {
  if (value > 400) {
    digitalWrite(resultLED, HIGH);
  }
  else {
    digitalWrite(resultLED, LOW);
  }
}

//updates database with a value
void updateDatabase(int value) {
  Firebase.setInt("LED", value);
  if (Firebase.failed()) { //handle error
    Serial.println("Failed setting value");
    Serial.println(Firebase.error()); //get error info
  }
  else {
    Serial.println("Success setting value");
  }
}
