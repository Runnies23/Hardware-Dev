// Board2 lighy


#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#define TOPIC_LIGHT    TOPIC_PREFIX "/light"
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_BROKER, 1883, wifiClient);
uint32_t last_publish;

#include <FastLED.h>
#define LED_PIN     7 //กำหนดขาที่เชื่อมต่อกับสายส่งสัญญาณของ LED ในที่คือขา 7
#define NUM_LEDS    16 //กำหนดจำนวนหลอดไฟ LED ที่ต้องการให้ทำงาน
CRGB leds[NUM_LEDS];



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
    int value = atoi((char*)payload); 
    if (value == 1) { // high light - open light

      leds[0] = CRGB(0, 0, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 0, 0);
      leds[7] = CRGB(0, 0, 0);
      leds[8] = CRGB(0, 0, 0);
      leds[9] = CRGB(0, 0, 0);
      leds[10] = CRGB(0, 0, 0);
      leds[11] = CRGB(0, 0, 0);
      leds[12] = CRGB(0, 0, 0);
      leds[13] = CRGB(0, 0, 0);
      leds[14] = CRGB(0, 0, 0);
      leds[15] = CRGB(0, 0, 0);
      FastLED.show();

      delay(1000);
      

      leds[0] = CRGB(255, 255, 255);
      leds[1] = CRGB(255, 255, 255);
      leds[2] = CRGB(255, 255, 255);
      leds[3] = CRGB(255, 255, 255);
      leds[4] = CRGB(255, 255, 255);
      leds[5] = CRGB(255, 255, 255);
      leds[6] = CRGB(255, 255, 255);
      leds[7] = CRGB(255, 255, 255);
      leds[8] = CRGB(255, 255, 255);
      leds[9] = CRGB(255, 255, 255);
      leds[10] = CRGB(255, 255, 255);
      leds[11] = CRGB(255, 255, 255);
      leds[12] = CRGB(255, 255, 255);
      leds[13] = CRGB(255, 255, 255);
      leds[14] = CRGB(255, 255, 255);
      leds[15] = CRGB(255, 255, 255);
      FastLED.show();
    }
    else {
      printf("Invalid payload received.\n");
    }
  }
}


void setup()
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  //init the sensor
  Serial.begin(115200);
  delay(1000);

  //set up mqtt and wifi
  connect_wifi();
  connect_mqtt();
  last_publish = 0;
}


void loop()
{
}

