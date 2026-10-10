#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "";
const char* password = "";

#define BOTtoken ""
#define CHAT_ID ""

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

#define sensor D5
#define Status1 D6

void setup() {
  Serial.begin(9600);
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int a = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    a++;
  }

  Serial.print("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  bot.sendMessage(CHAT_ID, "System started", "");
  Serial.print("System started");
  pinMode(sensor, INPUT);
  pinMode(Status1, OUTPUT);
  delay(1000);
}
void loop() {
  bool value = digitalRead(sensor);
  Serial.println(value);
  if (value == 1) {
    digitalWrite(Status1, HIGH);
    Serial.println("Motion detected!");
    bot.sendMessage(CHAT_ID, "Motion detected!!", "");
    delay(1000);
  } else if (value == 0) {
    digitalWrite(Status1, LOW);
    delay(1000);
  }
}
