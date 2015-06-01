/*
    SmeIoT Library - DataModeUs

    Demostrate how move in data mode for the Telit Sigfox component.
    Before send the message on the air the Example set the frequency to 902.8MHz
    as per US Standard.

    The Library sends an "Hello" String over the SigFox network.

	The RGB Green Led ligth on when the Telit sends the message on the air, 
		ligth off at the positive answer from the component. 
    


    created 05 May 2015
    by Mik (smkk@amel-tech.com)

    This example is in the public domain

    http://www.amel-tech.com/smarteverything/tutorial/IoT/SigFox
 */

#include <Wire.h>
#include <SmeSFX.h>
#include <Arduino.h>



char helloMsg[5]= {'H','e', 'l', 'l', 'o'};
bool messageSent;

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);
    digitalWrite(PIN_LED_13, LOW);
    
    SerialUSB.begin(115200);
    sfxAntenna.begin();
    int initFinish=1;

    SerialUSB.println("SFX in Command mode");
    sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode

    do {
        uint8_t answerReady = sfxAntenna.hasSfxAnswer();
        if (answerReady){
            switch (initFinish){
            case 1:                                
                SerialUSB.println("SFX in Data mode");
                sfxAntenna.setSfxDataMode();
                initFinish++;
                break;

            case 2:
                digitalWrite(PIN_LED_13, HIGH);
                initFinish++; // exit
                break;
            }
        }
    } while (initFinish!=3);

    SerialUSB.println("sending Hello over the network");
    // send Hello on the air
    sfxAntenna.sfxSendData(helloMsg, strlen((char*)helloMsg));
}

// the loop function runs over and over again forever
void loop() {

    bool answerReady = sfxAntenna.hasSfxAnswer();

    if (answerReady) {
        if (sfxAntenna.getSfxMode() == sfxDataMode) {

            switch (sfxAntenna.sfxDataAcknoledge()) {
            case SFX_DATA_ACK_START:
                SerialUSB.println("Waiting Answer");
                break;

            case SFX_DATA_ACK_PROCESSING:
                SerialUSB.print('.');
                break;

            case SFX_DATA_ACK_OK:
                digitalWrite(PIN_LED_13, LOW);
                SerialUSB.println(' ');
                SerialUSB.println("Answer OK :) :) :) :)");
                break;

            case SFX_DATA_ACK_KO:
                SerialUSB.println(' ');
                SerialUSB.println("Answer KO :( :( :( :(");
                break;
            }
        }
    }
}

