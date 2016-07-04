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
#define SFX_FRAME "333336"

bool debounce;

// the setup function runs once when you press reset or power the board
void setup() {
    SerialUSB.begin(115200);
    sfxAntenna.begin();
    debounce =false;

    ledYellowTwoLight(LOW);
    ledYellowOneLight(LOW);
    
    while (!SerialUSB) {
        ;
    }
    


    sfxAntenna.sfxSendDataAck(SFX_FRAME, strlen(SFX_FRAME), true); // send the data;
    SerialUSB.println("Message sent over the air");
}

// the loop function runs over and over again forever
void loop() {    

    // if message has been received correctly print it out
    if (sfxAntenna.hasSfxAnswer()) {
        if (sfxAntenna.getSfxError() == SME_SFX_OK) {
            ledYellowOneLight(HIGH);
            ledYellowTwoLight(HIGH);

            SerialUSB.println("Acknowledge received");
            SerialUSB.print("Message = ");
            SerialUSB.println((const char*)sfxAntenna.getLastReceivedMessage());

            sfxAntenna.setSfxDataMode(); // move in Data Mode if need
            debounce = false;
        }
        else {
            SerialUSB.println("Acknowledge not received");
            sfxAntenna.setSfxDataMode(); // move in Data Mode if need
            debounce = false;
        }
    }

}