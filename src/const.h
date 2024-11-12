//------------------------------------------------------------------------------
// File: Const
//
// Purpose: Project constants.
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef CONST_H
#define CONST_H

#ifdef DEBUG
    #define TRACE(char) Trace(char);
#else
    #define TRACE(char) 
#endif

// software revision
#define SOFTWARE_REV            0x08

#define MAX_ECU				8 	// per ISO 15675-4 7.3.3. max ECUs is 8

#define DEVID1_ADDR 0x3FFFFEL
#define DEVID2_ADDR 0x3FFFFFL 

// The old 18F248 and the new 18F2480 have some differences, mostly in the analog
// to digital converter. Using _DEVID2 check the processor version. 
#define VER_18F248				0x08
//#define VER_18F2480 					

// destination address
#define DESTINATION_ADDR        0x2d

// from host msg byte positions
#define IDX_HOST_DESTINATION        0
#define IDX_HOST_COMMAND            1
#define IDX_HOST_BYTE_COUNT         2
#define IDX_HOST_START_DATA         3
#define IDX_HOST_CHECKSUM           14

// to host msg byte positions
#define IDX_VEHICLE_DESTINATION     0
#define IDX_VEHICLE_COMMAND         1
#define IDX_VEHICLE_CHECKSUM        13

// max size of all messages
#define HOST_RECV_MSG_SIZE          15      // from host size including all non-OBD II bytes
#define HOST_SEND_MSG_SIZE          14      // to host size
#define VEHICLE_MSG_SIZE            12      // to/from max vehicle size, not as sent to host (RSP byte not included) 
#define VEHICLE_MAX_MSGS            32      // number of VEHICLE_MSG_SIZE we can store in RAM

// error codes returned to host
#define ERR_NONE                        0x00   // ERR_NONE must be 0!
#define ERR_NOT_IMPLEMENTED             0x01
#define ERR_BAD_CHECKSUM                0x02
#define ERR_ISO_NO_SYNC                 0x03
#define ERR_ISO_INCORRECT_INV_ADDR      0x04
#define ERR_ISO_NO_RESPONSE             0x05
#define ERR_J1850_NO_RESPONSE           0x07
#define ERR_ISO_BAD_CHECKSUM            0x08
#define ERR_J1850_BAD_CRC               0x09    
#define ERR_KWP_BAUD_TOO_SLOW           0x0b    // not used
#define ERR_KWP_NO_RESPONSE             0x0c    // not used
#define ERR_KWP_INCORRECT_SYNC          0x0d    // not used
#define ERR_ISO_NOT_ENABLED             0x10    // not used
#define ERR_J1850_VPW_NOT_ENABLED       0x11    // not used
#define ERR_J1850_PWM_NOT_ENABLED       0x12    // not used
#define ERR_KWP_NOT_ENABLED             0x13    // not used
#define ERR_VW_PASS_THRU_NOT_ENABLED    0x14    // not used
#define ERR_CAN_NO_RESPONSE             0x15	// COMSTAT CAN register returned as extra data
#define ERR_CAN_RX_OVERFLOW             0x16
#define ERR_CAN_TX_BUFFER_UNAVAIL       0x18
#define ERR_CAN_RX_MESSAGE_LEN_WRONG    0x19
#define ERR_CAN_ERROR_INTERRUPT			0x1A	// COMSTAT CAN register returned as extra data
#define ERR_CAN_INVALID_MSG_INTERRUPT   0x1B    // TXBnCON CAN register returned as extra data 1, 2 and 3
#define ERR_ISO_INV_ADDR_NOT_RECEIVED	0x1F
#define ERR_ISO_SYNC_BYTE_NOT_RECEIVED	0x20

#define ERR_WRONG_DESTINATION           0x80    
#define ERR_BYTE_COUNT_TOO_LARGE        0x81
#define ERR_J1850_FRAMING_ERROR         0x82
#define ERR_J1850_MSG_OVERFLOW          0x83 
#define ERR_ISO_NO_KEYBYTE              0x84
#define ERR_ADAPTER_LOCKED              0x85
#define ERR_INVALID_NON_VOLATILE_SECTOR 0x86
#define ERR_NON_VOLATILE_SECTOR_LOCKED  0x87

