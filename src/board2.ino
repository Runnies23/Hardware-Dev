// board2 

#include <WiFi.h>
#include <esp_now.h>

#define R_PIN 25
#define G_PIN 26
#define B_PIN 27

typedef struct {
  int hr;
  int stress;
  int r;
  int g;
  int b;
} DataPacket;

DataPacket incoming;

void onReceive(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming, incomingData, sizeof(incoming));

  analogWrite(R_PIN, incoming.r);
  analogWrite(G_PIN, incoming.g);
  analogWrite(B_PIN, incoming.b);

  Serial.printf("HR:%d Stress:%d\n", incoming.hr, incoming.stress);
}

void setup() {
  Serial.begin(115200);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {}
