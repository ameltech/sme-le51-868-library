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
   https://github.com/ameltech
   
   Telit le51-868-s mor einformatioin available here:
   http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <Wire.h>
#include <SmeSFX.h>
#include <Arduino.h>



char helloMsg[5]= {'H','e', 'l', 'l', 'o'};
#define US_FREQ "AT$IF=902800000"
#define PAYLOAD_LEN 0x0f

// the setup function runs once when you press reset or power the board
void setup() {
    
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
                SerialUSB.println("Set US Freq 902.8MHz");
                sfxAntenna.sfxSendConf(US_FREQ, PAYLOAD_LEN); // send the data configuration
                initFinish++;
                break;

            case 2:
                SerialUSB.println("SFX in Data mode");
                sfxAntenna.setSfxDataMode();
                initFinish++;
                break;

            case 3:
                ledGreenLight(HIGH);
                initFinish++; // exit
                break;
            }
        }
    } while (initFinish!=4);


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
                ledGreenLight(LOW);
                SerialUSB.print(".");
                break;

            case SFX_DATA_ACK_OK:
                ledGreenLight(HIGH);
                SerialUSB.println(' ');
                SerialUSB.println("Answer OK :) :) :) :)");
                break;

            case SFX_DATA_ACK_KO:
                ledRedLight(HIGH);
                SerialUSB.println(' ');
                SerialUSB.println("Answer KO :( :( :( :(");
                break;
            }


        }
    }
}