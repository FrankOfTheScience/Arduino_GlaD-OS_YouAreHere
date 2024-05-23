#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const int sensorPin = 2;
ESP8266WebServer server(80);

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  pinMode(sensorPin, OUTPUT);
  digitalWrite(sensorPin, LOW);

  server.on("/", handleRoot());
  server.on("/activate", activateRoot());
  server.on("/deactivate", deactivateRoot());

  server.begin();
  Serial.println("HTTP Server has started");
}

void loop(){
  server.handleClient();
}

void handleRoot(){
  server.send(200, "text/plain", "Arduino with ESP8266 module is up and running.";
}

void handleActivate(){
  digitalWrite(sensorPin, HIGH);
  server.send(200, "text/plain", "Sensor activated");
}

void handleDeactivate(){
  digitalWrite(sensorPin, LOW);
  server.send(200, "text/plain", "Sensor deactivated");
}
