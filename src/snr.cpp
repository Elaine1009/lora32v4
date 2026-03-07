// this program tests both send and receive on heltec lora v4
#include <Arduino.h>
#include <RadioLib.h>
#define DELIM ":"
volatile bool rxDone = false;
unsigned long lastSend = 0;
void onReceive();

uint32_t DEVICE_ID;

SX1262 radio = new Module(8, 14, 12, 13);
int cycle = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
	// bottom 4 bytes of MAC address of chip
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

void loop() {
	if (rxDone) {
		rxDone = false;
		String str;
		String payload;
		String inID = "";
		int state = radio.readData(str);
		int delimindex = str.indexOf(DELIM);
		/*
		Serial.print("Raw: '");
		Serial.print(str);
		Serial.print("' delim: ");
		Serial.println(delimindex);
		*/
		if (str.length() == 0) { // if zero length string, then ignore it
			radio.startReceive();
			return;
		}
		if (delimindex == 8) {
			uint32_t senderID = strtoul(str.substring(0, 8).c_str(), NULL, 16); // takes first 8 characters of str and turns it into hex
			if (senderID == DEVICE_ID) { // i guess we should never hit this?
				Serial.println("Own packet received."); // delete, testing line
				radio.startReceive(); // start receive before returning
				return;
			} else { // this means there is device ID so we separate the ID and the message
				inID = str.substring(0, 8);
				payload = str.substring(9);
			}
		} else { // no ID found, so we just print the whole message
			payload = str;
		}
		if (state == RADIOLIB_ERR_NONE) {
			Serial.print("Received: ");
			Serial.print(payload);
			Serial.print(", RSSI: ");
			Serial.print(radio.getRSSI());
			Serial.println(" dBm, ID: ");
			Serial.println(inID);
		} else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
			Serial.print("Receive failed, code ");
			Serial.println(state);
		}
		radio.startReceive(); // rearm receive after receiving a packet
	}
  if (millis() - lastSend >= 5000) { // assuming 24k clock speed, 5 seconds 
		lastSend = millis();
		cycle++;
		char buffer[50];
		sprintf(buffer, "%08X%spacket #%d", DEVICE_ID, DELIM, cycle);
		Serial.printf("Sending: %s\n", buffer);

		int state = radio.transmit(buffer);
		if (state == RADIOLIB_ERR_NONE) {
			Serial.println("Packet sent!");
		} else {
			Serial.print("Send failed, code ");
			Serial.println(state);
		}

		radio.startReceive(); // start receiving after sending
	}
}


