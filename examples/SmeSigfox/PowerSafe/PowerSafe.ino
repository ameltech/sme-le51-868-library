/*
SmeIoT Library - FwUpgrade

This Sketch update the Telit Chip to the GP.S03.02.00-B002GP.B00.01.0C version.
At the end the Telit Chip is upgraded to the newest version and its internal Baudrate
change to 115200

created 27 Nov 2015
by Mik (smkk@amel-tech.com)

This example is in the public domain
https://github.com/ameltech

Telit le51-868-s more information available here:
http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <SmeSFX.h>
#include <Arduino.h>

char helloMsg[5]= {'H','e', 'l', 'l', 'o'};


static void sleepSFX(void) {
	if (sfxAntenna.getSfxSleepMode()==SFX_HW_WAKE){
		SerialUSB.println("Set PowerSave Mode");
		sfxWakeup();
	}
}



// the setup function runs once when you press reset or power the board
void setup() {
	int initFinish = 1;
	SerialUSB.begin(115200);
	sfxAntenna.begin();

	if (!SerialUSB) {
		;
	}

	SerialUSB.println("SFX in Command mode");
	sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode

	do {
		uint8_t answerReady = sfxAntenna.hasSfxAnswer();
		if (answerReady){
			switch (initFinish){
			case 1:
				// set the PowerSafe mode
				sfxAntenna.setSfxSleepMode(SFX_HW_WAKE);
				initFinish++;
				break;

			case 2:
				SerialUSB.println("New Power Safe configured !");
				sfxAntenna.setSfxDataMode();
				initFinish++;
				break;

			}
		}
	} while (initFinish!=2);

	SerialUSB.println("The SFX chip will be move in powersafe after every msg sent.");
	SerialUSB.println("To test the PowerSafe type:");
	SerialUSB.println("1) wake  (the message will be  sent)");
	SerialUSB.println("0) don't wake (the message will NOT sent)");
}

unsigned long time;

// the loop function runs over and over again forever
void loop() {

	// wait for a trigger from the Serial Usb
	while (!SerialUSB.available()) {
		char wake = (char)SerialUSB.read();

		switch (wake) {

		// wake SFX
		case '1':
			sfxWakeup();
			break;

			//do nothing.... do not wake SFX
		case '0':
			break;
		}
	}

	// send Hello on the air
	sfxAntenna.sfxSendData(helloMsg, strlen((char*)helloMsg));

	//charge timeout it will be useful to show
	// the the SFX was not waked up
	time = millis();

	uint8_t exit=0;
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
					SerialUSB.println(' ');
					SerialUSB.println("Answer OK :) :) :) :)");
					exit = 1;
					break;

				case SFX_DATA_ACK_KO:
					ledRedLight(HIGH);
					SerialUSB.println(' ');
					SerialUSB.println("Answer KO :( :( :( :(");
					exit=1;
					break;
				}
			}
		}
		// after 10Sec. exit from timeout
		// it is enough because SFX message, and its answer,
		// take around 6Sec.
		if ((millis()-time) >= 10000)
			exit = 2;

	} while (!exit);

	// if timeout is expired, show with the blu led on
	if (exit=2) {
		ledGreenLight(LOW);
		ledRedLight(LOW);
		ledBlueLight(HIGH);
	}

	// sleep the SFX again for new test
	sfxSleep();

}
