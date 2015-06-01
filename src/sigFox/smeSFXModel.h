/*
 * smeSFXModel.h
 *
 * Created: 4/27/2015 9:48:13 AM
 * by Mik (smkk@amel-tech.com)
 *
 */ 


#ifndef SMESFXMODEL_H_
#define SMESFXMODEL_H_

#define SIGFOX_REGISTER_READ  'R'
#define SIGFOX_REGISTER_WRITE 'W'
#define SIGFOX_FACTORY_RESET  'F'

/*
Hayes Mode Activation
‘+++’ command gives an instant access to the modem’s parameters
configuration mode (Hayes or AT mode), whatever the current operating
mode in process might be.
‘+++’ command should be entered as one string, i.e. it should not be
preceded by ‘AT’ and followed by <CR> but two silent times whose
duration is configurable via S214 register (Serial time-out). The time
between two ‘+’ must not exceed the time-out value.
Hayes mode inactivates radio functions.
*/
#define ENTER_CONF_MODE     "+++"


/*
Parameters reset
‘ATR’ comman
*/
#define RESET_FAB_MODE      "ATR\r"

#define ENTER_DATA_MODE     "ATX\r"

/*
Register modification
‘ATSn=m’ command configures Hayes register number n with the value
m, e.g. ATS200=4<CR> enters the value ‘4’ in the register S200.
The value is automatically stored
*/
#define CONF_REGISTER       "ATS"

/*
Modem’s firmware version
‘AT/V’ command displays the modem’s firmware version as follows:
pp.XCR.MM.mm-Bbbb<CR>pp.B00.NN.nn
With:
pp is the platform:
· GC: LE50-868
· GD: LE50-433
· GJ: LE70-868
XCR: X is the stack (S for Star Network and B for Bootloader).
C is the casing:
· 0: OEM boards
· 1: USB dongle or IP67 box
R is reserved and always read as ‘0’
MM: Major version of firmware
mm: minor version of firmware
bbb: build number of firmware
NN: major version number of bootloader
Nn: minor version number of bootloader
*/
#define FW_SW_VERSION		"AT/V"


#define SIGFOX_END_READ     '?'
#define SIGFOX_EQUAL_CHAR   '='
#define SIGFOX_END_MESSAGE  0xd

#define SFX_MSG_HEADER      0xA5
#define SFX_MSG_TAILER      0x5A#define SFX_HEADER_POS 0#define SFX_SEQUENCE_POS 3
#define SGF_CONF_OK         'O' // OK
#define SGF_CONF_ERROR      'E' // ERROR

#define SFX_DATA     0x01
#define SFX_KEEP     0x02
#define SFX_BIT      0x03

#define SFX_CONFIRM  0x10

#define SFX_DATA_PAYLOAD 0x0C


/* see
http://www.adaptivemodules.com/assets/telit/Telit-LE51-868-S-Software-User-Guide-r1.pdf
 *
 * In order to get a register value, the user shall send:
 * ATSxxx?< cr>, where ‘xxx’ is the register address.
 * The answer will be: Sxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the
 * register value (up to 255, it depends to the available values). *
 * In order to set a new register value, the user shall send:
 * ATSxxx=yy<cr>, where ‘xxx’ is the register address and ‘yy’ the register value (up
 * to 255, it depends to the available values).
 */

typedef enum
{
    TELIT_SFX_NO_ERROR = 0,
    TELIT_SFX_SERIAL_LENGTH_ERROR,
    TELIT_SFX_SERIAL_CRC_ERROR,
    TELIT_SFX_SERIAL_TAILER_MISS_ERROR,
    TELIT_SFX_SERIAL_TIMEOUT_ERROR,
    TELIT_SFX_SERIAL_GENERIC_ERROR,
    TELIT_SFX_SFX_INIT_ERROR,
    TELIT_SFX_SFX_SEND_ERROR,
    TELIT_SFX_SFX_CLOSE_ERROR} SFX_ACK;

#define SIG_FOX_MAX_REGISTER_LEN 0x6
#define SIG_FOX_MAX_REG_VALUE_LEN 0x10


#define SFX_MAX_PAYLOAD 0xff


#endif /* SME_MODEL_SIGFOX_H_ */

