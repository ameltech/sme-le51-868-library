/*
    SmeIoT Library - Sendconfiguration

    Demostrate how move in configuration mode for the Telit Sigfox component.
    Any command passed trougth the USB console are forwarded to the SigFox Component.

    The command should finish with the '.' character.

    The Led13 shows the command status
    ON : command sent
    OFF: OK Answer received

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

// max length of message consider the register name + register Value + the symbol for write/read command
#define MAX_SFX_MESSAGE_LEN SIG_FOX_MAX_REGISTER_LEN+SIG_FOX_MAX_REG_VALUE_LEN+sizeof(SIGFOX_EQUAL_CHAR)
char sfcCommandMsg[MAX_SFX_MESSAGE_LEN];
char msgPtr;
#define COMMAND_END '.'

/*
Co All the incoming data to the command message will be forwarded to SFX
 * Consider the '.' as the termination command from the console
 */
void composeSendSFXCommand(void){

    while(SerialUSB.available()) {        
        if (msgPtr>MAX_SFX_MESSAGE_LEN) {
            SerialUSB.println("Command msg too big, not sent to the SFX antenna");
            msgPtr=0;
            return;
        }

        char data = (char)SerialUSB.read();
        //echoes the data
        SerialUSB.print(data);

        if (COMMAND_END != data) {
            sfcCommandMsg[msgPtr++] = data;
        }else  {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledGreenLight(HIGH);
#endif
            sfxAntenna.sfxSendConf(sfcCommandMsg, msgPtr); // send the data
            msgPtr=0;
        }
    }
}



// the setup function runs once when you press reset or power the board
void setup() {

    inConfiguration = false;

    SerialUSB.begin(115200);
    sfxAntenna.begin();

    sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode
}

// the loop function runs over and over again forever
void loop() {

    if (!inConfiguration) {
        uint8_t answerReady = sfxAntenna.hasSfxAnswer();
        if (answerReady){
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledGreenLight(HIGH);
#endif
            SerialUSB.println("SFX in Command mode");
            inConfiguration = true;
            msgPtr=0;
        }
    } else {
        // forward any command received by the USB port to the SFX chip
        if (SerialUSB.available()) {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
            ledGreenLight(LOW);
#endif
            composeSendSFXCommand();
        }

        // if message has been received correctly print it out
        if (sfxAntenna.hasSfxAnswer()) {
            if (sfxAntenna.getSfxError() == SME_SFX_OK) {
                SerialUSB.println("Command accepted !!");
                SerialUSB.println((const char*)sfxAntenna.getLastReceivedMessage());
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
                ledGreenLight(HIGH);
#endif      
            }
        }
    }
}
