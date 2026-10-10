#include <IRremote.h>
const int RemotePin = 7;
IRrecv IRrecv(RemotePin);
decode_results results;
int in1 = 3;
int in2 = 5;
int in3 = 6;
int in4 = 9;

void setup() {

  Serial.begin(9600);
  IRrecv.enableIRIn();
  pinModeFast(in1, OUTPUT);
  pinModeFast(in2, OUTPUT);
  pinModeFast(in3, OUTPUT);
  pinModeFast(in4, OUTPUT);
}

void loop() {

  if (IRrecv.decode(&results)) {

    if (results.value == 4294967295) {
      Forward();
    } else if (results.value == 0xFFA857) {
      Backward();
    } else if (results.value == 0xFF22DD) {
      Left();
    } else if (results.value == 0xFFC23D) {
      Right();
    } else if (results.value == 0xFF02FD) {
      Stop();
    }
    Serial.println(results.value, HEX);
    delay(200);
    IRrecv.resume();
  }
}

void Backward() {
  digitalWriteFast(in1, HIGH);
  digitalWriteFast(in2, LOW);
  digitalWriteFast(in3, HIGH);
  digitalWriteFast(in4, LOW);
}

void Forward() {
  digitalWriteFast(in1, LOW);
  digitalWriteFast(in2, HIGH);
  digitalWriteFast(in3, LOW);
  digitalWriteFast(in4, HIGH);
}

void Right() {
  digitalWriteFast(in1, LOW);
  digitalWriteFast(in2, HIGH);
  digitalWriteFast(in3, LOW);
  digitalWriteFast(in4, HIGH);
}

void Left() {
  digitalWriteFast(in1, LOW);
  digitalWriteFast(in2, HIGH);
  digitalWriteFast(in3, LOW);
  digitalWriteFast(in4, HIGH);
}

void Stop() {
  digitalWriteFast(in1, LOW);
  digitalWriteFast(in2, LOW);
  digitalWriteFast(in3, LOW);
  digitalWriteFast(in4, LOW);
}
