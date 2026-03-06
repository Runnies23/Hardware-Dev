#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"

#define TOPIC_LIGHT  TOPIC_PREFIX "/light"

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
MAX30105 particleSensor;

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;
int beatAvg = 0;
float beatsPerMinute = 0;

bool lastState = false;

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

// ---------------- MQTT ----------------
void connect_mqtt() {
  mqtt.setServer(MQTT_BROKER, 1883);

  Serial.print("Connecting MQTT");
  while (!mqtt.connect("ppg_client", MQTT_USER, MQTT_PASS)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" connected");
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(8, 9);
  Wire.setClock(100000);

  if (!particleSensor.begin(Wire)) {
    Serial.println("MAX30105 not found");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  connect_wifi();
  connect_mqtt();
}

// ---------------- LOOP ----------------
void loop() {
  if (!mqtt.connected()) connect_mqtt();
  mqtt.loop();

  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60.0 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++)
        beatAvg += rates[i];

      beatAvg /= RATE_SIZE;
    }
  }

  Serial.printf("Avg BPM: %d\n", beatAvg);

  // -------- TRIGGER LIGHT --------
  bool currentState = beatAvg > 90;

  if (currentState != lastState) {
    mqtt.publish(TOPIC_LIGHT, currentState ? "1" : "0");
    lastState = currentState;
  }

  delay(200);
}