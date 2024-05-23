#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>

const char* zapierWebhookURL = "ZAPIER_WEBHOOK_URL ";
const int sensorPin;
ESP8266WebServer server(80);
Ticker timer;
bool sensorActive = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(getenv("WIFI_SSID"), getenv("WIFI_PASSWORD"));

  String sensorPinStr = getenv("SENSOR_PIN");
  sensorPin = sensorPinStr.toInt();

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
        sendZapierEvent();
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


void sendZapierEvent() {
  HTTPClient http;
  
  http.begin(getenv("ZAPIER_WEBHOOK_URL"));
  http.addHeader("Content-Type", "application/json");
  
  String timestamp = String(systime());
  String payload = "{\"value1\":\"Alert! Motion detected\", \"timestamp\":\"" + timestamp + "\"}";
  
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    Serial.print("Zapier request sent, response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error sending IFTTT request, error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
