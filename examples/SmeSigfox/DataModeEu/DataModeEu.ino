/*
    SmeIoT Library - DataModeEu

    Demostrate how move in data mode for the Telit Sigfox component.

    The Library sends an "Hello" String over the SigFox network.

    The RGB Green Led ligth on when the Telit sends the message on the air, 
        ligth off at the positive answer from the component. 


    created 05 May 2015
    by Mik (smkk@axelelettronica.it)

    This example is in the public domain
    https://github.com/ameltech

    Telit le51-868-s more information available here:
    http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <Wire.h>
#include <SmeSFX.h>
#include <Arduino.h>



char helloMsg[5]= {'H','e', 'l', 'l', 'o'};
bool messageSent;

// the setup function runs once when you press reset or power the board
void setup() {

    SerialUSB.begin(115200);
    sfxAntenna.begin();
    int initFinish=1;

    while (!SerialUSB) {
        ; 
    }    

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
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
                ledGreenLight(HIGH);
#endif
                SerialUSB.println(' ');
                SerialUSB.println("Answer OK :) :) :) :)");
                break;

            case SFX_DATA_ACK_KO:
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
                ledRedLight(HIGH);
#endif
                SerialUSB.println(' ');
                SerialUSB.println("Answer KO :( :( :( :(");
                break;
            }
        }
    }
}