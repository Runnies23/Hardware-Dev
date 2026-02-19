// Board1 

#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 sensor;

typedef struct {
  int hr;
  int stress;
  int r;
  int g;
  int b;
} DataPacket;

DataPacket data;

uint8_t receiverMac[] = {0x24,0x6F,0x28,0xAA,0xBB,0xCC}; // <-- CHANGE

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!sensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found");
    while (1);
  }

  sensor.setup();
  sensor.setPulseAmplitudeRed(0x0A);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_peer_info_t peer;
  memcpy(peer.peer_addr, receiverMac, 6);
  peer.channel = 0;
  peer.encrypt = false;
  esp_now_add_peer(&peer);
}

void loop() {
  long irValue = sensor.getIR();

  if (checkForBeat(irValue)) {
    static long lastBeat = 0;
    long delta = millis() - lastBeat;
    lastBeat = millis();

    float bpm = 60 / (delta / 1000.0);

    if (bpm < 255 && bpm > 40) {
      data.hr = bpm;

      // simple stress estimate
      if (bpm < 70) data.stress = 1;
      else if (bpm < 90) data.stress = 2;
      else data.stress = 3;

      // light logic
      if (data.stress == 1) { data.r=0; data.g=255; data.b=100; }
      if (data.stress == 2) { data.r=255; data.g=180; data.b=0; }
      if (data.stress == 3) { data.r=255; data.g=0; data.b=0; }

      esp_now_send(receiverMac, (uint8_t*)&data, sizeof(data));

      Serial.printf("HR:%d Stress:%d\n", data.hr, data.stress);
    }
  }
}
