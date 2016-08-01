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
    this->swVer[0]=0;
    this->sn[0] = 0;
    sfxSequenceNumber = 0x25;
    sfxMode = sfxDataMode; 
    recFsm	= headerRec;
    sleepMode = SFX_ERROR_WAKE;
}

void SmeSFX::begin (unsigned long baudRate, Uart *_antenna) {
    this->antenna = _antenna;
    antenna->begin(baudRate);
}

void SmeSFX::setSfxConfigurationMode(void) {
    antenna->write(ENTER_CONF_MODE, sizeof(ENTER_CONF_MODE)-1);
    answer.payloadPtr=0;
    sfxMode = sfxConfigurationMode;
}

void SmeSFX::setSfxDataMode(void) {
    antenna->print(ENTER_DATA_MODE);
    answer.payloadPtr=0;
    sfxMode = sfxEnterDataMode;
}

bool SmeSFX::hasSfxAnswer(void) {
    if (readSfxAnswer()) {
        return ((SME_SFX_OK==getSfxError()) || (SME_SFX_KO==getSfxError())); // return in case of OK or ERROR
    }
    return false;
}

// PRIVATE FUNCTIONS
const byte SmeSFX::readSfxAnswer(void)
{
    while (antenna->available()) {
        // get the new byte:
        char inChar = (char)antenna->read();
        
        switch(sfxMode) {
            case sfxConfigurationMode:
            case sfxEnterDataMode:
            case sfxEnterBtlMode:
                return composeSfxConfigurationAnswer(inChar);
            break;

            case sfxDataMode:
                return composeSfxDataAnswer(inChar);
            break;

            case sfxBtlMode:
                return composeSfxBtlAnswer(inChar);
            break;

            case sfxReceiveMode:
                return composeAckAnswer(inChar);
            break;

            default:
                return false;
            break;
        }
    }
    return false;
}

void SmeSFX::prepareSFXForNewMsg(void){
    //clear old message
    memset(message, 0 , sizeof(message)); 
    
    // clear the rx buffer pointer & message
    answer.payloadPtr=0;
    memset(answer.payload, 0xa5 , sizeof(answer.payload));
}
    
void SmeSFX::sfxSendBtlPage(const char btlCmdMsg[], uint16_t btlCmdMsgLen) {       
    prepareSFXForNewMsg();
    sendSFXMsg(btlCmdMsg, btlCmdMsgLen);
}
    
void  SmeSFX::sfxSendConf(const char confMsg[], byte confLen) {
    prepareSFXForNewMsg();
    
    // write the command on the message
    memcpy(message, confMsg, confLen);
    message[confLen]= SIGFOX_END_MESSAGE;
    
    sfxMode = sfxConfigurationMode;
    sendSFXMsg(message, strlen(message));
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
    
    // the 4 is due by the Header+payloadLen+DataType+Sequence
    message[4+payloadLen+crcLen] = SFX_MSG_TAILER;
    size_t messageLen = 4+payloadLen+crcLen +1;  // add the Tailer, too


    // reset answer Fsm and Ackoledge
    dataAck = SFX_DATA_ACK_START;
    recFsm=headerRec;
    
    // clear the rx buffer pointer
    answer.payloadPtr=0;
    memset(answer.payload, 0 , sizeof(answer.payload));
    
    // set the FSM to sending Data
    sfxMode = sfxDataMode;
    sendSFXMsg((const char*)message, messageLen);
    
    return 1; // just return true in first library release
}

