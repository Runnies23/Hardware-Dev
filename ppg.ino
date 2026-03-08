#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#define TOPIC_LIGHT  TOPIC_PREFIX "/light" //light stage
#define TOPIC_HRV  TOPIC_PREFIX "/hrv" //hrv value
#define TOPIC_BPM  TOPIC_PREFIX "/bpm" //bpm value 
#define TOPIC_STRESS TOPIC_PREFIX "/stress" //stress stage

WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
uint32_t last_publish;

// sensor lib
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
int peak_ir = 0;


const int fingerThreshold = 50000; // Finger detection threshold
const int beatThreshold = 2000; // Beat detection threshold
const int peakThreshold = 1000000;     // detect heartbeat peak

// HRV storage
const int MAX_BEATS = 30;
int ibiBuffer[MAX_BEATS];
int beatCount = 0;
unsigned long lastBeatTime = 0;
int prevSignal = 0;


void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  printf("WiFi MAC address is %s\n", WiFi.macAddress().c_str());
  printf("Connecting to WiFi %s.\n", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    printf(".");
    fflush(stdout);
    delay(500);
  }
  printf("\nWiFi connected.\n");
}

void connect_mqtt() {
  printf("Connecting to MQTT broker at %s.\n", MQTT_BROKER);
  if (!mqtt.connect("", MQTT_USER, MQTT_PASS)) {
    printf("Failed to connect to MQTT broker.\n");
    for (;;) {} // wait here forever
  }
  mqtt.setCallback(mqtt_callback);
  mqtt.subscribe(TOPIC_LIGHT);
  mqtt.subscribe(TOPIC_HRV);
  printf("MQTT broker connected.\n");
}

// ---------- Finger Detection ----------
bool fingerDetected(int ir) {
  return ir > fingerThreshold;
}

// ---------- Simple Moving Average Filter ----------
int smoothSignal(int signal) {
  static int buffer[4];
  static int index = 0;
  buffer[index] = signal;
  index = (index + 1) % 4;
  int sum = 0;
  for (int i = 0; i < 4; i++)
    sum += buffer[i];
  return sum / 4;
}

// ---------- Peak Detection from IR ----------
bool detectPeak(int ir) {
  if (ir > peakThreshold && prevIR <= peakThreshold) {
    prevIR = ir;
    return true;
  }
  prevIR = ir;
  return false;
}

// ---------- BPM ----------
float computeBPM() {
  if (beatCount < 2) return 0;
  float sum = 0;
  for (int i = 0; i < beatCount; i++)
    sum += ibiBuffer[i];
  float avgIBI = sum / beatCount;
  return 60000.0 / avgIBI;
}

// ---------- HRV (RMSSD) ----------
float computeHRV() {
  if (beatCount < 3) return 0;
  float sum = 0;
  for (int i = 1; i < beatCount; i++) {
    float diff = ibiBuffer[i] - ibiBuffer[i - 1];
    sum += diff * diff;
  }
  return sqrt(sum / (beatCount - 1));
}

// ---------- Stress Estimation ----------
int computeStress(float rmssd) {
  if (rmssd > 50) return 0;
  else if (rmssd > 30) return 1;
  else if (rmssd > 15) return 2;
  else return 3;
}

// ---------- Publish MQTT ----------
void publishResults(float bpm, float hrv, int stress) {
  char msg[20];
  sprintf(msg, "%.2f", (int)bpm);
  client.publish(TOPIC_BPM, msg);
  sprintf(msg, "%.2f", (int)hrv);
  client.publish(TOPIC_HRV, msg);
  sprintf(msg, "%d", (int)stress);
  client.publish(TOPIC_STRESS, msg);
}

// ---------- Collect 10s Data ----------
void collectData() {
  beatCount = 0;
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    currentIR = particleSensor.getIR();
    filteredIR = smoothSignal(currentIR);

    if (detectPeak(filteredIR)) {
      unsigned long now = millis();
      int ibi = now - lastBeatTime;
      if (ibi > 300 && ibi < 2000) {
        ibiBuffer[beatCount] = ibi;
        beatCount++;
        if (beatCount >= MAX_BEATS)
          break;
      }
      lastBeatTime = now;
    }
    delay(10);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing...");
  Wire.begin(8, 9);          // SDA = GPIO8, SCL = GPIO9
  Wire.setClock(100000);     // 100kHz for stability
  if (!particleSensor.begin(Wire))
  {
    Serial.println("MAX30105 was not found. Check wiring.");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  Serial.println("Place your finger on the sensor.");

  //set up mqtt and wifi
  connect_wifi();
  connect_mqtt();
  last_publish = 0;
}

void loop() {
  int ir = analogRead(IR_PIN);
  if (fingerDetected(ir)) {
    Serial.println("Finger detected");
    collectData();
    float bpm = computeBPM();
    float hrv = computeHRV();
    int stress = computeStress(hrv);

    Serial.print("BPM: ");
    Serial.println(bpm);

    Serial.print("HRV: ");
    Serial.println(hrv);

    Serial.print("Stress: ");
    Serial.println(stress);

    publishResults(bpm, hrv, stress);

    delay(5000);
  }
  else {

    Serial.println("Place finger on sensor");

    delay(1000);
  }
}