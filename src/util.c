//------------------------------------------------------------------------------
// File: Util
//
// Purpose: Misc utility routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#include "Util.h"

// must be a valid mask, not just any old number. Use different masks for different 
// manufacturers.
#define LFSR_MASK 0xB38C    // Softworks    		
//#define LFSR_MASK 0x9C63    // Nology

// decryption block size (not including 2-byte Lfsr pad)
#define BLOCK_SIZE 4				

// trace debug port used to view debug chars on HyperTermial 
#use rs232(baud=9600,xmit=PIN_B4,parity=n,bits=8,stream=STREAM_DEBUG)

//------------------------------------------------------------------------------
// Function Name: init_ports
// Description: Initialize all PIC ports to power-on state.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void init_ports() 
{
	set_tris_a ( 0x55 ); 	// This goes into TRISA, a 1 sets a port pin for input
							// See hardware.h for definitions of I/O pins & TRIS bits
	set_tris_b ( 0x28 );	// This goes into TRISB, a 1 sets a port pin for input
	set_tris_c ( 0xB7 );	// This goes into TRISC, a 1 sets a port pin for input
	#USE FAST_IO(a)			// forces compiler to not set port directions on every port write
	#USE FAST_IO(b)			// forces compiler to not set port directions on every port write
	#USE FAST_IO(c)			// forces compiler to not set port directions on every port write
 
	port_b_pullups(TRUE);	// disable global Port B pull up resistors 
							// set all bus outputs inactive (PWM, VPW, ISO & CAN)
	output_a(0x08);			// PWM_P_OUT RA3 = 1
							// PWM_N_OUT RA0 = 0

	output_b(0x00);			// VPW_4X	 RB0 = 0
							// VPW_OUT   RB1 = 0
							// CAN_TX	 RB2 = 0
							// ISO_K_TX  RB6 = 0
							// ISO_L_TX  RB7 = 0
}

//------------------------------------------------------------------------------
// Function Name: IsEepromSectorWriteProtected
// Description: Checks if a EEPROM sector is write protected. The write-protect
// 	bits are stored at the end of the 256 byte EEPROM range.  
// Parameters: sector - A value from 0 to 30 representing a sector number.
// Output Parameters:
// Return Values: 1 if write protected, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsEepromSectorWriteProtected(int8 sector)
{
	int32 writeProtectBits;

	writeProtectBits = ReadEepromSectorWriteProtectTable();

	// a 1 in the EEPROM bit location within the table means not-write protected, 
	// a 0 means protected
	writeProtectBits = writeProtectBits >> sector;
	if (writeProtectBits & 1)
		return 0;
	else
		return 1;
}

//------------------------------------------------------------------------------
// Function Name: ReadEepromSectorWriteProtectTable
// Description: Reads the 4-byte write-protect table. Each bit within the table
//  represents 1 sector (except the last location which is not used). Only
// 	31 sectors, so 31 bits to represent a write-protect state for each of those
//	sectors. 
// Parameters: 
// Output Parameters:
// Return Values: The write-protect table as a int32. 
//------------------------------------------------------------------------------
int32 ReadEepromSectorWriteProtectTable()
{
	int8 cnt;
	int32 writeProtectBits;
	for (cnt = 0; cnt<4; cnt++)
		*(&writeProtectBits + cnt) = read_eeprom(EEPROM_WRITE_PROTECT_TABLE_ADDR + cnt);
	return writeProtectBits;
}

//------------------------------------------------------------------------------
// Function Name: WriteEerpomSectorWriteProtectTable
// Description: Write the 4-byte write-protect table. 
// Parameters: writeProtectBits - a 32-bit integer representing the write-protect
//	sector table status.
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void WriteEerpomSectorWriteProtectTable(int32 writeProtectBits)
{
	int8 cnt;
	for (cnt = 0; cnt<4; cnt++)
		write_eeprom(EEPROM_WRITE_PROTECT_TABLE_ADDR + cnt, *(&writeProtectBits + cnt));
}

