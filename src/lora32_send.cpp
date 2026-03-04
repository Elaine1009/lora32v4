#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
// #include <esp_bt.h>

// Heltec V4 SX1262
SX1262 radio = new Module(8, 14, 12, 13); // NSS, DIO1, RESET, BUSY
int cnt = 0; // package counter

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Initializing LoRa...");

  int state = radio.begin(915.0); // 915 MHz
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true); // stops program if fail
  }

  WiFi.mode(WIFI_OFF);
  // btstop();

  radio.setDio2AsRfSwitch(); // this line is important!

  radio.setSpreadingFactor(12); // sweeps frequencies for each bit- SF12=4096 chirp/bit, SF6=64chirps/bit
  radio.setBandwidth(125.0); // 7.8kHz to 500kHz
  radio.setCodingRate(8); // for every 4 bits, send CR. min 5 max 8
  radio.setOutputPower(22); // how loud broadcast is
  radio.setPreambleLength(16); // gives receiver more time to lock onto transmission signal

  Serial.println("LoRa init success!");
}

void loop() {
  cnt++;

  // *** FIX: enable GC1109 front-end PA before transmitting ***
  // (I asked claude code for this and I have no clue what this means)
  pinMode(46, OUTPUT); // enables GPIO46 as a power amplifier (FEM_PA)
  digitalWrite(46, HIGH); // turns the PA ON

  char buffer[50];
  sprintf(buffer, "packet #%d", cnt); // prints packet#

  Serial.printf("Sending: %s\n", buffer);
  int state = radio.transmit(buffer);

  digitalWrite(46, LOW); // turns PA off
  pinMode(46, INPUT); // changes PA to floating

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Packet sent!");
  } else {
    Serial.print("Send failed, code ");
    Serial.println(state);
  }

  delay(1000); // wait 1s
}