
#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>

SX1262 radio = new Module(8, 14, 12, 13);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Initializing LoRa...");

  int state = radio.begin(
    915.0,  // frequency
    125.0,  // bandwidth
    12,     // spreading factor
    8,      // coding rate
    0x12,   // sync word
    22,     // TX power
    16,     // preamble length
    1.6     // TCXO voltage
  );
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }

  WiFi.mode(WIFI_OFF);

  // *** FIX: enable RF switch via DIO2 (required on Heltec V4) ***
  radio.setDio2AsRfSwitch();

  // *** FIX: GPIO46 LOW for receive (PA disabled, LNA path active) ***
  pinMode(46, OUTPUT);
  digitalWrite(46, LOW);

  // must be applied AFTER begin()
  // radio.setSpreadingFactor(12);
  // radio.setBandwidth(125.0);
  // radio.setCodingRate(8);
  // radio.setPreambleLength(16);

  Serial.println("LoRa init success!");
}

void loop() {
  String str;
  int state = radio.receive(str);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Received: ");
    Serial.println(str);
    Serial.print("RSSI: ");
    Serial.print(radio.getRSSI());
    Serial.println(" dBm");
  } else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}
