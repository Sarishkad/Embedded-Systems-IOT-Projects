const int buzzerPin = 7;           // Pin connected to buzzer
const int flameSensorPin = 2;      // Digital pin for flame sensor
const int smokeSensorPin = A0;     // Analog pin for smoke sensor (MQ-2, MQ-135, etc.)

const int smokeThreshold = 300;    // Smoke level threshold (adjust as needed)
const int flameThreshold = LOW;    // Flame detected when pin goes LOW

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(flameSensorPin, INPUT);
  Serial.begin(9600);              // Initialize serial communication
}

void loop() {
  // Read the flame sensor status (LOW = flame detected)
  int flameDetected = digitalRead(flameSensorPin);

  // Read the smoke sensor value (analog: 0-1023)
  int smokeLevel = analogRead(smokeSensorPin);

  // --- SMOKE DETECTION: Immediate buzzer ring ---
  if (smokeLevel > smokeThreshold) {
    Serial.println("SENSOR_SMOKE");
    digitalWrite(buzzerPin, HIGH);  // Turn on buzzer for smoke
    delay(1000);                    // Keep buzzer on for 1 second
    digitalWrite(buzzerPin, LOW);   // Turn off buzzer
    delay(1000);                    // Wait for 1 second before checking again
  }

  // --- FLAME DETECTION: Send status to Python ---
  if (flameDetected == flameThreshold) {  // LOW = flame detected
    Serial.println("SENSOR_FIRE");
  }

  // --- Listen for Python's command to trigger buzzer (for FIRE only) ---
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    if (command == "FIRE") {
      digitalWrite(buzzerPin, HIGH);  // Turn on buzzer for fire
      delay(1000);                    // Keep buzzer on for 1 second
      digitalWrite(buzzerPin, LOW);   // Turn off buzzer
    } else if (command == "CLEAR") {
      digitalWrite(buzzerPin, LOW);   // Turn off buzzer if fire is cleared
    }
  }

  delay(100);  // Avoid spamming serial and CPU
}
