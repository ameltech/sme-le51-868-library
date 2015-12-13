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
#define RESET_FAB_MODE      "ATR"

#define ENTER_DATA_MODE     "ATX\r"

/*
Register modification
‘ATSn=m’ command configures Hayes register number n with the value
m, e.g. ATS200=4<CR> enters the value ‘4’ in the register S200.
The value is automatically stored
*/
#define CONF_REGISTER       "ATS"

/*
Switch to Bootloader
‘ATBL’ command escape from the main program and run the
bootloader. This command is useful to update the firmware by serial or
radio link.
*/
#define FW_BOOTLOADER		"ATBL"


#define SFX_DATA_ACK "AT$SF"
#define WITH_ACK      ",1"
#define WITH_NO_ACK   ",0"
#define SFX_DOWNLINK_PAYLOAD 7 //eg: OK/r+RX=3132333435363738/r+RX END/r
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


// ---------- BaudRate Register -------------------
#define SFX_BAUDRATE_REG    "ATS210"
/*
1. Serial baud rate : S210
This register selects the serial baud rate value. It is linked to the time-out register S214. They
can be set with the following values:
*/
typedef enum {
    BNOBaudRate,
    B1200,
    B2400,
    B4800,
    B9600,
    B19200,
    B38400,
    B57600,
    B115200
}SfxBaudE;
// ---------- BaudRate Register -------------------


// ---------- SerialNum Register -------------------
#define GET_SN   "ATS192?"
#define GET_SN_LEN (sizeof(GET_SN)-1)
// ---------- SerialNum Register -------------------


// ---------- PowerSafe Register -------------------
/*NOTE:
When a receiver runs Wake on Radio, the sender must send radio frames with very long
preamble in order to increase the probability of detection by the sleeping receiver. The
higher probability is reached when the preamble of sender is equal to the sleep duration of
the sleeping receiver. For example, if radio data rate is 38,4 kb/s, and if the sleeping device
is set to wakeup every 1000 ms, you should consider to set the preamble length (S204
register) to [38,4 x 1000]/8 = 4800 bytes of preamble on sender side.
WARNING:
Due to some internal limitations, when Wake on Radio is used, the maximum frame length
is limited to 210 bytes in Addressed Secured mode.
In Transparent mode, when Wake on Radio is used the maximum frame length depends on
the combination of radio baud rate and serial speed, and decreases when using higher radio
baud rate and lower serial speed values; in the worst case (radio baud rate set to 115.2 Kbps
and serial speed set to 1200 bps) it is limited to 126 bytes.

Wake on Radio can not be enabled alone; otherwise there is a risk to never take back
control of the sleeping module. Wake on Radio is enabled only in combination with Hard
stand by mode ensuring to always have a chance to awake the module by stand-by pin.
*/
#define SFX_CFG_WAKE_ADDR   "ATS240"#define SFX_NO_WAKE         '0'#define SFX_HW_WAKE         '1'#define SFX_SW_WAKE         '2'#define SFX_RADIO_WAKE      '4'#define SFX_ERROR_WAKE      0xFF

/*Stand By Activation
When serial stand by is set, the ‘ATP’ command put the module in stand
by mode. To wake up the module, send a NULL (0x00) character. */
#define SFX_SW_SLEEP_ACTIVATION "ATP"
// ---------- PowerSafe Register -------------------


#define SIGFOX_END_READ     '?'
#define SIGFOX_EQUAL_CHAR   '='
#define SIGFOX_END_MESSAGE  0xd

#define SFX_BTL_MSG_LEN     12
#define SFX_BTL_ACK_OK      0
#define SFX_BTL_COMMAND_ERROR          0x01 // The bootloader is not in flashing mode and cannot accept this command
#define SFX_BTL_Command_Error_Frame    0x02 // The command has a wrong length 
#define SFX_BTL_Command_Error_CRC      0x03 // An error occurred verifying CRC 
#define SFX_BTL_Command_Error_Address  0x04 // The command has a wrong address 
#define SFX_BTL_Command_Error_Order    0x05 // The command ID is wrong 
#define SFX_BTL_Flash_Erase_Error      0x06 // An error occurred after an erase command 
#define SFX_BTL_Flash_Blankcheck_Error 0x07 // An error occurred verifying blankcheck after an erase command
#define SFX_BTL_Flash_Write_Error      0x08 // An error occurred after a write command 
#define SFX_BTL_Flash_Verify_Error     0x09 // Data written in the flash are not equal to those received in the write command 
#define SFX_BTL_Flash_Address_Error    0x0A // The flash address held in the Address field is outside the range

#define SFX_MSG_HEADER      0xA5
#define SFX_MSG_TAILER      0x5A#define SFX_HEADER_POS 0#define SFX_SEQUENCE_POS 3
#define SGF_CONF_OK         'O' // OK
#define SGF_CONF_ERROR      'E' // ERROR

#define SFX_DATA     0x01
#define SFX_KEEP     0x02
#define SFX_BIT      0x03

#define SFX_CONFIRM  0x10

#define SFX_DATA_PAYLOAD 0x0C


// BTL definitions
// BTL Mode Commands
#define ENTER_FLASH 0x11
#define ERASE_FLASH 0x22
#define FW_DOWNLOAD 0x33
#define FW_DOWNLOAD_ENCRYPTED 0x34
#define CRC_COMPARE 0x66
#define EXIT_FLASH  0x55


#define DUMMY_VALUE 0xAA
// END BTL definitions

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

#define ACK_TIMEOUT_20SEC 20000 
#define SFX_CONFIGURATION_TIMEOUT_1SEC 1000

#endif /* SME_MODEL_SIGFOX_H_ */

