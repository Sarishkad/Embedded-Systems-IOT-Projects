#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

// DHT Sensor setup
#define DHTPIN D1 // Define the data pin for DHT sensor
#define DHTTYPE DHT22 // Use DHT11 or DHT22 based on your sensor
DHT dht(DHTPIN, DHTTYPE);

// Replace with your network credentials
const char* ssid = "LAPTOP-HR03BFI2 7263";
const char* password = ",3453fU4";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
  // Read temperature value
  float t = dht.readTemperature();
  
  // Check if the read failed
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return ""; // Return empty to avoid errors on the client side
  } else {
    Serial.println("Temperature: " + String(t) + " °C");
    return String(t); // Return temperature as a string
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);

  // DHT Sensor initialization
  dht.begin();

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());

  // Route for web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  // Route for temperature
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });

  // Start server
  server.begin();
}

void loop() {
  // Empty loop as AsyncWebServer handles everything
}
