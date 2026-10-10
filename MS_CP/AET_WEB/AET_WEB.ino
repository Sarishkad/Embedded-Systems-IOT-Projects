#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

const char* ssid = "LAPTOP-HR03BFI2 7263";
const char* password = ",3453fU4";

#define DHTPIN D5
#define DHTTYPE DHT22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

Adafruit_BMP280 bmp; 
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

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
  display.println("IP Address: ");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);

  server.begin();

  if (!mlx.begin()) {
    Serial.println("Could not find MLX90614 sensor!");
    while (1);
  }

  if (!tsl.begin()) {
    Serial.println("Could not find TSL2561 sensor!");
    while (1);
  }
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor!");
    while (1);
  }

  dht.begin();
  Serial.println("Sensors initialized!");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while (!client.available()) {
    delay(1);
  }

  double ambientTemp = mlx.readAmbientTempC();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;
  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = event.light;

  float slopeSaturationVaporPressure = 0.6108 * exp((17.27 * temperature) / (temperature + 237.3));
  float psychrometricConstant = 0.066;
  float solarRadiation = lux * 0.0079;
  float netRadiation = solarRadiation * (1 - 0.23) * 0.0864;
  float AET = (0.408 * slopeSaturationVaporPressure * netRadiation + psychrometricConstant * (900 / (temperature + 273)) * humidity) /
              (slopeSaturationVaporPressure + psychrometricConstant * (1 + 0.34));

  // Calculate KPIs dynamically
  float waterSupplied = 100.0; // Example: total water supplied to crops (mm/day)
  float PET = AET * 1.2; // Assuming PET is 20% higher than AET for simplicity
  float irrigationEfficiency = (AET / waterSupplied) * 100.0;
  float waterUsageEfficiency = (AET / PET) * 100.0;

  String cropWaterRequirement;
  if (AET < 2.0) {
    cropWaterRequirement = "Low";
  } else if (AET >= 2.0 && AET <= 4.0) {
    cropWaterRequirement = "Balanced";
  } else {
    cropWaterRequirement = "High";
  }

  // Serve Web Page
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>Drone Sensor Data</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #f0f2f5; color: #333333; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }");
  client.println("h1 { color: #1a73e8; margin-top: 20px; font-size: 32px; }");
  client.println(".card-container { display: grid; grid-template-columns: repeat(3, 1fr); gap: 20px; justify-content: center; max-width: 1200px; }");
  client.println(".card { background: #ffffff; border-radius: 15px; box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15); margin: 15px; padding: 25px; text-align: center; transition: transform 0.2s; }");
  client.println(".card:hover { transform: scale(1.05); }");
  client.println(".card h2 { font-size: 22px; color: #333333; margin-bottom: 10px; }");
  client.println(".card p { font-size: 20px; color: #666666; margin: 10px 0; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>Drone Sensor Data</h1>");
  client.println("<div class='card-container'>");

  client.println("<div class='card'><h2>Ambient Temp (MLX90614)</h2><p>" + String(ambientTemp) + " °C</p></div>");
  client.println("<div class='card'><h2>Temperature (DHT22)</h2><p>" + String(temperature) + " °C</p></div>");
  client.println("<div class='card'><h2>Humidity (DHT22)</h2><p>" + String(humidity) + " %</p></div>");
  client.println("<div class='card'><h2>Pressure (BMP280)</h2><p>" + String(pressure) + " hPa</p></div>");
  client.println("<div class='card'><h2>Light Level (TSL2561)</h2><p>" + String(lux) + " lux</p></div>");
  client.println("<div class='card'><h2>Estimated AET</h2><p>" + String(AET) + " mm/day</p></div>");
  client.println("<div class='card'><h2>Irrigation Efficiency</h2><p>" + String(irrigationEfficiency) + " %</p></div>");
  client.println("<div class='card'><h2>Water Usage Efficiency</h2><p>" + String(waterUsageEfficiency) + " %</p></div>");
  client.println("<div class='card'><h2>Crop Water Requirement</h2><p>" + cropWaterRequirement + "</p></div>");

  client.println("</div>");
  client.println("</body></html>");
  client.println();
  client.stop();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("IP: ");
  display.println(WiFi.localIP());

  display.setCursor(0, 15);
  display.print("Amb Temp: ");
  display.print(ambientTemp);
  display.println(" C");

  display.setCursor(0, 25);
  display.print("Temp (DHT22): ");
  display.print(temperature);
  display.println(" C");

  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  display.setCursor(0, 45);
  display.print("Pressure: ");
  display.print(pressure);
  display.println(" hPa");

  display.setCursor(0, 55);
  display.print("Light: ");
  display.print(lux);
  display.println(" lux");

  display.display();

  delay(1000);
}
;;''