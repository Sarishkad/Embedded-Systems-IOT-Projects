#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define TOUCH_SENSOR_1 D5
#define TOUCH_SENSOR_2 D6
#define TOUCH_SENSOR_3 D7
#define GREEN_LED D3
#define RED_LED D4
#define BUZZER D8

const char* ssid = "LAPTOP-HR03BFI2 7263";
const char* password = ",3453fU4";

WiFiServer server(80);

bool sittingStable = false;
unsigned long sitStart = 0;
unsigned long lastStableCheck = 0;
unsigned long duration = 0;
bool buzzerPlayed = false;

void setup() {
  Serial.begin(9600);

  pinMode(TOUCH_SENSOR_1, INPUT);
  pinMode(TOUCH_SENSOR_2, INPUT);
  pinMode(TOUCH_SENSOR_3, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();

  bool s1 = digitalRead(TOUCH_SENSOR_1);
  bool s2 = digitalRead(TOUCH_SENSOR_2);
  bool s3 = digitalRead(TOUCH_SENSOR_3);
  bool currentlySitting = s1 && s2 && s3;

  static bool lastSittingState = false;
  static unsigned long lastStateChange = 0;

  // Debounce: Wait for 1000ms stability
  if (currentlySitting != lastSittingState) {
    lastStateChange = currentMillis;
    lastSittingState = currentlySitting;
  }

  if (currentlySitting && (currentMillis - lastStateChange > 1000)) {
    if (!sittingStable) {
      sitStart = currentMillis;
      buzzerPlayed = false;
    }
    sittingStable = true;
    duration = (currentMillis - sitStart) / 1000;
  } else if (!currentlySitting && (currentMillis - lastStateChange > 1000)) {
    sittingStable = false;
    duration = 0;
    buzzerPlayed = false;
  }

  // LED & Buzzer
  if (sittingStable) {
    if (duration >= 5) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      if (!buzzerPlayed) {
        digitalWrite(BUZZER, HIGH);
        delay(500);
        digitalWrite(BUZZER, LOW);
        buzzerPlayed = true;
      }
    } else {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
    }
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);
  }

  // OLED Display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Posture Monitor");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.print("Status: ");
  display.println(sittingStable ? "Sitting" : "Not Sitting");
  display.print("Time: ");
  display.print(duration);
  display.println(" sec");
  display.display();

  // Web Server
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) delay(1);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE html><html>");
    client.println("<head><title>Posture Monitor</title><style>");
    client.println("body { font-family: Arial; text-align: center; background-color: #f0f0f0; padding: 20px; }");
    client.println(".box { font-size: 36px; background: #fff; padding: 20px; border-radius: 10px; display: inline-block; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }");
    client.println("</style></head><body>");
    client.println("<h1>Posture Detection</h1>");
    client.println("<div class='box'>Status: " + String(sittingStable ? "Sitting" : "Not Sitting") + "</div>");
    client.println("</body></html>");
    client.println();
    client.stop();
  }
}
