#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define sensor and output pins
#define TOUCH_SENSOR_1 D5
#define TOUCH_SENSOR_2 D6
#define TOUCH_SENSOR_3 D7
#define GREEN_LED D3
#define RED_LED D4
#define BUZZER D8  

// WiFi credentials
const char* ssid = "LAPTOP-HR03BFI2 7263";
const char* password = ",3453fU4";

// Web server
WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  // Pin modes
  pinMode(TOUCH_SENSOR_1, INPUT);
  pinMode(TOUCH_SENSOR_2, INPUT);
  pinMode(TOUCH_SENSOR_3, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);  // Buzzer pin

  // I2C setup
  Wire.begin(D2, D1);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();

  server.begin();
}

void loop() {
  // Read sensor values
  bool s1 = digitalRead(TOUCH_SENSOR_1);
  bool s2 = digitalRead(TOUCH_SENSOR_2);
  bool s3 = digitalRead(TOUCH_SENSOR_3);

  // Determine posture
  String postureStatus = "Incorrect Posture";
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  bool isCorrectPosture = (s1 && s2 && s3);
  if (isCorrectPosture) {
    postureStatus = "Correct Posture";
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW); 
  } else {
    // Trigger buzzer for 3 seconds
    digitalWrite(BUZZER, HIGH);
    delay(2000);
    digitalWrite(BUZZER, LOW);
  }

  // Update OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Posture Monitor");
  display.setCursor(0, 16);
  display.setTextSize(1);
  display.println(postureStatus); 
  display.setCursor(0, 32);
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();

  // Serve web page
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) delay(1);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><title>Posture Detection</title>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #f0f2f5; color: #333; text-align: center; padding: 50px; }");
    client.println(".status-box { font-size: 36px; font-weight: bold; padding: 30px; background: white; border-radius: 12px; display: inline-block; box-shadow: 0px 6px 12px rgba(0,0,0,0.3); margin-top: 20px; }");
    client.println(".warning-box { font-size: 24px; color: red; font-weight: bold; padding: 20px; background: #ffe6e6; border-radius: 10px; display: inline-block; box-shadow: 0px 4px 8px rgba(255,0,0,0.2); margin-top: 20px; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>Posture Detection</h1>");
    client.println("<div class='status-box'>" + postureStatus + "</div>");

    // If incorrect posture, show warning
    if (!isCorrectPosture) {
      client.println("<div class='warning-box'>Please sit properly!</div>");
    }

    client.println("</body></html>");
    client.println();
    client.stop();
  }

  delay(500);
}
