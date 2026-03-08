#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"

// MQTT Topics
#define TOPIC_LIGHT  TOPIC_PREFIX "/light"
#define TOPIC_HRV    TOPIC_PREFIX "/hrv"
#define TOPIC_BPM    TOPIC_PREFIX "/bpm"
#define TOPIC_STRESS TOPIC_PREFIX "/stress"

WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);

MAX30105 particleSensor;

const int fingerThreshold = 50000;

// HRV buffer
const int MAX_BEATS = 100;
// const int MAX_BEATS = 30;
int ibiBuffer[MAX_BEATS];
int beatCount = 0;

unsigned long lastBeat = 0;

// ---------- WiFi ----------
void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi connected");
}

// ---------- MQTT ----------
void mqtt_callback(char* topic, byte* payload, unsigned int length) {}

void connect_mqtt() {

  Serial.println("Connecting MQTT");

  while (!mqtt.connect("esp32_hrv", MQTT_USER, MQTT_PASS)) {
    Serial.println("MQTT failed... retry");
    delay(2000);
  }

  mqtt.setCallback(mqtt_callback);
  mqtt.subscribe(TOPIC_LIGHT);
  mqtt.subscribe(TOPIC_HRV);
  mqtt.subscribe(TOPIC_BPM);
  mqtt.subscribe(TOPIC_STRESS);

  Serial.println("MQTT connected");
}

// ---------- HRV ----------
float computeHRV() {

  if (beatCount < 3) return 0;

  float sum = 0;

  for (int i = 1; i < beatCount; i++) {
    float diff = ibiBuffer[i] - ibiBuffer[i-1];
    sum += diff * diff;
  }

  float rmssd = sqrt(sum / (beatCount - 1));
  return rmssd;
}

// ---------- BPM ----------
float computeBPM() {

  if (beatCount < 2) return 0;

  float sum = 0;

  for (int i = 0; i < beatCount; i++) {
    sum += ibiBuffer[i];
  }

  float avgIBI = sum / beatCount;

  return 60000.0 / avgIBI;
}

// ---------- Stress ----------
int computeStress(float hrv) {

  if (hrv > 50) return 0;       // Relax
  else if (hrv > 30) return 1;  // Normal
  else if (hrv > 15) return 2;  // Mild stress
  else return 3;                // High stress
}

// ---------- Publish MQTT ----------
void publishResults(float bpm, float hrv, int stress) {
  String payload = String((int)bpm);
  Serial.printf("Publishing BPM: %d\n", (int)bpm);
  mqtt.publish(TOPIC_BPM, payload.c_str());

  payload = String((int)hrv);
  Serial.printf("Publishing HRV: %d\n", (int)hrv);
  mqtt.publish(TOPIC_HRV, payload.c_str());

  if (stress > 0){
    payload = String((int)hrv);
    Serial.printf("Publishing Light: %d\n", 0);
    mqtt.publish(LIGHT, payload.c_str());
  }

  payload = String(stress);
  Serial.printf("Publishing Stress: %d\n", stress);
  mqtt.publish(TOPIC_STRESS, payload.c_str());
}

// ---------- Collect 10s data ----------
void collectData() {

  beatCount = 0;
  unsigned long startTime = millis();

  while (millis() - startTime < 10000) {

    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue)) {

      unsigned long now = millis();
      int ibi = now - lastBeat;

      lastBeat = now;

      if (ibi > 300 && ibi < 2000) {

        ibiBuffer[beatCount] = ibi;
        beatCount++;

        if (beatCount >= MAX_BEATS)
          break;
      }
    }

    delay(5);
  }
}

// ---------- Setup ----------
void setup() {

  Serial.begin(115200);

  Wire.begin(8, 9);

  Serial.println("Initializing MAX30105");

  if (!particleSensor.begin(Wire)) {

    Serial.println("MAX30105 not found");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeIR(0x1F);

  Serial.println("Place finger on sensor");

  connect_wifi();
  connect_mqtt();
}

// ---------- Loop ----------
void loop() {

  mqtt.loop();

  long ir = particleSensor.getIR();

  if (ir > fingerThreshold) {

    Serial.println("Finger detected");

    collectData();

    float bpm = computeBPM();
    float hrv = computeHRV() / 10;
    int stress = computeStress(hrv);

    Serial.print("BPM: ");
    Serial.println(bpm);

    Serial.print("HRV: ");
    Serial.println(hrv);

    Serial.print("Stress: ");
    Serial.println(stress);

    publishResults(bpm, hrv, stress);

    delay(1000);
  }

  else {

    Serial.println("Place finger on sensor");

    delay(1000);
  }
}