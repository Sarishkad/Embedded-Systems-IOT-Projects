#include <ESP8266WiFi.h>

const char *ssid = "ESP8266_AP";
const char *password = "12345678";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  server.begin();
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client Connected");
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.print("Received: ");
        Serial.println(request);
        client.flush();
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
  }
}
