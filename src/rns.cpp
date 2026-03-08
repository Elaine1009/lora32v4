// this program tests both send and receive on heltec lora v4
#include <Arduino.h>
#include <RadioLib.h>
#define DELIM ":"
volatile bool rxDone = false;

void onReceive();

uint32_t DEVICE_ID;
SX1262 radio = new Module(8, 14, 12, 13);
int cycle = 0; // every "5" seconds

void setup() {
  Serial.begin(115200);
  delay(2000);
  DEVICE_ID = (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF);
	Serial.printf("Device ID: %08X\n", DEVICE_ID);
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
  String payload;
  String inID;
  int state = radio.receive(str);
  int delimindex = str.indexOf(DELIM);

  // Serial.print("Raw: '");
  // Serial.print(str);
  // Serial.print("' delim: ");
  // Serial.println(delimindex);
  
  if (str.length() == 0) {
    radio.startReceive();
    return;
  }

  if (delimindex == 8) {
    uint32_t senderID = strtoul(str.substring(0, 8).c_str(), NULL, 16); // takes first 8 characters of str and turns it into hex
    if (senderID == DEVICE_ID) {
      // Serial.println("Own packet received.");
      radio.startReceive(); // ignore packet if its own packet
      return;
    } else {
      inID = str.substring(0, 8);
      payload = str.substring(9);
    }
  } else {
    payload = str;
  }

  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.print("Received: ");
    Serial.print(str);
    Serial.print(", RSSI: ");
    Serial.print(radio.getRSSI());
    Serial.print(" dBm, ID: ");
    Serial.println(inID);

    char buffer[70];
    // snprintf(buffer, sizeof(buffer), "Received and sent back: %s", str.c_str());
    sprintf(buffer, "%08X%sReceived and sent back: %s", DEVICE_ID, DELIM, str.c_str());
    Serial.printf("Sending: %s\n", buffer);

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


