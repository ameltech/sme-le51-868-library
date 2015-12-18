/*
SmeIoT Library - SfxDownlik

The function demostrates how to request the 8 bytes of downlink message (AckMessage).
Remember that 4 downlink messages are possible per day.
The acknowledge answer could take up to 30Seconds before return to the system.

A configuration on your Sigfox backend is required.
Here a brief tutorial: http://www.disk91.com/2014/technology/internet-of-things-technology/sigfox-downlink-howto/

The RGB Led shows the acknoledge status
GREEN : Ack Message received
RED:    No Ack Message received

created 13 Dec 2015
by Mik (smkk@axelelettronica.it)

This example is in the public domain
https://github.com/ameltech

Telit le51-868-s more information available here:
http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
*/

#include <SmeSFX.h>
#include <Arduino.h>

// max length of message consider the register name + register Value + the symbol for write/read command
char sfcCommandMsg[20];
#define SFX_FRAME "333332"

bool debounce;

// the setup function runs once when you press reset or power the board
void setup() {
    sfxAntenna.begin();
    debounce =false;
    digitalWrite(PIN_LED_GREEN, HIGH);
   
}

// the loop function runs over and over again forever
void loop() {

    // forward any command received by the USB port to the SFX chip
    if (isButtonOnePressed() && !debounce) {
         sfxAntenna.sfxSendDataAck(SFX_FRAME, strlen(SFX_FRAME), true); // send the data;
        ledBlueLight(HIGH);   
        ledRedLight(LOW);
        ledGreenLight(LOW);     
        debounce = true;
    }

    // if message has been received correctly print it out
    if (sfxAntenna.hasSfxAnswer()) {
        if (sfxAntenna.getSfxError() == SME_SFX_OK) {
            (const char*)sfxAntenna.getLastReceivedMessage();
            ledBlueLight(LOW);  
            ledGreenLight(HIGH);
            sfxAntenna.setSfxDataMode(); // move in Data Mode if need
            debounce = false;
        }
        else {
            ledBlueLight(LOW);
            ledRedLight(HIGH);
            sfxAntenna.setSfxDataMode(); // move in Data Mode if need
            debounce = false;
        }
    }

}
