#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define sensorPower 7
#define sensorPin A0

LiquidCrystal_I2C lcd(0x27,  16, 2);
SoftwareSerial mySerial(3, 2); //HC-05 Tx & Rx 

void setup() {
	pinMode(sensorPower, OUTPUT);
	digitalWrite(sensorPower, LOW);
	
	Serial.begin(9600);
  mySerial.begin(9600);

  lcd.init();
  lcd.backlight();
}

void loop() {

	Serial.print("Moisture: ");
	Serial.println(readSensor());
	
	delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Moisture: ");
  lcd.print(readSensor());

  Serial.println("Initializing...");
  Serial.println("The device started, now you can pair it with bluetooth!");

  if(Serial.available()) 
  {
    mySerial.write(Serial.read()); //Forward what Serial received to Software Serial Port
  }
  if(mySerial.available()) 
  {
    Serial.write(mySerial.read()); //Forward what Software Serial received to Serial Port
  }
  delay(20);

}

int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// Allow power to settle
	int val = analogRead(sensorPin);	// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// Return analog moisture value
}

