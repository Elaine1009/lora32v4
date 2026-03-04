
#include <Arduino.h>
#include <RadioLib.h>

SX1262 radio = new Module(8, 14, 12, 13);

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Initializing LoRa...");

  int state = radio.begin(915.0);
  radio.setSpreadingFactor(12);
  radio.setBandwidth(125.0);
  radio.setCodingRate(8);
  radio.setPreambleLength(16);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa init success!");
  } else {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }
}

void loop() {
  String str;
  int state = radio.receive(str);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Received: ");
    Serial.println(str);
  }
}
