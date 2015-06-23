/*
* smeSFX.cpp
*
* Created: 4/26/2015 10:15:50 PM
* by Mik (smkk@amel-tech.com)
*
*/
#include <Arduino.h>
#include "../SmeSFX.h"
#include "smeSFXModel.h"
#include "smeErrorCode.h"

// fixed position on the sequence number array, used by the answer FSM to check the consistence
// between the sent message and the received one.
#define DATA_SEQUENCE_NUMBER 0
#define KEEP_SEQUENCE_NUMBER 1

SmeSFX::SmeSFX(void) {
    memset(swVer, 0, SN_LENGTH);
    swVerLength=0;
    sfxSequenceNumber = 0x25;
    sfxMode = sfxDataMode;
    recFsm	= headerRec;
}

void SmeSFX::begin (void){
    SigFox.begin(19200);
    sfxMode = sfxDataMode;
}

void SmeSFX::setSfxConfigurationMode(void) {
    SigFox.print(ENTER_CONF_MODE);
    answer.payloadPtr=0;
    sfxMode = sfxConfigurationMode;
}

void SmeSFX::setSfxDataMode(void) {
    SigFox.print(ENTER_DATA_MODE);
    answer.payloadPtr=0;
    sfxMode = sfxEnterDataMode;
}

bool SmeSFX::hasSfxAnswer(void) {
    if (readSfxAnswer()) {
        return (SME_SFX_OK==getSfxError());
    }
    return false;
}


// PRIVATE FUNCTIONS
const byte SmeSFX::readSfxAnswer(void)
{
    while (SigFox.available()) {
        // get the new byte:
        char inChar = (char)SigFox.read();

        switch(sfxMode) {
            case sfxConfigurationMode:
            case sfxEnterDataMode:
            return composeSfxConfigurationAnswer(inChar);
            break;

            case sfxDataMode:
            return composeSfxDataAnswer(inChar);
            break;

            default:
            return false;
            break;
        }
    }
    return false;
}


void  SmeSFX::sfxSendConf(const char confMsg[], byte confLen) {
    memset(message, 0 , sizeof(message)); //clear old message
    memcpy(message, confMsg, confLen);
    message[confLen]= SIGFOX_END_MESSAGE;
    
    // clear the rx buffer pointer
    answer.payloadPtr=0;
    memset(answer.payload, 0 , sizeof(answer.payload));
    sfxMode = sfxConfigurationMode;
    
    SigFox.print(message);
}



byte  SmeSFX::sfxSendData(const char payload[], byte payloadLen) {
    if (SFX_MAX_PAYLOAD<payloadLen)
    return SME_EINVAL; // too long payload

    memset(message, 0 , sizeof(message)); //clear old message
    message[0] = SFX_MSG_HEADER;
    message[1] = payloadLen;
    message[2] = SFX_DATA;
    message[3] = getNewSequenceNumber();
    for (int i=0; i<payloadLen; i++) {
        message[4+i] = payload[i];
    }

    // store the message Id that will be sent
    sfxMessageIdx[DATA_SEQUENCE_NUMBER] = message[3];

    byte  crcLen= insertCRC(&message[4+payloadLen], payload, SFX_DATA, message[3], payloadLen);
    message[4+payloadLen+crcLen] = SFX_MSG_TAILER;


    // reset answer Fsm and Ackoledge
    dataAck = SFX_DATA_ACK_START;
    recFsm=headerRec;
    
    // clear the rx buffer pointer
    answer.payloadPtr=0;
    memset(answer.payload, 0 , sizeof(answer.payload));
    
    SigFox.print((const char*)message);
    
    return 1; // just return true in first library releae
}


// TODO Da Fare
void  SmeSFX::sfxSendKeep(void) {

    memset(message, 0 , sizeof(message)); //clear old message
    message[0] = SFX_MSG_HEADER;
    message[1] = 1;
    message[2] = SFX_KEEP;
    message[3] = getNewSequenceNumber();
    
    // clear the rx buffer pointer
    answer.payloadPtr=0;
    memset(answer.payload, 0 , sizeof(answer.payload));

    // store the message Id that will be sent
    /*sfxMessageIdx[KEEP_SEQUENCE_NUMBER] = message[3];

    byte  crcLen= insertCRC(&message[4+payloadLen], payload, SFX_DATA, message[3], payloadLen);
    message[4+payloadLen+crcLen] = SFX_MSG_TAILER;*/

    SigFox.print((const char*)message);
}


word SmeSFX::calculateCRC(byte payloadLen, byte msgType, byte seqNumber, const char *payload){
    word crc = payloadLen;
    crc += msgType;
    crc += seqNumber;
    for(int i=0; i<payloadLen; i++){
        crc+=payload[i];
    }

    return crc;
}

