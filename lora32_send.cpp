#include <Arduino.h>
#include <RadioLib.h>

// Heltec V4 SX1262
SX1262 radio = new Module(8, 14, 12, 13);

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Initializing LoRa...");

  int state = radio.begin(915.0);   // US frequency

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa init success!");
  } else {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.println("Sending packet...");

  int state = radio.transmit("Hello from Heltec V4");
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Packet sent!");
  } else {
    Serial.print("Send failed, code ");
    Serial.println(state);
  }

  delay(3000);
}