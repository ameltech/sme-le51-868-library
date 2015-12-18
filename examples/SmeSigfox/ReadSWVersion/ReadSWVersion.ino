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


  SerialUSB.println("\n\nSFX in Command mode");
  sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode

  while (!SerialUSB) {
    ;
  }
}

// the loop function runs over and over again forever
void loop() {
  uint8_t diff;
  uint8_t answerReady = sfxAntenna.hasSfxAnswer();

  if (answerReady) {

    SerialUSB.print("Current installed Telit SFX Version = ");
    SerialUSB.println((const char*)sfxAntenna.readSwVersion());
    do {
      ledBlueLight(HIGH);
      delay(1000);
      ledBlueLight(LOW);
      delay(1000);
    } while (1);
  }
}
