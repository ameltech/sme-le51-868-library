/*
SmeIoT Library - PowerSave

This Sketch update Shows how to put the Telit chip in powerSave mode.
This will reduce the power consumption of the unit.

First need to configure the Telit Chip in a specific power save mode.
    This operation could be just once, it is stored in the internal Telit flash.

When configured in SFX_HW_WAKE the specific wakeup pin need to be drive, and to do this there is
    the specific api of the core

created 27 Nov 2015
by Mik (smkk@axelelettronica.it)

This example is in the public domain
https://github.com/ameltech

Telit le51-868-s more information available here:
http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <SmeSFX.h>
#include <Arduino.h>

char helloMsg[5] = {'H', 'e', 'l', 'l', 'o'};


static void sleepSFX(void) {
	if (sfxAntenna.getSfxSleepMode() == SFX_HW_WAKE) {
		SerialUSB.println("Set PowerSave Mode");     
		sfxSleep();
		delay(10);     
	}
}



// the setup function runs once when you press reset or power the board
void setup() {
	int initFinish = 1;
	SerialUSB.begin(115200);
	sfxAntenna.begin();

	//while (!SerialUSB.available()) {
	while (!SerialUSB) {
		;
	}

	sfxWakeup();
	delay(10);


	SerialUSB.println("SFX in Command mode");
	sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode

	do {
		uint8_t answerReady = sfxAntenna.hasSfxAnswer();
		if (answerReady) {
			switch (initFinish) {
			case 1:
				// set the PowerSafe mode
				sfxAntenna.setSfxSleepMode(SFX_HW_WAKE);
				sfxAntenna.setSfxDataMode();
				initFinish++;
				break;

			case 2:
				SerialUSB.println("New Power Safe configured !");
				initFinish++;
				break;

			}
		}
	} while (initFinish != 2);

	SerialUSB.println("The SFX chip will be move in powersafe after every msg sent.");
	SerialUSB.println("To test the PowerSafe type:");
	SerialUSB.println("1) wake  (the message will be  sent)");
	SerialUSB.println("0) don't wake (the message will NOT sent)");
}

unsigned long time, newTime;
#define TIME_OUT 8000

// the loop function runs over and over again forever
void loop() {
	char wake = 0;
	uint8_t exit = 0;

  // sleep the SFX again for new test
  sfxSleep();

	// wait for a trigger from the Serial Usb
	do {
		if (SerialUSB.available()) {
			wake = (char)SerialUSB.read();
		}
		switch (wake) {

		// wake SFX
		case '1':
			SerialUSB.println("WakeUp SFX");
			if (sfxAntenna.getSfxSleepMode() == SFX_HW_WAKE) {
				sfxWakeup();
				delay(10);
			}  
			exit = 1;
			break;

			//do nothing.... do not wake SFX
		case '0':
			exit = 1;
			break;

		default:
			break;
		}
	} while (!exit);


	ledGreenLight(LOW);
	ledRedLight(LOW);
	ledBlueLight(LOW);

	// send Hello on the air
	SerialUSB.println("Sending Hello over the air");
	sfxAntenna.sfxSendData(helloMsg, strlen((char*)helloMsg));

	exit=0;
	//charge timeout it will be useful to show
	// the the SFX was not waked up
	time = millis();
	do {
		bool answerReady = sfxAntenna.hasSfxAnswer();

		if (answerReady) {
			if (sfxAntenna.getSfxMode() == sfxDataMode) {

				switch (sfxAntenna.sfxDataAcknoledge()) {
				case SFX_DATA_ACK_START:
					SerialUSB.println("Waiting Answer");
					break;

				case SFX_DATA_ACK_PROCESSING:
					ledGreenLight(LOW);
					SerialUSB.print(".");
					break;

				case SFX_DATA_ACK_OK:
					ledGreenLight(HIGH);
					SerialUSB.println("\nAnswer OK :) :) :) :)\n");
					exit = 1;
					break;

				case SFX_DATA_ACK_KO:
					ledRedLight(HIGH);
					SerialUSB.println("\nAnswer KO :( :( :( :(\n");
					exit = 1;
					break;
				}
			}
		}

		// after 10Sec. exit from timeout
		// it is enough because SFX message, and its answer,
		// take around 6Sec.
		newTime = millis();
		if ((newTime- time) >= TIME_OUT) {
			exit = 2;
		}      

	} while (!exit);

	// if timeout is expired, show with the blu led on
	if (exit == 2) {
		ledGreenLight(LOW);
		ledRedLight(LOW);
		ledBlueLight(HIGH);
    SerialUSB.println("TimeOUT !!:( :( :( :(\n");
	}
}

