#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi & MQTT Configuration
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor Settings
const int sensorPin = A0;
const int threshold = 400; // Adjust based on your sensitivity needs (0-1023)
unsigned long lastKnockTime = 0;
const int debounceDelay = 300; // Cooldown in ms to prevent multiple triggers per knock

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266KnockSensor")) {
      client.publish("home/knock/status", "online");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int sensorValue = analogRead(sensorPin);

  // Check if vibration exceeds threshold and debounce time has passed
  if (sensorValue > threshold && (millis() - lastKnockTime) > debounceDelay) {
    lastKnockTime = millis();
    
    // Publish MQTT message
    client.publish("home/knock/event", "knock_detected");
    Serial.println("Knock detected and published via MQTT!");
  }
  
  delay(50); // Small delay to stabilize readings
}
