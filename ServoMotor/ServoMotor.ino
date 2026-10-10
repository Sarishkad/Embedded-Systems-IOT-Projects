#include <Servo.h>

int servoPin = 3;
Servo servo;
int angle = 0;  // servo position in degrees

void setup() {
  servo.attach(servoPin);
}

void loop() {

  // scan from 0 
  for (angle = 0; angle < 180; angle++) {
    servo.write(angle);
    delay(15);
  }

  // now scan back
  for (angle = 180; angle > 0; angle--) {
    servo.write(angle);
    delay(15);
  }
}