const int PIR_SENSOR_PIN = 7;  // PIR sensor output pin (changed to pin 7)
const int BUZZER_PIN = 8;      // Buzzer connected to pin 8

void setup() {
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("PIR Sensor Initialized. Waiting for motion...");
}

void loop() {
  if (digitalRead(PIR_SENSOR_PIN) == HIGH) {
    Serial.println("1");               // Send '1' when motion is detected
    digitalWrite(BUZZER_PIN, HIGH);     // Turn on buzzer
    delay(500);                         // Buzzer on for 500 ms
    digitalWrite(BUZZER_PIN, LOW);      // Turn off buzzer
  }
}
