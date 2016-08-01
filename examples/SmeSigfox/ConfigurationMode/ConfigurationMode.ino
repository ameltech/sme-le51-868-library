/*
    SmeIoT Library - ConfigurationMode

    Demonstrate how move in configuration mode for the Telit Sigfox component.

    If the Telit component is well configured the RGB Green Led blinks

    created 27 Apr 2015
    by Mik (smkk@axelelettronica.it)

    This example is in the public domain
    https://github.com/ameltech

    Telit le51-868-s more information available here:    
    http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */ 

#include <Wire.h>
#include <SmeSFX.h>
#include <Arduino.h>


bool inConfiguration;

// the setup function runs once when you press reset or power the board
void setup() {

    inConfiguration = false;

    SerialUSB.begin(115200);

    sfxAntenna.begin();
    sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode
}


// the loop function runs over and over again forever
void loop() {


    delay(1000);

    if (!inConfiguration) {
        uint8_t answerReady = sfxAntenna.hasSfxAnswer();
        if (answerReady){
            SerialUSB.println("Entered in configuration Mode");
            inConfiguration = true;
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledGreenLight(HIGH);
#endif;
        }
    } else {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
        ledGreenLight(LOW);
#endif;
        delay(1000);
    }    
}