// private functions
byte SmeSFX::insertCRC(char *crcPos, const char *payload, byte msgType, byte seqNumber, byte payloadLen) {
    word crc = calculateCRC(payloadLen, msgType, seqNumber, payload);


    char* tmp = (char*)&crc;
    crcPos[0] = tmp[0];
    crcPos[1] = tmp[1];
    return 2;
}

sfxRxFSME SmeSFX::crcCheck(void) {
    word crc = calculateCRC(answer.length, answer.type,
    answer.sequenceNumber, (const char*)answer.payload);
    word *receivedCrc=(word *)answer.crc;

    if (*receivedCrc == crc) {
        return tailerRec;
    }
    else {
        sfxError = SME_SFX_KO;
        return nullState;
    }
}


byte SmeSFX::composeSfxDataAnswer(char data) {

    // by default consider the char is OK
    sfxError = SME_SFX_OK;

    switch (recFsm) {
        case headerRec:
        dataAck = SFX_DATA_ACK_PROCESSING;
        if (SFX_MSG_HEADER != data) {
            sfxError = SME_SFX_KO;
            return sfxError;
        }
        else
        recFsm = lenRec;
        break;

        case lenRec:
        answer.length = data;
        recFsm = typeRec;
        break;

        case typeRec:
        answer.type  = data;
        recFsm = sequenceRec;
        break;

        case sequenceRec:
        recFsm = checkSequenceConsistence(data);
        break;

        case payloadRec:
        answer.payload[answer.payloadPtr++]= data;
        if (answer.payloadPtr == answer.length) {
            recFsm = check_msg_error();
        }
        break;

        case crcRec:
        answer.crc[answerCrcCounter++] = data;
        if (answerCrcCounter == 2)
        recFsm = crcCheck();
        break;

        case tailerRec:
        // remove the charged timeout
        //stopSfxCommandTimer();

        recFsm = headerRec;
        if (SFX_MSG_TAILER == data){
            dataAck = SFX_DATA_ACK_OK;
            sfxError = SME_SFX_OK;
        } else
        sfxError = SME_SFX_KO;
        break;

        case nullState:
        // enter here in case of CRC error or sequence error
        // remove all the last incoming data
        // remove the charged timeout
        //stopSfxCommandTimer();
        dataAck = SFX_DATA_ACK_KO;
        break;
    }

    return sfxError;
}


sfxRxFSME SmeSFX::checkSequenceConsistence(byte sequence) {

    for(int i=0; i<MAX_MESSAGE_OUT; i++ ) {
        if (sfxMessageIdx[i] == sequence){
            answer.sequenceNumber = sequence;
            answer.payloadPtr = 0;

            return payloadRec;
        }
    }
    
    sfxError = SME_SFX_KO;
    return nullState;
}
/*
* If Telit SFX return an error state different from TELIT_SFX_NO_ERROR
* discharge all the other incoming bytes and exit with error (LED RED)
*/
sfxRxFSME SmeSFX::check_msg_error(void) {

    // only in case of SFX answer for a sent message check the error
    if ((answer.type & SFX_CONFIRM ) == SFX_CONFIRM) {
        if (answer.payload[0]!=TELIT_SFX_NO_ERROR){
            sfxError = SME_SFX_KO;
            return nullState;
        }
    }

    answerCrcCounter =0;
    return crcRec;

}

byte  SmeSFX::composeSfxConfigurationAnswer(char data){
    // store all the received bye till the end of message
    if (data != SIGFOX_END_MESSAGE) {
        answer.payload[answer.payloadPtr++]= data;
        } else {
        // remove the charged timeout
        //stopSfxCommandTimer();
        
        // end found check if the answer is ok, just check the first Byte
        byte msgOk = SGF_CONF_ERROR != answer.payload[0];
        if (msgOk) {
            // if the status was configured to enter in data mode
            // adjust here the new status
            if (sfxMode == sfxEnterDataMode) {
                sfxMode = sfxDataMode;
            }
            sfxError = SME_SFX_OK;
            } else {
            sfxError = SME_SFX_KO;
        }
        
        return sfxError;
    }

    return SME_OK;
}

byte  SmeSFX::readSwVersion(char swVer[]) {
    
    // if it already loaded return it immediately
    if (swVerLength ==0) {
    
    // get the S/N
    byte exit = 0;
    // if is not in configuration Mode move it
    if (sfxMode != sfxConfigurationMode)
        setSfxConfigurationMode();
        
    do {
        if (sfxAntenna.hasSfxAnswer()) {
            exit++; // move the FSM
            if (exit ==1){
                sfxSendConf(FW_SW_VERSION, sizeof(FW_SW_VERSION)-1);
            }
        }
    } while(exit<2);
    
    this->swVerLength = answer.payloadPtr;
    if (this->swVerLength>SN_LENGTH)
    this->swVerLength = SN_LENGTH;
    
    memcpy(this->swVer, getLastReceivedMessage(), this->swVerLength);    
    }

    memcpy(swVer, this->swVer, this->swVerLength); 

    return this->swVerLength;
}

SmeSFX  sfxAntenna;
