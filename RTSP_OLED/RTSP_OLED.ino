#include "WiFi.h"
#include "StreamIO.h"
#include "VideoStream.h"
#include "RTSP.h"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CHANNEL 0

VideoSetting config(CHANNEL);
RTSP rtsp;
StreamIO videoStreamer(1, 1);  // 1 Input Video -> 1 Output RTSP

char ssid[] = "Sarish's iPhone";
char pass[] = "12345678";
int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(115200);

  // Initialize OLED using default I2C (PA_26 SDA, PA_25 SCL on AMB82-MINI)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while (true)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // Connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  // Camera config
  Camera.configVideoChannel(CHANNEL, config);
  Camera.videoInit();

  // RTSP config
  rtsp.configVideo(config);
  rtsp.begin();

  // StreamIO setup
  videoStreamer.registerInput(Camera.getStream(CHANNEL));
  videoStreamer.registerOutput(rtsp);
  if (videoStreamer.begin() != 0) {
    Serial.println("StreamIO link start failed");
  }

  Camera.channelBegin(CHANNEL);
  delay(1000);

  printInfo();  // Print and display IP & port
}

void loop() {
  // Do nothing
}

void printInfo(void) {
  Serial.println("------------------------------");
  Serial.println("- Summary of Streaming -");
  Serial.println("------------------------------");
  Camera.printInfo();

  IPAddress ip = WiFi.localIP();
  int port = rtsp.getPort();

  Serial.println("- RTSP -");
  Serial.print("rtsp://");
  Serial.print(ip);
  Serial.print(":");
  Serial.println(port);

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("RTSP Streaming");
  display.println("IP Address:");
  display.println(ip);
  display.println("Port:");
  display.println(port);
  display.display();
}