//------------------------------------------------------------------------------
// Function Name: ReadEepromLocal
// Description: Read PIC EEPROM and do not send to host. EEPROM data will be located in
//  gVehicleBuf[0][0], eVehicleBuf[0][EEPROM_SECTOR_SIZE].
//
//	For Bluetooth, EEPROM sector 0 and sector 1 PIC must be programmed with the Bluetooth 
//  address used by the adapter test fixture. The Bluetooth address is 12 bytes and is used
//  to automatically pair and connect to the Windows test fixture. This prevents having
//  to manually pair each Bluetooth adapter to the PC during production. Once production
//  is done, the test fixture software writes all FF's to this location to prevent the 
//  automatic pairing when used by the customer. The PIC programmer software is used
//  to modify the HEX file to add the unique 12 byte Bluetooth address and therefore
//  can be changed without recompiling the program.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void ReadEepromLocal(int8 sector)
{
	// put the sector in the host buf used by the ReadEeprom() function
	gHostBuf[2] = sector;
}

//------------------------------------------------------------------------------
// Function Name: ReadEeprom
// Description: Read PIC EEPROM and send to host. 
//	The PIC has 256 bytes of EEPROM. 248 bytes can be accessed by the host. The
//	space is logically broken into 31 8-byte sections. The last 8-bytes is reserved
//	for bits to define whether a section is write-protected (i.e. locked) or not.
//	31 bits are required for the locking table, 1 bit for each section. A 1 means
//	the section is locked, a 0 means not-locked. Once a section is write-protected,
//  the host cannot reverse it. 
//
//	During production, the PIC must be programmed to have the EEPROM programmed to
//	all locations 0xFF.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void ReadEeprom()
{
	int8 cnt;
	//gFlags.vehicleData = FALSE;

	// only 31 8-byte EEPROM sectors (0 through 30) can be read
	if (gHostBuf[2] > 30)  
	{
		gError = ERR_INVALID_NON_VOLATILE_SECTOR;
		return;
	}

	// add sector number being read to buffer
	gVehicleBuf[0][0] = gHostBuf[2];

	// add write-protect state to buffer
	if (IsEepromSectorWriteProtected(gHostBuf[2]))
		gVehicleBuf[0][1] = 1;
	else
		gVehicleBuf[0][1] = 0;

	// copy 8 bytes if EEPROM data into outgoing host buffer
	for (cnt = 0; cnt<EEPROM_SECTOR_SIZE; cnt++)
		gVehicleBuf[0][cnt+2] = read_eeprom((gHostBuf[2] * EEPROM_SECTOR_SIZE) + cnt);
}

//------------------------------------------------------------------------------
// Function Name: WriteEeprom
// Description: Write to EEPROM values from host. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void WriteEeprom()
{
	int8 cnt;
	int32 writeProtectBits;
	int32 bitLocation;
	//gFlags.vehicleData = FALSE;

	// only 31 8-byte EEPROM sectors (0 through 30) can be written
	if (gHostBuf[2] > 30)
	{
		gError = ERR_INVALID_NON_VOLATILE_SECTOR;
		return;
	}

	// error check 8-byte section is not locked
	if (IsEepromSectorWriteProtected(gHostBuf[2]))
	{
		gError = ERR_NON_VOLATILE_SECTOR_LOCKED;
		return;
	}

	// copy 8 bytes from host message into the appropriate EEPROM sector 
	for (cnt = 0; cnt<EEPROM_SECTOR_SIZE; cnt++)
		write_eeprom((gHostBuf[2] * EEPROM_SECTOR_SIZE) + cnt, gHostBuf[cnt+4]);

	// if host says to write-protect the EEPROM 8-byte section, then do it by
	// updating the EEPROM table. 
	if (gHostBuf[3])
	{
		writeProtectBits = ReadEepromSectorWriteProtectTable();
		bitLocation = 1;
		bitLocation = bitLocation << gHostBuf[2];
		writeProtectBits &= ~bitLocation;   // set 1 bit to 0. 0 means write-protected
		WriteEerpomSectorWriteProtectTable(writeProtectBits);	
	}

	// add sector number being wrote to buffer 
	gVehicleBuf[0][0] = gHostBuf[2];
}

