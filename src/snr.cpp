// this program tests both send and receive on heltec lora v4
#include <Arduino.h>
#include <RadioLib.h>
#include <WiFi.h>
#define DELIM ":"
volatile bool rxDone = false;
unsigned long lastSend = 0;
void onReceive();

uint32_t DEVICE_ID;

SX1262 radio = new Module(8, 14, 12, 13);
int cycle = 0;

void setup() {
	Serial.begin(115200);
	pinMode(36, OUTPUT);
  	digitalWrite(36, LOW);
	delay(2000);
	uint64_t mac = ESP.getEfuseMac();
	// bottom 4 bytes of MAC address of chip
	DEVICE_ID = (uint32_t)(mac & 0xFFFFFFFF) ^ (uint32_t)(mac >> 32);
	Serial.printf("Device ID: %08X\n", DEVICE_ID);
	Serial.println("Initializing LoRa...");
	WiFi.mode(WIFI_OFF);

	int state = radio.begin(
		915.0,  // frequency
		125.0,  // bandwidth
		7,     // spreading factor
		8,      // coding rate
		0x34,   // sync word
		22,     // TX power
		16,     // preamble length
		1.8     // TCXO voltage
	);
	if (state != RADIOLIB_ERR_NONE) {
		Serial.print("LoRa init failed, code ");
		Serial.println(state);
		while (true);
	}

	radio.setDio2AsRfSwitch(); // asks for FEM_EN to enable
	radio.setCurrentLimit(140.0);


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
		int rstate = radio.readData(str);
		int delimindex = str.indexOf(DELIM);

		// Serial.print("Raw: '");
		// Serial.print(str);
		// Serial.print("' delim: ");
		// Serial.println(delimindex);

		if (str.length() == 0) { // if zero length string, then ignore it
			radio.startReceive();
			return;
		}
		if (delimindex == 8) {
			uint32_t senderID = strtoul(str.substring(0, 8).c_str(), NULL, 16); // takes first 8 characters of str and turns it into hex
			if (senderID == DEVICE_ID) { // i guess we should never hit this?
				// Serial.println("Own packet received."); // delete, testing line
				radio.startReceive(); // start receive before returning
				return;
			} else { // this means there is device ID so we separate the ID and the message
				inID = str.substring(0, 8);
				payload = str.substring(9);
			}
		} else { // no ID found, so we just print the whole message
			payload = str;
		}
		if (rstate == RADIOLIB_ERR_NONE) {
			Serial.print("Received: ");
			Serial.print(payload);
			Serial.print(", RSSI: ");
			Serial.print(radio.getRSSI());
			Serial.print(" dBm, ID: ");
			Serial.println(inID);
		} else if (rstate != RADIOLIB_ERR_RX_TIMEOUT) {
			Serial.print("Receive failed, code ");
			Serial.println(rstate);
		}
		radio.startReceive(); // rearm receive after receiving a packet
	}
	if (millis() - lastSend >= 10000) { // A lot of seconds
		pinMode(46, OUTPUT);
  		digitalWrite(46, HIGH);
		lastSend = millis();
		cycle++;
		char buffer[50];
		sprintf(buffer, "%08X%spacket #%d", DEVICE_ID, DELIM, cycle);
		Serial.printf("Sending: %s\n", buffer);

		int sstate = radio.transmit(buffer);
		if (sstate == RADIOLIB_ERR_NONE) {
			Serial.println("Packet sent!");
		} else {
			Serial.print("Send failed, code ");
			Serial.println(sstate);
		}

		digitalWrite(46, LOW); 	
  		pinMode(46, INPUT); 

		radio.startReceive(); // start receiving after sending
	}
}


