/*
    SmeIoT Library - getSwVersion

    Demonstrate how to read the SwVersion of the LE51-868
    
    The Sw Version is mapped in this way:
    pp.XCR.MM.mm-Bbbb<CR>pp.B00.NN.nn
    With:
    pp is the platform:
    ? GC: LE50-868
    ? GD: LE50-433
    ? GJ: LE70-868
    XCR: X is the stack (S for Star Network and B for Bootloader).
    C is the casing:
    ? 0: OEM boards
    ? 1: USB dongle or IP67 box
    R is reserved and always read as ?0?
    MM: Major version of firmware
    mm: minor version of firmware
    bbb: build number of firmware
    NN: major version number of bootloader
    Nn: minor version number of bootloader

    created 27 Apr 2015
    by Mik (smkk@amel-tech.com)

    This example is in the public domain
	https://github.com/ameltech
	 
	Telit le51-868-s mor einformatioin available here:	 
	http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */ 

#include <Wire.h>
#include <SmeSFX.h>
#include <Arduino.h>


bool inConfiguration;
char serialNum[SN_LENGTH];

// the setup function runs once when you press reset or power the board
void setup() {

    inConfiguration = false;

    SerialUSB.begin(115200);
    
    sfxAntenna.begin();
    sfxAntenna.readSwVersion(serialNum); // enter in configuration Mode
}


// the loop function runs over and over again forever
void loop() {

    ledGreenLight(HIGH);
    delay(1000);
    
    SerialUSB.println(serialNum);
           
}
