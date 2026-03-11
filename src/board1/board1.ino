#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"

// ---------- MQTT Topics ----------
#define TOPIC_LIGHT  TOPIC_PREFIX "/light"
#define TOPIC_HRV    TOPIC_PREFIX "/hrv"
#define TOPIC_BPM    TOPIC_PREFIX "/bpm"
#define TOPIC_STRESS TOPIC_PREFIX "/stress"

// ---------- Objects ----------
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
MAX30105 particleSensor;

// ---------- Finger detection ----------
const int fingerThreshold = 50000;

// ---------- HRV buffer ----------
const int MAX_BEATS = 100;
int ibiBuffer[MAX_BEATS];
int beatCount = 0;

unsigned long lastBeat = 0;

// ---------- Light control ----------
bool lightActive = false;
unsigned long lightStart = 0;

// --------------------------------------------------
// WiFi
// --------------------------------------------------

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

// --------------------------------------------------
// MQTT
// --------------------------------------------------

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
}

void connect_mqtt() {

  mqtt.setServer(MQTT_BROKER, 1883);

  Serial.println("Connecting MQTT...");

  while (!mqtt.connect("esp32_hrv", MQTT_USER, MQTT_PASS)) {

    Serial.println("MQTT failed, retrying...");
    delay(2000);
  }

  mqtt.setCallback(mqtt_callback);

  mqtt.subscribe(TOPIC_LIGHT);
  mqtt.subscribe(TOPIC_HRV);
  mqtt.subscribe(TOPIC_BPM);
  mqtt.subscribe(TOPIC_STRESS);

  Serial.println("MQTT connected");
}

// --------------------------------------------------
// HRV Calculation (RMSSD)
// --------------------------------------------------

float computeHRV() {

  if (beatCount < 3) return 0;

  float sum = 0;

  for (int i = 1; i < beatCount; i++) {

    float diff = ibiBuffer[i] - ibiBuffer[i - 1];
    sum += diff * diff;
  }

  float rmssd = sqrt(sum / (beatCount - 1));

  return rmssd;
}

// --------------------------------------------------
// BPM Calculation
// --------------------------------------------------

float computeBPM() {

  if (beatCount < 2) return 0;

  float sum = 0;

  for (int i = 1; i < beatCount; i++) {
    sum += ibiBuffer[i];
  }

  float avgIBI = sum / (beatCount - 1);

  return 60000.0 / avgIBI;
}

// --------------------------------------------------
// Stress Level
// --------------------------------------------------

int computeStress(float hrv) {

  if (hrv > 50) return 0;       // Relax
  else if (hrv > 30) return 1;  // Normal
  else if (hrv > 15) return 2;  // Mild stress
  else return 3;                // High stress
}

// --------------------------------------------------
// Publish MQTT
// --------------------------------------------------

void publishResults(float bpm, float hrv, int stress) {

  String payload;

  payload = String((int)bpm);
  Serial.printf("Publishing BPM: %d\n", (int)bpm);
  mqtt.publish(TOPIC_BPM, payload.c_str());

  payload = String((int)hrv);
  Serial.printf("Publishing HRV: %d\n", (int)hrv);
  mqtt.publish(TOPIC_HRV, payload.c_str());

  payload = String(stress);
  Serial.printf("Publishing Stress: %d\n", stress);
  mqtt.publish(TOPIC_STRESS, payload.c_str());

  if (stress >= 1) {

    Serial.println("Publishing Light: 1");
    mqtt.publish(TOPIC_LIGHT, "1");

    lightStart = millis();
    lightActive = true;
  }
}

// --------------------------------------------------
// Collect 10 seconds data
// --------------------------------------------------

void collectData() {

  beatCount = 0;
  lastBeat = millis();

  unsigned long startTime = millis();

  while (millis() - startTime < 10000) {

    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue)) {

      unsigned long now = millis();
      int ibi = now - lastBeat;

      lastBeat = now;

      if (ibi > 300 && ibi < 2000) {

        if (beatCount < MAX_BEATS) {
          ibiBuffer[beatCount++] = ibi;
        }
      }
    }

    delay(5);
  }
}

// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup() {

  Serial.begin(115200);
  Wire.begin(8, 9);

  Serial.println("Initializing MAX30105");

  if (!particleSensor.begin(Wire)) {

    Serial.println("MAX30105 not found");
    while (1);
  }

  // Better sensor configuration
  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  int sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 16384;

  
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  Serial.println("Place finger on sensor");

  connect_wifi();
  connect_mqtt();
}

// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop() {

  // MQTT reconnect
  if (!mqtt.connected()) {
    connect_mqtt();
  }

  mqtt.loop();

  // Turn off light after 10 seconds
  if (lightActive && millis() - lightStart >= 10000) {

    Serial.println("Publishing Light: 0");
    mqtt.publish(TOPIC_LIGHT, "0");

    lightActive = false;
  }

  long ir = particleSensor.getIR();

  if (ir > fingerThreshold && ir < 200000) {

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