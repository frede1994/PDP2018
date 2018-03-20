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
	//get boolean from database, switch LED based on this
	bool on = Firebase.getBool("LED");
	if (on) {
		digitalWrite(resultLED, HIGH);
	}
	else {
		digitalWrite(resultLED, LOW);
	}

	//read light intensitivity from LDR
	int lightValue = analogRead(LDR);
	Serial.println(lightValue);
	//put lightValue into database
	updateDatabase(lightValue);

	//delay to reduce calls over the network
	delay(1000);
}

//updates database with a value
void updateDatabase(int value) {
	Firebase.setInt("LDR-sensor", value);
	if (Firebase.failed()) { //handle error
		Serial.println("Failed setting value");
		Serial.println(Firebase.error()); //get error info
	}
	else {
		Serial.println("Success setting value");
	}
}
