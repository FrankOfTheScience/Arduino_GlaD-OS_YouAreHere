#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";
const char* iftttWebhookURL = "IFTTT_WEBHOOK_URL ";
const int sensorPin = 2;
ESP8266WebServer server(80);
Ticker timer;
bool sensorActive = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
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

  if (sensorActive) {
      if (digitalRead(sensorPin) == HIGH){
        sendIFTTTEvent();
        handleDeactivate();
        delay(3600000);
        handleActivate();
      }
}

void handleRoot(){
  server.send(200, "text/plain", "Arduino with ESP8266 module is up and running.";
}

void handleActivate() {
  digitalWrite(sensorPin, HIGH);
  sensorActive = true;
  server.send(200, "text/plain", "Sensor activated");
}

void handleDeactivate() {
  digitalWrite(sensorPin, LOW);
  sensorActive = false;
  server.send(200, "text/plain", "Sensor deactivated");
}


void sendIFTTTEvent() {
  HTTPClient http;
  
  http.begin(iftttWebhookURL);
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST("{\"value1\":\"Alert! Motion detected\"}");
  
  if (httpResponseCode > 0) {
    Serial.print("IFTTT request sent, response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error sending IFTTT request, error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
