#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <math.h>

#include "MAX30105.h"
#include "heartRate.h"
#include "config.h"

// MQTT Topics
#define TOPIC_BPM    TOPIC_PREFIX "/bpm"
#define TOPIC_HRV    TOPIC_PREFIX "/hrv"
#define TOPIC_STRESS TOPIC_PREFIX "/stress"

// Finger detection threshold
#define FINGER_THRESHOLD 50000

// HRV buffer
#define MAX_BEATS 30

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

MAX30105 particleSensor;

// beat detection
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// HRV
int ibiBuffer[MAX_BEATS];
int beatCount = 0;

// timing
unsigned long collectionStart = 0;
bool collecting = false;


// ---------------- WIFI ----------------
void connect_wifi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("WiFi Connected");
}


// ---------------- MQTT ----------------
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
}

void connect_mqtt() {

  mqtt.setServer(MQTT_BROKER, 1883);
  mqtt.setCallback(mqtt_callback);

  while (!mqtt.connected()) {

    Serial.print("Connecting MQTT...");

    if (mqtt.connect("esp32_hrv", MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
    } else {

      Serial.print("failed rc=");
      Serial.println(mqtt.state());
      delay(2000);
    }
  }
}


// ---------------- SENSOR ----------------
bool fingerDetected(long ir) {
  return ir > FINGER_THRESHOLD;
}


// ---------------- BPM ----------------
float computeBPM() {

  if (beatCount < 2) return 0;

  float sum = 0;

  for (int i = 0; i < beatCount; i++)
    sum += ibiBuffer[i];

  float avgIBI = sum / beatCount;

  return 60000.0 / avgIBI;
}


// ---------------- HRV (RMSSD) ----------------
float computeHRV() {

  if (beatCount < 3) return 0;

  float sum = 0;

  for (int i = 1; i < beatCount; i++) {

    float diff = ibiBuffer[i] - ibiBuffer[i - 1];
    sum += diff * diff;
  }

  return sqrt(sum / (beatCount - 1));
}


// ---------------- Stress ----------------
int computeStress(float rmssd) {

  if (rmssd > 50) return 0;      // relaxed
  else if (rmssd > 30) return 1; // normal
  else if (rmssd > 15) return 2; // moderate
  else return 3;                 // high
}


// ---------------- MQTT Publish ----------------
void publishResults(float bpm, float hrv, int stress) {

  char msg[20];

  sprintf(msg, "%d", (int)bpm);
  mqtt.publish(TOPIC_BPM, msg);

  sprintf(msg, "%d", (int)hrv);
  mqtt.publish(TOPIC_HRV, msg);

  sprintf(msg, "%d", stress);
  mqtt.publish(TOPIC_STRESS, msg);

  Serial.println("Published to MQTT");
}


// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);

  Serial.println("Initializing sensor...");

  Wire.begin(8, 9);

  if (!particleSensor.begin(Wire)) {
    Serial.println("MAX30105 not found");
    while (1);
  }

  particleSensor.setup(60, 4, 2, 100, 411, 4096);
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);

  Serial.println("Place finger on sensor");

  connect_wifi();
  connect_mqtt();
}


// ---------------- LOOP ----------------
void loop() {

  // reconnect WiFi
  if (WiFi.status() != WL_CONNECTED)
    connect_wifi();

  // reconnect MQTT
  if (!mqtt.connected())
    connect_mqtt();

  mqtt.loop();

  long ir = particleSensor.getIR();

  // -------- finger detection --------
  if (!fingerDetected(ir)) {

    Serial.println("Waiting for finger...");
    collecting = false;
    beatCount = 0;
    delay(500);
    return;
  }

  // -------- beat detection --------
  if (checkForBeat(ir)) {

    long delta = millis() - lastBeat;
    lastBeat = millis();

    if (delta > 300 && delta < 2000) {

      if (beatCount < MAX_BEATS) {

        ibiBuffer[beatCount] = delta;
        beatCount++;

        Serial.print("Beat detected IBI=");
        Serial.println(delta);
      }
    }
  }

  // -------- start collection --------
  if (!collecting) {

    collecting = true;
    collectionStart = millis();
    beatCount = 0;

    Serial.println("Collecting HRV data...");
  }

  // -------- after 10 seconds compute --------
  if (collecting && millis() - collectionStart > 10000) {

    collecting = false;

    float bpm = computeBPM();
    float hrv = computeHRV();
    int stress = computeStress(hrv);

    Serial.println("---- RESULT ----");

    Serial.print("BPM: ");
    Serial.println(bpm);

    Serial.print("HRV: ");
    Serial.println(hrv);

    Serial.print("Stress: ");
    Serial.println(stress);

    publishResults(bpm, hrv, stress);

    beatCount = 0;
  }
}