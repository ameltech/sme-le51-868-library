/*
    SmeIoT Library - ChangeSerial

   The Sketch shows as to change the internal SFX Uart baudrate.

    created 27 Nov 2015
    by Mik (smkk@amel-tech.com)

   This example is in the public domain
   https://github.com/ameltech

   Telit le51-868-s more information available here:
   http://www.telit.com/products/product-service-selector/product-service-selector/show/product/le51-868-s/
 */

#include <SmeSFX.h>
#include <Arduino.h>

bool serialChanged = false;
// the setup function runs once when you press reset or power the board
void setup() {

  SerialUSB.begin(115200);
  sfxAntenna.begin(115200);


  SerialUSB.println("\n\nSFX in Command mode");
  sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode


}

// the loop function runs over and over again forever
void loop() {
  uint8_t diff;
  uint8_t answerReady = sfxAntenna.hasSfxAnswer();

  if (answerReady) {

    if (!serialChanged && sfxAntenna.getBaudRate() != B19200) {
      sfxAntenna.setBaudRate(B19200);
      delay(1000);
      do {
        ledGreenLight(HIGH);
        delay(1000);
        ledGreenLight(LOW);
      } while (1);
    }
    do {
      ledBlueLight(HIGH);
      delay(1000);
      ledBlueLight(LOW);
    } while (1);
  }
}


