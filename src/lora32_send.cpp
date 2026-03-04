
#include <Arduino.h>
#include <RadioLib.h>

// Heltec V4 SX1262
SX1262 radio = new Module(8, 14, 12, 13); // sets GPIO pins of radio, these are default
int cnt = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("Initializing LoRa...");

  int state = radio.begin(750.0);   // US frequency
  
  // these apparentally help transmit further
  radio.setSpreadingFactor(12);
  radio.setBandwidth(125.0);
  radio.setCodingRate(8);
  radio.setOutputPower(22);
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
  cnt ++;
  Serial.println("Sending packet...");
  Serial.printf("# of packet sent: %d\n", cnt);
  char buffer[50];
  sprintf(buffer, "# of packet sent: %d\n", cnt);

  int state = radio.transmit(buffer);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Packet sent!");
  } else {
    Serial.print("Send failed, code ");
    Serial.println(state);
  }

  delay(1000);
}
