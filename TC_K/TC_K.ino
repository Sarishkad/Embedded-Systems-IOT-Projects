#include "max6675.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int thermoDO = 4; 
int thermoCS = 5; 
int thermoCLK = 6; 

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

LiquidCrystal_I2C lcd(0x27,  16, 2);

float minTemp = 25.0; 
float maxTemp = 100.0; 
float minCurrent = 4.0; 
float maxCurrent = 20.0; 

void setup() {
  Serial.begin(9600);
  delay(500); 
//   Serial.println("Thermocouple ready.");

  lcd.init();
  lcd.backlight();
}

void loop() {

  double tempC = thermocouple.readCelsius();

  if (tempC < minTemp) {
    tempC = minTemp; 
  } else if (tempC > maxTemp) {
    tempC = maxTemp; 
  }

  float current_mA = convertTempToCurrent(tempC);

  simulateCurrentOutput(current_mA);

  // Serial.print("Temperature: ");
  Serial.println(tempC);
   Serial.print("Current: ");
   Serial.print(current_mA);
   Serial.println(" mA");

  lcd.setCursor(0,0);
  lcd.print("Temperature:");
  lcd.print(tempC);
  lcd.setCursor(0,1);
  lcd.print("Current:");
  lcd.print(current_mA);

  delay(1000);
}

float convertTempToCurrent(float tempC) {

  float slope = (maxCurrent - minCurrent) / (maxTemp - minTemp);
  float intercept = minCurrent - slope * minTemp;
  

  float current_mA = slope * tempC + intercept;
  return current_mA;
}


void simulateCurrentOutput(float current_mA) {

  int pwmValue = map(current_mA * 1000, minCurrent * 1000, maxCurrent * 1000, 0, 255); // Multiplied by 1000 to maintain precision
  
  // analogWrite(pwmPin, pwmValue);
}
