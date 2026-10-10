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

// Wi-Fi Credentials
const char* ssid = "LAPTOP-HR03BFI2 7263";
const char* password = ",3453fU4";

// Define DHT22 pin and type
#define DHTPIN D6
#define DHTTYPE DHT22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Initialize sensors
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
Adafruit_BMP280 bmp; // BMP280 for pressure
DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80); // Web server on port 80

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Connect to Wi-Fi
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

  // Initialize MLX90614
  if (!mlx.begin()) {
    Serial.println("Could not find MLX90614 sensor!");
    while (1);
  }

  // Initialize TSL2561
  if (!tsl.begin()) {
    Serial.println("Could not find TSL2561 sensor!");
    while (1);
  }
  tsl.enableAutoRange(true);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);

  // Initialize BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor!");
    while (1);
  }

  // Initialize DHT22
  dht.begin();
  
  Serial.println("Sensors initialized!");
}

void loop() {
    // Wait for a client to connect
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while (!client.available()) {
    delay(1);
  }

  // Sensor Readings
  double ambientTemp = mlx.readAmbientTempC();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;
  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = event.light;

  // AET Calculation
  float slopeSaturationVaporPressure = 0.6108 * exp((17.27 * temperature) / (temperature + 237.3));
  float psychrometricConstant = 0.066;
  float solarRadiation = lux * 0.0079;
  float netRadiation = solarRadiation * (1 - 0.23) * 0.0864;
  float AET = (0.408 * slopeSaturationVaporPressure * netRadiation + psychrometricConstant * (900 / (temperature + 273)) * humidity) /
              (slopeSaturationVaporPressure + psychrometricConstant * (1 + 0.34));

  // HTTP Response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  // HTML Page with Enhanced Card View
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP8266 Sensor Data</title>");
  client.println("<style>");
  
  // CSS Styling
  client.println("body { font-family: Arial, sans-serif; background-color: #f0f2f5; color: #333333; margin: 0; padding: 0; display: flex; flex-direction: column; align-items: center; }");
  client.println("h1 { color: #1a73e8; margin-top: 20px; font-size: 32px; }");
  
  client.println(".card-container { display: flex; flex-wrap: wrap; justify-content: center; max-width: 1200px; }");
  client.println(".card { background: #ffffff; border-radius: 15px; box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15); margin: 15px; padding: 25px; width: 300px; text-align: center; transition: transform 0.2s; }");
  client.println(".card:hover { transform: scale(1.05); }");
  
  client.println(".card h2 { font-size: 22px; color: #333333; margin-bottom: 10px; }");
  client.println(".card p { font-size: 20px; color: #666666; margin: 10px 0; }");

  // Responsive styling for 3-card rows
  client.println("@media (min-width: 768px) { .card-container { display: grid; grid-template-columns: repeat(3, 1fr); gap: 20px; } }");

  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  
  // Header
  client.println("<h1>ESP8266 Sensor Data</h1>");
  
  // Card Container
  client.println("<div class='card-container'>");
  
  // Individual Cards for Sensor Data
  client.println("<div class='card'><h2>Ambient Temp (MLX90614)</h2><p>");
  client.print(ambientTemp); client.println(" °C</p></div>");

  client.println("<div class='card'><h2>Temperature (DHT22)</h2><p>");
  client.print(temperature); client.println(" °C</p></div>");

  client.println("<div class='card'><h2>Humidity (DHT22)</h2><p>");
  client.print(humidity); client.println(" %</p></div>");

  client.println("<div class='card'><h2>Pressure (BMP280)</h2><p>");
  client.print(pressure); client.println(" hPa</p></div>");

  client.println("<div class='card'><h2>Light Level (TSL2561)</h2><p>");
  client.print(lux); client.println(" lux</p></div>");

  client.println("<div class='card'><h2>Estimated AET</h2><p>");
  client.print(AET); client.println(" mm/day</p></div>");

  // Close Card Container
  client.println("</div>");
  
  client.println("</body></html>");
  
  client.println();
  client.stop();

  
  // Display IP and sensor readings on OLED
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