byte  SmeSFX::sfxSendDataAck(const char payload[], byte payloadLen, bool ack) {
    long timeOut, startTime; //volatile only for debug
    startTime = millis();
    uint8_t answerReady;
    // the SFX chip must be in configuration mode
    if ((sfxAntenna.getSfxMode() != sfxConfigurationMode) || (sfxAntenna.getSfxMode() != sfxReceiveMode)) {
       sfxAntenna.setSfxConfigurationMode(); // enter in configuration Mode
       
       do {
           delay(5);
           timeOut = (millis()-startTime);
           if (SFX_CONFIGURATION_TIMEOUT_1SEC < timeOut)
                return -1;
       } while (!sfxAntenna.hasSfxAnswer());
       
    }
    
    prepareSFXForNewMsg();
    
    // write the command on the message
    memcpy(message, SFX_DATA_ACK, strlen(SFX_DATA_ACK)); // write command
    message[strlen(SFX_DATA_ACK)]= SIGFOX_EQUAL_CHAR;    // write '='
    memcpy(&message[strlen(SFX_DATA_ACK)+1], payload, payloadLen); //write payload
    
    if (ack) {
        memcpy(&message[strlen(SFX_DATA_ACK)+1+payloadLen], WITH_ACK, strlen(WITH_ACK));
                // the OK message need 3 /r
                // The ERROR just one
                answer.numberOfCR=2; // suppose everything goes well
    } else {
        memcpy(&message[strlen(SFX_DATA_ACK)+1+payloadLen], WITH_NO_ACK, strlen(WITH_NO_ACK));
        // the OK message need 1 /r
        answer.numberOfCR=1; // suppose everything goes well
    }        
    
    timeOut = (millis()-startTime);
    // at the end store the /r
    message[strlen(SFX_DATA_ACK)+1+payloadLen+2]= SIGFOX_END_MESSAGE;
    sfxMode = sfxReceiveMode;        
    
    sendSFXMsg(message, strlen(message));
    timeOut = (millis()-startTime);
    
    
    
    return SME_OK; // just return true in first library release
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
    sendSFXMsg((const char*)message, 4);
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

    if (((word)*receivedCrc) == crc) {
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
        recFsm = headerRec;
        dataAck = SFX_DATA_ACK_OK;
        if (SFX_MSG_TAILER == data){
            sfxError = SME_SFX_OK;
        } else
            sfxError = SME_SFX_KO;
        break;

        case nullState:
        // enter here in case of CRC error or sequence error
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
    answer.payload[answer.payloadPtr++]= data;
    
    if (data == SIGFOX_END_MESSAGE) {
        
        // in case of SWVersion skip the first 0x0d
        if (answer.numberOfCR) {
            answer.numberOfCR--;
        } else {
            // end found check if the answer is ok, just check the first Byte
            byte msgOk = SGF_CONF_ERROR != answer.payload[0];
            if (msgOk) {
                // if the status was configured to enter in data mode
                // adjust here the new status
                switch(sfxMode){
                    case sfxEnterDataMode:
                        sfxMode = sfxDataMode; // by default ready to receive messages
                    break;
                    
                    case sfxEnterBtlMode:
                        sfxMode= sfxBtlMode;
                    break;
                        
                    default:
                    break;
                }
                sfxError = SME_SFX_OK;
                } else {
                sfxError = SME_SFX_KO;
            }
        }
        return sfxError;
    }

    return SME_OK;
}


byte  SmeSFX::composeSfxBtlAnswer(char data){

    // store all the received bye till the last which is the Error Code
    if (answer.payloadPtr < SFX_BTL_MSG_LEN-1) {
        
        // the first byte to consider must be 'G'
        if ((answer.payloadPtr==0) && (data!= 'G'))
            return SME_OK;
        
        answer.payload[answer.payloadPtr++]= data; //just store the data
    } else {
        answer.payload[answer.payloadPtr]= data; // store the result code
        
        if (SFX_BTL_ACK_OK == data) {
            sfxError = SME_SFX_OK;
        } else {
            sfxError = SME_SFX_KO;
        }

        return sfxError;
    }

    return SME_OK;
}

byte SmeSFX::composeAckAnswer(char data) {
    // store all the received bye till the end of message
    answer.payload[answer.payloadPtr++]= data;
    
    if (data == SIGFOX_END_MESSAGE) {
        if (answer.numberOfCR) {
            // at the first /r check if there is an ERROR and exit
            if ((SGF_CONF_ERROR == answer.payload[0]) &&
                (answer.payloadPtr == 6)){
                sfxError = SME_SFX_KO;
                return sfxError;
            }
            
            // check if there is an OK and exit (because it has not been request the ACK
            if ((SGF_CONF_OK == answer.payload[0]) && 
                (answer.payloadPtr == 3)) {
                sfxError = SME_SFX_OK;                
                return sfxError;
            }
            
            // in this case the answer is because there is an ACK
            answer.numberOfCR--;
        } else {
            // msg OK just consider the payload of downlink
            int i;
            for (i=0; i<SFX_ANSWER_LEN; i++){
                answer.payload[i] = answer.payload[i+SFX_DOWNLINK_PAYLOAD];
                if (answer.payload[i]==SIGFOX_END_MESSAGE) {
                    answer.payload[i]=0; // substitute the SIGFOX_END_MESSAGE with a string terminator
                    answer.payloadPtr = strlen((const char*)answer.payload);
                    break;
                }
            }
                       
            sfxError = SME_SFX_OK;
            
            return sfxError;
        }
    }

    return SME_OK;
}

SfxBaudE  SmeSFX:: getBaudRate(void){
    
    // the SFX chip must be in configuration mode
    if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
        return BNOBaudRate;
    }
    
    char send[8];
    
    memcpy(send,SFX_BAUDRATE_REG,6);
    send[6]=SIGFOX_END_READ;
    sfxSendConf(send, 7);
    
    byte exit = 0;
    do {
        sfxAntenna.hasSfxAnswer();
        exit = ((sfxAntenna.getSfxError() == SME_SFX_KO) || (sfxAntenna.getSfxError() == SME_SFX_OK));
        delay(100);
    } while(!exit);
    
    int baud=0;
    if ((sfxAntenna.getSfxError() == SME_SFX_OK)){
        baud = atoi((const char*)&answer.payload[5]);
    }
    
    return static_cast<SfxBaudE>(baud);
}

