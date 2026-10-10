#include "NewPing.h"
#define TRIGGER_PIN 10
#define ECHO_PIN 11

#define MAX_DISTANCE 400

char userInput;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 1) {
    userInput = Serial.read();

    if (userInput == 'g') {
      Serial.print("Distance = ");
      Serial.print(sonar.ping_cm());
      Serial.println(" cm");
    }
  }
}