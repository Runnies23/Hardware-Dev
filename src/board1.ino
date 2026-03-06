// Board1 PPG
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#define TOPIC_LIGHT    TOPIC_PREFIX "/light"
#define TOPIC_HRV    TOPIC_PREFIX "/hrv"
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
unsigned long lastPublish = 0;

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
  mqtt.subscribe(TOPIC_LED_RED);
  printf("MQTT broker connected.\n");
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(topic, TOPIC_LED_RED) == 0) {
    payload[length] = 0; // null-terminate the payload to treat it as a string
    int value = atoi((char*)payload); 
    if (value == 0) {
      digitalWrite(RED_GPIO, LOW);
    }
    else if (value == 1) {
      digitalWrite(RED_GPIO, HIGH);
    }
    else {
      printf("Invalid payload received.\n");
    }
  }
}

void setup()
{
  //init the sensor
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initializing...");
  // Recommended ESP32-S3 I2C pins
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

void loop()
{
  long irValue = particleSensor.getIR();

  // Detect heartbeat
  if (checkForBeat(irValue))
  {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60.0 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255)
    {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++)
        beatAvg += rates[i];

      beatAvg /= RATE_SIZE;
    }
  }

  // Debug output
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.println(beatAvg);


  from 
  ###right here is the HVB

  if (beatAvg > 90){
      send playload of the light into another board which// 
  }

  if (millis() - lastPublish >= 60000)
  {
    String payload = String(beatAvg);
    Serial.printf("Publishing HeartRate: %d\n", beatAvg);
    mqtt.publish(TOPIC_LIGHT, payload.c_str());

    lastPublish = millis();
  }

  // Finger detection
  if (irValue < 50000)
  {
    Serial.println("No finger detected");
  }
}
