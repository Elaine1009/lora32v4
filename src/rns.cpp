// this program tests both send and receive on heltec lora v4
#include <Arduino.h>
#include <RadioLib.h>
volatile bool rxDone = false;
unsigned long lastSend = 0;
void onReceive();

SX1262 radio = new Module(8, 14, 12, 13);
int cycle = 0; // every "5" seconds

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

  radio.setDio2AsRfSwitch(); // asks for FEM_EN to enable

	pinMode(2, OUTPUT); // enables FEM_EN
  digitalWrite(2, HIGH);

	radio.setPacketReceivedAction(onReceive);
	radio.startReceive();
  Serial.println("LoRa init success!");
}

void onReceive() {
	rxDone = true;
}

void loop()
{
  String str;
  int state = radio.receive(str);

  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.print("Received: ");
    Serial.println(str);
    Serial.print("RSSI: ");
    Serial.print(radio.getRSSI());
    Serial.println(" dBm");

    Serial.printf("Sending: %s\n", str);
    char buffer[70];
    snprintf(buffer, sizeof(buffer), "Received and sent back: %s", str.c_str());

	int sstate = radio.transmit(buffer);

    // digitalWrite(46, LOW); // turns PA off
    // pinMode(46, INPUT); // changes PA to floating

    if (sstate == RADIOLIB_ERR_NONE)
    {
      Serial.println("Packet sent!");
    }
    else
    {
      Serial.print("Send failed, code ");
      Serial.println(sstate);
    }
    radio.startReceive();
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT)
  {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}


