#include <Arduino.h>
#include <RadioLib.h>

// Heltec V4 SX1262
SX1262 radio = new Module(8, 14, 12, 13);
int cnt = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Initializing LoRa...");

  int state = radio.begin(915.0);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }

  radio.setDio2AsRfSwitch(); // this line is important!

  radio.setSpreadingFactor(12);
  radio.setBandwidth(125.0);
  radio.setCodingRate(8);
  radio.setOutputPower(22);
  radio.setPreambleLength(16);

  Serial.println("LoRa init success!");
}

void loop() {
  cnt++;

  // *** FIX: enable GC1109 front-end PA before transmitting ***
  // (I asked claude code for this and I have no clue what this means)
  pinMode(46, OUTPUT);
  digitalWrite(46, HIGH);

  char buffer[50];
  sprintf(buffer, "packet #%d", cnt);

  Serial.printf("Sending: %s\n", buffer);
  int state = radio.transmit(buffer);

  digitalWrite(46, LOW);
  pinMode(46, INPUT);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Packet sent!");
  } else {
    Serial.print("Send failed, code ");
    Serial.println(state);
  }

  delay(1000);
}