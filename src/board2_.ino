#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include "config.h"

#define TOPIC_LIGHT TOPIC_PREFIX "/light"
#define LED_PIN 7
#define NUM_LEDS 16

CRGB leds[NUM_LEDS];
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ---------------- WIFI ----------------
void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

// ---------------- MQTT CALLBACK ----------------
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  int value = atoi((char*)payload);

  if (strcmp(topic, TOPIC_LIGHT) == 0) {
    if (value == 1) {
      Serial.println("LIGHT ON");
      fill_solid(leds, NUM_LEDS, CRGB::White);
    } else {
      Serial.println("LIGHT OFF");
      fill_solid(leds, NUM_LEDS, CRGB::Black);
    }
    FastLED.show();
  }
}

// ---------------- MQTT ----------------
void connect_mqtt() {
  mqtt.setServer(MQTT_BROKER, 1883);
  mqtt.setCallback(mqtt_callback);

  Serial.print("Connecting MQTT");
  while (!mqtt.connect("light_client", MQTT_USER, MQTT_PASS)) {
    Serial.print(".");
    delay(1000);
  }

  mqtt.subscribe(TOPIC_LIGHT);
  Serial.println(" connected");
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();

  connect_wifi();
  connect_mqtt();
}

// ---------------- LOOP ----------------
void loop() {
  if (!mqtt.connected()) connect_mqtt();
  mqtt.loop();
}