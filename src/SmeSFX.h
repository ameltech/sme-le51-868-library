/*
 * SmeSFX.h
 *
 *  Created on: Apr 26, 2015
 * by Mik (smkk@amel-tech.com)
 * 
 * License Information
 * -------------------
 *
 * Copyright (c) Amel Technology. All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SMESFX_H_
#define SMESFX_H_

#include <stdint-gcc.h>
#include <Arduino.h>

#include "sigFox/smeSFXModel.h"
#include "sigFox/smeErrorCode.h"



#define SN_LENGTH           20 // max digit for SN

/*
Header  1   0xA5
Length  1   0x00 – 0xFF It’s the payload length
Type    1   DATA - 0x01: simple data
            KEEP - 0x02: used to send a keep alive message as described
                         in the SIGFOX standard; it’s in charge to the user to send
                         the keep alive message every 24 hours
            BIT - 0x03:  : send a status bit following the SIGFOX protocol
            CONFIRM - 0x10
Sequence Number 1 0x01 – 0xFF It’s in charge to user to put a valid value to recognize 
                                the association packet send confirm received
Payload     0x00 – 0xFF     Up to customer DATA: up to 255 bytes
KEEP: must be omitted
BIT: one byte with value 0
or 1
CRC 2 Up to customer It’s the sum of all bytes
except header and tailer (lsb
first)
Tailer 1 0x5A

Example: dataMsg  "HELLO"  = A5 05 01 25 48 65 6C 6C 6F 1F 02 5A

*/


typedef enum {
	headerRec,
	lenRec,
	typeRec,
	sequenceRec,
	payloadRec,
	crcRec,
	tailerRec,
	nullState // used when gotcha an error
} sfxRxFSME;



typedef enum {
	sfxConfigurationMode,
	sfxEnterDataMode,
	sfxDataMode
} sigFoxModeE;


#define SFX_ANSWER_LEN 6 // considering the max payload that could be ERROR in Conf mode

// only one KEEP and one DATA message could be sent simultaneously
#define MAX_MESSAGE_OUT 2 // current limit is for 2 message on the air



#define SFX_DATA_ACK_START      0
#define SFX_DATA_ACK_PROCESSING 1
#define SFX_DATA_ACK_OK         2
#define SFX_DATA_ACK_KO         3

/*
Structure that define the Telit Answer msg
 
*/
typedef struct {
	byte payloadPtr;	 // pointer to the payload  messageg
	byte length;		 // length of the payload message
	byte type;			 // type of the message with the answer bit set (0x10)
	byte sequenceNumber; //0x1 to 0xff
	byte payload[SFX_ANSWER_LEN];
	byte crc[2];	     // two byte of CRC
}sigFoxRxMessage;


class SmeSFX{
public:
    SmeSFX(void);
    virtual ~SmeSFX(){};
    void begin (void);

private:
    byte         sfxSequenceNumber;
    sigFoxModeE     sfxMode;
    char            message[SFX_MAX_PAYLOAD+7]; // the max payload plus header,tailer CRC and other bytes
    sigFoxRxMessage answer;                     //structure used to received message/configuration
    byte         sfxError;                      // internal error code 
    sfxRxFSME    recFsm;					    // Finite State machine for the parsing of the data answer    
    byte         answerCrcCounter;
    byte         dataAck;
    byte         swVer[SN_LENGTH];
    byte         swVerLength;
    
    // keep track of the messageId are active
    byte sfxMessageIdx[MAX_MESSAGE_OUT];

    const byte readSfxAnswer(void);   
    byte       getNewSequenceNumber(void){return sfxSequenceNumber++;};
    byte       composeSfxConfigurationAnswer(char data);
    byte       composeSfxDataAnswer(char data);
    sfxRxFSME  crcCheck(void);
    byte       insertCRC(char *crcPos, const char *payLoad, byte msgType, byte seqNumber, byte payloadLen); 
    word       calculateCRC(byte payloadLen, byte msgType, byte seqNumber, const char *payload);
    sfxRxFSME  check_msg_error(void);
    sfxRxFSME  checkSequenceConsistence(byte sequence);


    // library API
public:

	/*
     * \brief Return the S/N of the Antenna
     *
	 * \param [in,out] <serialNum> {The String of the Serial Number}
     * 
	 * \return the length of the Serial Number
	 *
     */
	byte  readSwVersion(char swVer[]);
	
	/*
     * \brief Return last message received from the Antenna.
	 *			message could be from the network or from the configuration
	 *			depending of the result of getSfxMode()
     *
	 *  
	 * \return the last 
	 *
     */
    const byte* getLastReceivedMessage(void) { return answer.payload;};
        
    byte      getSfxError() const { return sfxError;};

    /*
     * \brief To set the chip in configuration mode
     *
     */
    void    setSfxConfigurationMode(void);

    /*
     * \brief To set the chip in data sending mode
     *
     */
    void    setSfxDataMode(void);

    /*
     * \brief The function sends a message over the SFX network
     *
     * \param [in] <payload>    {the user data want to send on the SigFox network}
     * \param [in] <payloadLen> {the length in bytes of the user data}
     *
     */
    byte sfxSendData(const char payload[], byte payloadLen);


    /*
     * \brief The function sends the configuration message to SFX
     *
     * \param [in] <confMsg>    {the configuration messagek}
     * \param [in] <payloadLen> {the length in bytes of the user data}
     *
     */
    void sfxSendConf(const char confMsg[], byte confLen);
    
    /*
     * \brief used to send the KEEP message
     *
     */
    void sfxSendKeep(void);

    /*
     * \brief The function inform if a SigFox answer is ready
     *
     * \param void
     *
     * \return  <true>  {a message is ready}
     * \return  <false> {no message is ready or error accourred}
     */
    bool    hasSfxAnswer(void);



    /*
     * \brief To known the status of the chip
     *
     * \return on of the value of sigFoxModeE
     */
    sigFoxModeE getSfxMode() const {
        return sfxMode;
    }

    /*
     * \brief To known about the status of the receiving data acknoledge
     *
     * \param vod
     *
     * \return one of the possible status of the Finite State Machine that handle the acknoledge message from the Chip
     *
     */
    uint8_t    sfxDataAcknoledge(void){return dataAck;}
};

// external variable used by the sketches
extern SmeSFX  sfxAntenna;


#endif /* SMESFX_H_ */