//------------------------------------------------------------------------------
// Function Name: Bus_If_Init
// Description: Bus routines, common variables & timers initialization
// Parameters: timeout - the timeout in 2mS increments
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void Bus_If_Init(void) {
	LinkFlags = 0;			// clear all local flags
	LinkControl = 0;
	BusByteCntr = 0;
    MaskReg = 0x80;

	setup_timer_0( RTCC_INTERNAL | RTCC_DIV_2 );	//  intialize & start timer0 at 400ns per count
	setup_timer_2( T2_DIV_BY_16, 35, 16 );			//  intialize timer2 for 2ms rollover
}

//------------------------------------------------------------------------------
// Function Name: IsCmdCAN
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if mode is CAN, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdCAN(int8 cmd)
{
    if (cmd == CMD_CAN_SI_250KB ||
        cmd == CMD_CAN_SI_500KB ||
		cmd == CMD_CAN_EI_250KB ||
		cmd == CMD_CAN_EI_500KB ||
        cmd == CMD_CAN_SI_250KB_1ST ||
        cmd == CMD_CAN_SI_500KB_1ST ||
		cmd == CMD_CAN_EI_250KB_1ST ||
		cmd == CMD_CAN_EI_500KB_1ST ||
		cmd == CMD_CAN_SI_250KB_FBGO ||
        cmd == CMD_CAN_SI_500KB_FBGO ||
		cmd == CMD_CAN_EI_250KB_FBGO ||
		cmd == CMD_CAN_EI_500KB_FBGO ||
		cmd == CMD_CAN_SI_250KB_LISTEN ||
        cmd == CMD_CAN_SI_500KB_LISTEN ||
		cmd == CMD_CAN_EI_250KB_LISTEN ||
		cmd == CMD_CAN_EI_500KB_LISTEN ||
		cmd == CMD_CAN_SI_250KB_NO_RESPONSE ||
        cmd == CMD_CAN_SI_500KB_NO_RESPONSE ||
		cmd == CMD_CAN_EI_250KB_NO_RESPONSE ||
		cmd == CMD_CAN_EI_500KB_NO_RESPONSE)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdFBGO
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if mode is FBGO (fast bus get off), 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdFBGO(int8 cmd)
{
	if (cmd == CMD_CAN_SI_250KB_FBGO ||
        cmd == CMD_CAN_SI_500KB_FBGO ||
		cmd == CMD_CAN_EI_250KB_FBGO ||
		cmd == CMD_CAN_EI_500KB_FBGO)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdListen
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if mode is listen only, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdListen(int8 cmd)
{
	if (cmd == CMD_CAN_SI_250KB_LISTEN ||
        cmd == CMD_CAN_SI_500KB_LISTEN ||
		cmd == CMD_CAN_EI_250KB_LISTEN ||
		cmd == CMD_CAN_EI_500KB_LISTEN)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdNoResponse
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if mode is no response type, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdNoResponse(int8 cmd)
{
	if (cmd == CMD_CAN_SI_250KB_NO_RESPONSE ||
        cmd == CMD_CAN_SI_500KB_NO_RESPONSE ||
		cmd == CMD_CAN_EI_250KB_NO_RESPONSE ||
		cmd == CMD_CAN_EI_500KB_NO_RESPONSE ||
		cmd == CMD_ISO_NO_RESPONSE)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdStandardId
// Description:	Is the CAN command a standard identifier?
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if command is standard, else extended.
//------------------------------------------------------------------------------
int8 IsCmdStandardId(int8 cmd)
{
    // is this a standard identifier message?
    if (cmd == CMD_CAN_SI_250KB ||
        cmd == CMD_CAN_SI_500KB ||
        cmd == CMD_CAN_SI_250KB_1ST ||
        cmd == CMD_CAN_SI_500KB_1ST ||
		cmd == CMD_CAN_SI_250KB_FBGO ||
        cmd == CMD_CAN_SI_500KB_FBGO ||
		cmd == CMD_CAN_SI_250KB_LISTEN ||
        cmd == CMD_CAN_SI_500KB_LISTEN ||
		cmd == CMD_CAN_SI_250KB_NO_RESPONSE ||
        cmd == CMD_CAN_SI_500KB_NO_RESPONSE)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmd250kb
// Description:	Is the CAN command a 250kb/sec CAN message?
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if command is 250kb, else 500kb or non-CAN command.
//------------------------------------------------------------------------------
int8 IsCmd250kb(int8 cmd)
{
    // is this a 250kb message?
    if (cmd == CMD_CAN_SI_250KB ||
		cmd == CMD_CAN_EI_250KB ||
        cmd == CMD_CAN_SI_250KB_1ST ||
		cmd == CMD_CAN_EI_250KB_1ST ||
		cmd == CMD_CAN_SI_250KB_FBGO ||
		cmd == CMD_CAN_EI_250KB_FBGO ||
		cmd == CMD_CAN_SI_250KB_LISTEN ||
		cmd == CMD_CAN_EI_250KB_LISTEN ||
		cmd == CMD_CAN_SI_250KB_NO_RESPONSE ||
		cmd == CMD_CAN_EI_250KB_NO_RESPONSE)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdLocal
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if command is local command, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdLocal(int8 cmd)
{
    if (cmd == CMD_CAN_SET_STD_MASK_FILTER ||
        cmd == CMD_CAN_SET_XTD_MASK_FILTER ||
		cmd == CMD_CAN_SET_P2_CAN_TIMEOUT ||
		cmd == CMD_READ_BATTERY_VOLTAGE ||
        cmd == CMD_UNLOCK_ADAPTER ||
		cmd == CMD_ISO_ENABLE_L_LINE_XMIT ||
		cmd == CMD_ISO_SET_9600_BAUD ||
		cmd == CMD_ISO_CHECKSUM_ENABLE ||
		cmd == CMD_ISO_5MS_BYTE_DELAY_ENABLE ||
		cmd == CMD_READ_EEPROM ||
		cmd == CMD_WRITE_EEPROM)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------
// Function Name: IsCmdFirstResp
// Description:
// Parameters: cmd - adapter command
// Output Parameters:
// Return Values: Return 1 if command is a 1st response cmd, 0 otherwise.
//------------------------------------------------------------------------------
int8 IsCmdFirstResp(int8 cmd)
{
    if (cmd == CMD_VPW_1ST_RESPONSE ||
        cmd == CMD_PWM_1ST_RESPONSE ||
        cmd == CMD_ISO_1ST_RESPONSE ||
        cmd == CMD_KWP_1ST_RESPONSE ||
        cmd == CMD_CAN_SI_250KB_1ST ||
	    cmd == CMD_CAN_SI_500KB_1ST ||
		cmd == CMD_CAN_EI_250KB_1ST ||
		cmd == CMD_CAN_EI_500KB_1ST )
		return TRUE;
	else
		return FALSE;
}

//------------------------------------------------------------------------------
// Function Name: checksum
// Description: Calculate a checksum over an array of chars. 
// Parameters: pMsg - char array to compute checksum.
//  size - size of array
// Output Parameters:
// Return Values: computed checksum
//------------------------------------------------------------------------------
int8 checksum(int8* pMsg, int8 size)
{
    int8 cs=0;
    int8 i=0;
    for (i=0; i<size; i++)
        cs += pMsg[i];
    return cs;
}

//------------------------------------------------------------------------------
// Function Name: crc8
// Description: Calculate an 8-bit crc over an array of chars. 
//  Interface definition for 8-bit CRC (cyclic redundancy check):
//     Polynomial:                 1D
//     Initial CRC register value: FF
//     Reflected input and output: No
//     Inverted final output:      Yes
//     CRC of string "123456789":  4B
// Parameters: buffer - char array to compute crc.
//  count - size of array
// Output Parameters:
// Return Values: computed crc
//------------------------------------------------------------------------------
int8 crc8(int8* buffer, int8 count)
{
    int8 crc = 0xFF;
    int8 value; 
    int i;

    while (count--) {
        value = *buffer++;

        crc ^= (value << 0);
        for (i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x1D;
            }
            else {
                crc <<= 1;
            }
        }
    }
    return crc ^ 0xFF;
}

//------------------------------------------------------------------------------
// Function Name: crc8
// Description: Extract binary long packed into a string and return a 32 bit
//	integer number. String is packed MSB first.
// Parameters: str - string must be at least 4 bytes long.
// Output Parameters:
// Return Values: extracted int32 number from string.
//------------------------------------------------------------------------------
int32 StrToInt32(int8* str)
{
	int32 l;
    l = (int32)str[0] << 24;
    l |= (int32)str[1] << 16;
    l |= (int16)str[2] << 8;
    l |= str[3];
	return l;
}

//------------------------------------------------------------------------------
// Function Name: Decrypt
// Description: Decrypt a single byte using a LFSR (linear feedback shift 
//  register). 
// Parameters: cc - encrypted character to decrypt
// Output Paramters: 
// Return Values: The decrypted character
//------------------------------------------------------------------------------
int8 Decrypt(int8 cc)
{
/*    int8 cBit;

    // update LFSR
    cBit = gLfsr & 1;
    gLfsr >>= 1;

    if (cBit)
      gLfsr ^= LFSR_MASK;

    // Decrypt byte
    gLast ^= cc ^ (int8)gLfsr ^ (int8)(gLfsr >> 8);
    return gLast;				// return 'Last' which is current character*/
}

//------------------------------------------------------------------------------
// Function Name: DecryptBlock
// Description: Decrypt BLOCK_SIZE string of characters. The first two bytes
//  of the string is the initial Lfsr value. The Lfsr can't be 0. Starting with 
//  the 3rd inbuf byte is the encrypted data. 
// Parameters: inbuf - encrypted string padded with the Lfsr initial value padded
//  into the first two bytes.
// Output Paramters: outbfr - the decrypted string minus the two pad bytes.
// Return Values: The decrypted character
//------------------------------------------------------------------------------
void DecryptBlock(int8 *outbfr, int8 *inbfr)
{
    int8 size = BLOCK_SIZE;
    int8 last = 0;
    int8 cBit;

    gLfsr = *inbfr++ << 8;
    gLfsr |= *inbfr++;
    last = 0;

//    while (size-- > 0)
//        *outbfr++ = Decrypt(*inbfr++); 
   while (size-- > 0)
   {
       cBit = gLfsr & 1;
       gLfsr >>= 1;

       if (cBit)
          gLfsr ^= LFSR_MASK;

       // Decrypt byte
       last ^= *inbfr++ ^ (unsigned char)gLfsr ^ (unsigned char)(gLfsr >> 8);
       *outbfr++ = last;
   }
}

//------------------------------------------------------------------------------
// Function Name: TraceStr
// Description: Send string to the debug output UART.
// Parameters: pMsg - char array to send to port. 
//  size - size of array
// Output Parameters:
// Return Values:
//------------------------------------------------------------------------------
#ifdef DEBUG
void TraceStr(int8* pMsg, int8 size)
{
    while(size--)
    {
        fputc(*pMsg++, STREAM_DEBUG);
        delay_ms(1);
    }
}
#endif

//------------------------------------------------------------------------------
// Function Name: Trace
// Description: Send a byte to the debug output UART.
// Parameters: msg - char to send to port. 
// Output Parameters:
// Return Values:
//------------------------------------------------------------------------------
#ifdef DEBUG
void Trace(int8 msg)
{
    fputc(msg, STREAM_DEBUG);
    delay_ms(1);
}
#endif







