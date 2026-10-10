#include <WiFi.h>

const char WiFiPassword[] = ""; // Leave blank if you do not want to have a password for the Node
const char AP_NameChar[] = "JAndro";

WiFiServer server(80);
String request = "";
const int LED_Pin = 15;

void setup() {
    Serial.begin(9600);
    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, LOW); // Turn on LED
    delay(400);
    digitalWrite(LED_Pin, HIGH); // Turn off LED

    WiFi.softAP(AP_NameChar, WiFiPassword); // Create the access point
    server.begin();
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    // Read the first line of the request
    request = client.readStringUntil('\r');
    client.flush();

    // Control the LED based on the request
    if (request.indexOf("LEDON") > 0) {
        digitalWrite(LED_Pin, LOW);
        client.println("HTTP/1.1 200 OK\r\n");
        client.println("LEDON");
        client.flush();
        Serial.println("LED turned on");
    } else if (request.indexOf("LEDOFF") > 0) {
        digitalWrite(LED_Pin, HIGH);
        client.println("HTTP/1.1 200 OK\r\n");
        client.println("LEDOFF");
        client.flush();
        Serial.println("LED turned off");
    }
    delay(5);
}
