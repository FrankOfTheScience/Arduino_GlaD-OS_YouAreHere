#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h> 
#include <TimeLib.h> 
#include <Timezone.h>

const int sensorPin;
ESP8266WebServer server(80);
WiFiUDP udp;
TimeLib t;
bool sensorActive = false;
String tzString;

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const char* ntpServer3 = "europe.pool.ntp.org";

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

  udp.begin(ntpPort);
  setSyncInterval(10000); 

  tzString = getenv("TIMEZONE_STRING");

  if (tzString.isEmpty()) {
    Serial.println("WARNING: Timezone string not found in environment variable!");
    setTZ("UTC");
  } 
  else {
    setTZ(tzString.c_str());
  }
  
  WiFi.hostByName(ntpServer1, ntpUDP); 
  WiFi.hostByName(ntpServer2, ntpUDP); 
  WiFi.hostByName(ntpServer3, ntpUDP);

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
  String message = "Arduino with ESP8266 module is up and running. Current time: " + getCurrentTime();
  server.send(200, "text/plain", message);
}

void handleActivate() {
  digitalWrite(sensorPin, HIGH);
  sensorActive = true;

  String message = "Arduino infrared sensor activated. Current time: " + getCurrentTime();
  server.send(200, "text/plain", message);
}

void handleDeactivate() {
  digitalWrite(sensorPin, LOW);
  sensorActive = false;

  String message = "Arduino infrared sensor deactivated. Current time: " + getCurrentTime();
  server.send(200, "text/plain", message);
}

String getCurrentTime(){
  update();
  String formattedTime = t.strftime("%Y-%m-%d %H:%M:%S"); 
  return formattedTime;
}

void sendZapierEvent() {
  HTTPClient http;
  
  http.begin(getenv("ZAPIER_WEBHOOK_URL"));
  http.addHeader("Content-Type", "application/json");
  
  String payload = "{\"value1\":\"Alert! Motion detected\", \"timestamp\":\"" + getCurrentTime() + "\"}";
  int httpResponseCode = http.POST(payload);
  
  if (httpResponseCode > 0) {
    Serial.print("Zapier request sent at " + getCurrentTime() + ", response code: ");
    Serial.println(httpResponseCode);
  }
  else {
    Serial.print("Error sending Zapier request at " getCurrentTime() + ", error code: ");
    Serial.println(httpResponseCode);
  }
  
  http.end();
}
