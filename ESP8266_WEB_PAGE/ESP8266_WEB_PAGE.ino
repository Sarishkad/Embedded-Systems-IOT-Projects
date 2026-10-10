#include <ESP8266WiFi.h>

WiFiClient client;
WiFiServer server(80);

#define led D5

void setup(){
  Serial.begin(9600);
  WiFi.begin("LAPTOP-HR03BFI2 7263", ",3453fU4");
  while(WiFi.status() != WL_CONNECTED){
    delay(200);
    Serial.print(".");
  } 

  Serial.println();
  Serial.println("Node MCU Started");
  Serial.println(WiFi.localIP());
  server.begin();

  pinMode(led, OUTPUT);
}

void loop(){
  
  client = server.available();
  if(client == 1)
  {
    String request = client.readStringUntil('\n');
    Serial.println(request);
    request.trim();
    if(request == "GET /ledon HTTP/1.1")
    {
      digitalWrite(led, HIGH);
    }
    if(request == "GET /ledoff HTTP/1.1")
    {
      digitalWrite(led, LOW);
    }
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");

  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>NODE_MCU_ESP8266_WEBPAGE</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<br/>");
  client.println("<h1>WELCOME to the NODE_MCU_ESP8266_WEBPAGE</h1>");
  client.println("<a href=\"/ledon\"\"><button>LED ON</button></a>");
  client.println("<a href=\"/ledoff\"\"><button>LED OFF</button></a><br/>");
  client.println("</body>");
  client.println("</html>");
}