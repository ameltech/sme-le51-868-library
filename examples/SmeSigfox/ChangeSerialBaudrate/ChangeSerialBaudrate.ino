/*
    SmeIoT Library - ChangeSerial

   The Sketch shows as to change the internal SFX Uart baudrate.

   Adapt the initialization of the SFX chips with the real baudrate.
   The default one is 19200, so the first time this sketch runs the corect 
   initializatioin api is   sfxAntenna.begin(19200);

   The second time need to be change according with the value set to the 
   sfxAntenna.setBaudRate(<baudRate>); API, for this example 115200

    created 27 Nov 2015
    by Mik (smkk@axelelettronica.it)

   This example is in the public domain
   https://github.com/ameltech

   Telit le51-868-s more information available here:
   http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <SmeSFX.h>
#include <Arduino.h>

// the setup function runs once when you press reset or power the board
void setup() {

    SerialUSB.begin(115200);
    sfxAntenna.begin();

    while (!SerialUSB) {
        ;
    }

    SerialUSB.println("\n\nSFX in Command mode");
    sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode


}

// the loop function runs over and over again forever
void loop() {
    uint8_t diff;
    uint8_t answerReady = sfxAntenna.hasSfxAnswer();

    if (answerReady) {

        if (sfxAntenna.getBaudRate() != B115200) {
            SerialUSB.println("\nChange BaudRate.....");
            sfxAntenna.setBaudRate(B115200);

            delay(1000);

            SerialUSB.println("\nChanged !!");

            do {
                SerialUSB.println("\n BaudRate Identical, it has not been changed.");
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
                ledGreenLight(HIGH);
#endif;
                delay(1000);
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
                ledGreenLight(LOW);
#endif
            } while (1);
        }

        do {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledBlueLight(HIGH);
#endif
            delay(1000);
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledBlueLight(HIGH);
#endif
        } while (1);
    }
}
