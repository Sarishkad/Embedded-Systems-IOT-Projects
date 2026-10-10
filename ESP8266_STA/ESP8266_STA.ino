#include <ESP8266WiFi.h>

const char *ssid = "ESP8266_AP";
const char *password = "12345678";
const char *host = "192.168.4.1";  // IP of the AP

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to AP with IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("Connection to server failed");
    delay(1000);
    return;
  }

  // Send data to the server
  client.print("Hello from Client");
  delay(1000);  // Adjust the delay as needed
}