bool  SmeSFX:: setBaudRate(SfxBaudE baud){
    
    // the SFX chip must be in configuration mode
    if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
        return 0;
    }
    
    char send[8];
    memcpy(send,SFX_BAUDRATE_REG,6);
    send[6]='=';
    itoa(baud, &send[7], 10);
    sfxSendConf(send, sizeof(send));
    
    byte exit = 0;
    do {
        sfxAntenna.hasSfxAnswer();
        exit = ((sfxAntenna.getSfxError() == SME_SFX_KO) || (sfxAntenna.getSfxError() == SME_SFX_OK));
        delay(100);
    } while(!exit);
    
    return(exit);
}

void SmeSFX::setSfxFactoryReset(void){
    
    // the SFX chip must be in configuration mode
    if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
        return ;
    }
    sfxSendConf(RESET_FAB_MODE, sizeof(RESET_FAB_MODE)-1);
    
    byte exit = 0;
    do {
        sfxAntenna.hasSfxAnswer();
        exit = ((sfxAntenna.getSfxError() == SME_SFX_KO) || (sfxAntenna.getSfxError() == SME_SFX_OK));
        delay(100);
    } while(!exit);
    
}

void SmeSFX::setSfxSleepMode(uint8_t wakeMode){
    
    // the SFX chip must be in configuration mode
    if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
        return ;
    }
    
    char send[8];
    memcpy(send,SFX_CFG_WAKE_ADDR,6);
    send[6]='=';
    send[7]=wakeMode;
    sfxSendConf(send, sizeof(send));
    
    byte exit = 0;
    do {
        sfxAntenna.hasSfxAnswer();
        exit = ((sfxAntenna.getSfxError() == SME_SFX_KO) || (sfxAntenna.getSfxError() == SME_SFX_OK));
        delay(100);
    } while(!exit);
    
    sleepMode = wakeMode;
}

uint8_t SmeSFX::getSfxSleepMode(void){
    if (sleepMode == SFX_ERROR_WAKE) {
                
        // the SFX chip must be in configuration mode
        if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
            return SFX_ERROR_WAKE;
        }
        
        char send[8];
        
        memcpy(send,SFX_CFG_WAKE_ADDR,6);
        send[6]=SIGFOX_END_READ;
        sfxSendConf(send, 7);
        
        byte exit = 0;
        do {
            sfxAntenna.hasSfxAnswer();
            exit = ((sfxAntenna.getSfxError() == SME_SFX_KO) || (sfxAntenna.getSfxError() == SME_SFX_OK));
            delay(100);
        } while(!exit);
        
        int baud=0;
        if ((sfxAntenna.getSfxError() == SME_SFX_OK)){
            sleepMode = answer.payload[5];
        }
    }
    return sleepMode;
}   
    
     
     
const byte*  SmeSFX::readSwVersion(void) {
    
    // the SFX chip must be in configuration mode
    if (sfxAntenna.getSfxMode() != sfxConfigurationMode) {
        return (const byte*)"";
    }
        
    // if it already loaded return it immediately
    if (this->swVer[0] ==0) {
        
        // get the S/N
        byte exit = 0;

        sfxSendConf(FW_SW_VERSION, sizeof(FW_SW_VERSION)-1);
        answer.numberOfCR=1;
        
        do {
            if (sfxAntenna.hasSfxAnswer()) {
                exit++; // move the FSM
            }
            delay(100);
        } while(exit<1);
                
        memcpy(this->swVer, getLastReceivedMessage(), SW_VERSION);
    }

    return this->swVer;
}


const byte*  SmeSFX::readSN(void) {
    
    // if it already loaded return it immediately
    if (this->sn[0] ==0) {
        
        // get the S/N
        byte exit = 0;
        // if is not in configuration Mode move it
        if (sfxMode != sfxConfigurationMode) {
            setSfxConfigurationMode();
        }

        sfxSendConf(GET_SN, GET_SN_LEN);
        
        do {
            if (sfxAntenna.hasSfxAnswer()) {
                exit++; // move the FSM
            }
            delay(100);
        } while(exit<1);
        
        memcpy(this->sn, getLastReceivedMessage(), SN_LENGTH);
    }

    return this->sn;
}

void SmeSFX::enterBtl(bool recovery) {
    // if the system is not in recovery pahse, need to move in BTL mode
    // otherwise it is already in the requested situation
    if (!recovery) {
        // move in Boot loader mode
        sfxSendConf(FW_BOOTLOADER, sizeof(FW_BOOTLOADER)-1); 
        sfxMode = sfxEnterBtlMode;
    } else {
        sfxMode = sfxBtlMode;
    }
}

void SmeSFX::sendSFXMsg(const char *buffer, size_t size) {    
    sfxError= SME_OK; // reset the answer FSM
    antenna->write(buffer, size);
}
SmeSFX  sfxAntenna;