// host-to-adapter commands
#define CMD_ISO_WITH_5_BAUD             0x01
#define CMD_VPW                         0x02
#define CMD_PWM                         0x04
#define CMD_CAN_SI_250KB                0x08    
#define CMD_ISO                         0x10
#define CMD_VW_PASS_THRU_WITH_5_BAUD    0x20    // not used
#define CMD_VW_PASS_THRU_WITHOUT_5_BAUD 0x40    // not used
#define CMD_PWM_WITH_IFR                0x80
#define CMD_ISO_1ST_RESPONSE            0x81
#define CMD_VPW_1ST_RESPONSE            0x82
#define CMD_ATOD_CONVERSION             0x83    // not used
#define CMD_PWM_1ST_RESPONSE            0x84
#define CMD_KWP_WITH_5_BAUD             0x85
#define CMD_KWP_REPORT_KW1_KW2          0x86    // not used
#define CMD_KWP_FAST_INIT               0x87
#define CMD_KWP                         0x88
#define CMD_KWP_1ST_RESPONSE            0x89
#define CMD_CAN_SI_500KB                0x90
#define CMD_CAN_EI_250KB                0x91
#define CMD_CAN_EI_500KB                0x92
#define CMD_CAN_SI_250KB_1ST            0x93
#define CMD_CAN_SI_500KB_1ST            0x94
#define CMD_CAN_EI_250KB_1ST            0x95
#define CMD_CAN_EI_500KB_1ST            0x96
#define CMD_CAN_SET_STD_MASK_FILTER		0x97
#define CMD_CAN_SET_XTD_MASK_FILTER		0x98
#define CMD_CAN_SET_P2_CAN_TIMEOUT		0x99
#define CMD_CAN_SI_250KB_FBGO			0x9A	// FBGO = fast bus get off
#define CMD_CAN_SI_500KB_FBGO			0x9B	// FBGO = fast bus get off
#define CMD_CAN_EI_250KB_FBGO			0x9C	// FBGO = fast bus get off
#define CMD_CAN_EI_500KB_FBGO			0x9D	// FBGO = fast bus get off
#define CMD_CAN_SI_250KB_LISTEN			0x9E	
#define CMD_CAN_SI_500KB_LISTEN			0x9F	
#define CMD_CAN_EI_250KB_LISTEN			0xA0	
#define CMD_CAN_EI_500KB_LISTEN			0xA1
#define CMD_CAN_SI_250KB_NO_RESPONSE	0xA2	
#define CMD_CAN_SI_500KB_NO_RESPONSE	0xA3	
#define CMD_CAN_EI_250KB_NO_RESPONSE	0xA4	
#define CMD_CAN_EI_500KB_NO_RESPONSE	0xA5
#define CMD_ISO_NO_RESPONSE				0xA6
#define CMD_ISO_ENABLE_L_LINE_XMIT		0xA9
#define CMD_ISO_SET_9600_BAUD			0xAB
#define CMD_ISO_CHECKSUM_ENABLE			0xAC
#define CMD_ISO_5MS_BYTE_DELAY_ENABLE	0xAD
#define CMD_UNLOCK_ADAPTER              0xB0	
#define CMD_READ_EEPROM					0xB9
#define CMD_WRITE_EEPROM				0xBA
#define CMD_KWP_FAST_INIT_9600			0xC4
#define CMD_READ_BATTERY_VOLTAGE		0xF0

// adapter-to-host commands
#define CMD_ERROR                       0x80
#define CMD_ISO_MSG                     0x81
#define CMD_VPW_MSG                     0x82
#define CMD_PWM_MSG                     0x84
#define CMD_CAN_MSG                     0x88
#define CMD_KWP_MSG                     0x01
#define CMD_LOCAL_DATA					0x40

// transmit modes (must fit in 3-bits!)
#define MODE_ISO                 0x00
#define MODE_VPW                 0x01
#define MODE_PWM                 0x02
#define MODE_KWP                 0x03
#define MODE_CAN                 0x04

// interface capabilites (i.e. what com modes does the adapter support)
#define CAP_ISO         0x01
#define CAP_VPW         0x02
#define CAP_PWM         0x04
#define CAP_KWP         0x08
#define CAP_CAN         0x10
#define CAP_DUAL_CAN	0x20
#define DASH_DYNO_VER	0x40
#ifdef DASH_DYNO
#define CAP_INTERFACE   (CAP_ISO | CAP_VPW | CAP_PWM | CAP_KWP | CAP_CAN | DASH_DYNO_VER)
#else
#define CAP_INTERFACE   (CAP_ISO | CAP_VPW | CAP_PWM | CAP_KWP | CAP_CAN)
#endif

#endif 
