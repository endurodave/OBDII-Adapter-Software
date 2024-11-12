//------------------------------------------------------------------------------
// File: Host
//
// Purpose: Host communication and parsing routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#include "Host.h"
#include "Globals.h"
#include "Const.h"
#include "Timer.h"
#include "Util.h"

// setup the host UART which is the hardware UART
#ifdef DASH_DYNO
#use rs232(baud=115200,xmit=PIN_C6,rcv=PIN_C7,parity=n,bits=8,stream=STREAM_HOST) 
#else
#use rs232(baud=19200,xmit=PIN_C6,rcv=PIN_C7,parity=n,bits=8,stream=STREAM_HOST) 
#endif

//------------------------------------------------------------------------------
// Function Name: HostParseMsg 
// Description: Parse and error check the incoming host message. gError will 
//  contain an error if parse fails. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostParseMsg()
{
	// error check incoming message (order of these checks is important! Last
    // check has the highest priority error code).
    if (gFlags.adapterLocked == TRUE && gHostBuf[IDX_HOST_COMMAND] != CMD_UNLOCK_ADAPTER)
        gError = ERR_ADAPTER_LOCKED;
    if (gHostBuf[IDX_HOST_DESTINATION] != DESTINATION_ADDR)
        gError = ERR_WRONG_DESTINATION;
    if (gHostBuf[IDX_HOST_BYTE_COUNT] > 10 && !IsCmdCAN(gHostBuf[IDX_HOST_COMMAND]) &&
    	!IsCmdLocal(gHostBuf[IDX_HOST_COMMAND]))
        gError = ERR_BYTE_COUNT_TOO_LARGE;
    if (gHostBuf[IDX_HOST_CHECKSUM] != checksum(gHostBuf+1, HOST_RECV_MSG_SIZE-2))
        gError = ERR_BAD_CHECKSUM;
}

//------------------------------------------------------------------------------
// Function Name: HostSendVehicleData
// Description: Send a vehilce data message to the host. The message(s) to send 
//	is contained  within global arrays. Messages must be sent to host in the 
//	order they were received from the vehicle.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostSendVehicleData()
{
    int8 charCnt;
    int8 msgCnt = 0;

    // send the data out the ISO RS-232 port
    while (msgCnt < gVehicleBufCnt)
    {
        // zero all buffer bytes
        //memset(gHostBuf, 0x00, HOST_SEND_MSG_SIZE-1);

        // add destination
        gHostBuf[IDX_VEHICLE_DESTINATION] = DESTINATION_ADDR;

        // add command
        if (gFlags.comType == MODE_ISO)
            gHostBuf[IDX_VEHICLE_COMMAND] = CMD_ISO_MSG;
        else if (gFlags.comType == MODE_VPW)
            gHostBuf[IDX_VEHICLE_COMMAND] = CMD_VPW_MSG;
        else if (gFlags.comType == MODE_PWM)
            gHostBuf[IDX_VEHICLE_COMMAND] = CMD_PWM_MSG;
        else if (gFlags.comType == MODE_KWP)
            gHostBuf[IDX_VEHICLE_COMMAND] = CMD_KWP_MSG;
		else if (gFlags.comType == MODE_CAN)
            gHostBuf[IDX_VEHICLE_COMMAND] = CMD_CAN_MSG;
 
        // add 11-bytes of response
        for (charCnt=0; charCnt<VEHICLE_MSG_SIZE-1; charCnt++)
            gHostBuf[charCnt+2] = gVehicleBuf[msgCnt][charCnt];

        // add checksum (don't count first byte in checksum)
        gHostBuf[IDX_VEHICLE_CHECKSUM] = checksum(&gHostBuf[1], HOST_SEND_MSG_SIZE-2);
        
        // send message to host
        for (charCnt=0; charCnt<HOST_SEND_MSG_SIZE; charCnt++)
	        fputc(gHostBuf[charCnt], STREAM_HOST); 

        msgCnt++;    
    }
    
    // all messages sent
    gVehicleBufCnt = 0;
}

//------------------------------------------------------------------------------
// Function Name: HostSendLocalData
// Description: Send a message to the host that doesn't require vehicle 
//	communication (e.g. A/D battery reading). The message to send is contained 
//  within gVehicleBuf[0] up to 10 bytes of information.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostSendLocalData()
{
    int8 charCnt;

    // zero all buffer bytes
    //memset(gHostBuf, 0x00, HOST_SEND_MSG_SIZE-1);

    // add bytes
    gHostBuf[IDX_VEHICLE_DESTINATION] = DESTINATION_ADDR;
    gHostBuf[IDX_VEHICLE_COMMAND] = CMD_LOCAL_DATA;
    gHostBuf[IDX_HOST_BYTE_COUNT] = gCurrentCmd;

    // add up to 10-bytes of response bytes from vehicle buffer 0
    for (charCnt=0; charCnt<VEHICLE_MSG_SIZE-2; charCnt++)
        gHostBuf[charCnt+3] = gVehicleBuf[0][charCnt];

    // add checksum (don't count first byte in checksum)
    gHostBuf[IDX_VEHICLE_CHECKSUM] = checksum(&gHostBuf[1], HOST_SEND_MSG_SIZE-2);
            
    // send local data message to host
    for (charCnt=0; charCnt<HOST_SEND_MSG_SIZE; charCnt++)
        fputc(gHostBuf[charCnt], STREAM_HOST);
}

//------------------------------------------------------------------------------
// Function Name: HostSendError
// Description: Sends an error message to the host. Checks the global error 
//  byte set by some other part of the system.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostSendError()
{
    int8 charCnt;

    // zero all buffer bytes
    //memset(gHostBuf, 0x00, HOST_SEND_MSG_SIZE-1);

    // add bytes
    gHostBuf[IDX_VEHICLE_DESTINATION] = DESTINATION_ADDR;
    gHostBuf[IDX_VEHICLE_COMMAND] = CMD_ERROR;
    gHostBuf[2] = 0xFF;         // ME software rev byte, make FF
    gHostBuf[3] = gError;
    gHostBuf[4] = CAP_INTERFACE;
    gHostBuf[5] = SOFTWARE_REV; // Auterra software rev byte
	gHostBuf[6] = gErrorExtraData1;   // extra data, if any
	gHostBuf[7] = gErrorExtraData2;
	gHostBuf[8] = gErrorExtraData3;

    // add checksum (don't count first byte in checksum)
    gHostBuf[IDX_VEHICLE_CHECKSUM] = checksum(&gHostBuf[1], HOST_SEND_MSG_SIZE-2);
            
    // send error message to host
    for (charCnt=0; charCnt<HOST_SEND_MSG_SIZE; charCnt++)
        fputc(gHostBuf[charCnt], STREAM_HOST);

    // sent error so no vehicle buffers to send
    gVehicleBufCnt = 0; 
    gError = ERR_NONE;
}

#ifdef BLUETOOTH
//------------------------------------------------------------------------------
// Function Name: SendHost
// Description: 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostSend(int8* bufToSend, int8 size)
{
	int8 charCnt;
	for (charCnt=0; charCnt<size; charCnt++)
	{
        fputc(bufToSend[charCnt], STREAM_HOST);
        delay_ms(20);
    }
}

//------------------------------------------------------------------------------
// Function Name: HostInitBluetooth
// Description: 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void HostInitBluetooth()
{
	// If the EEPROM sector 30 first byte is 0xFF then auto pair the adapter to the
	// Windows PC for manufacturing ease. Otherwise, don't auto pair for shipment to
	// the customer. The AdapterTest windows software writes a non-FF value after
	// successfully completing the test.
	int8 autoPair;
	autoPair = read_eeprom(30 * EEPROM_SECTOR_SIZE);
	if (autoPair != 0xFF)
		return;
	
	delay_ms(1000);
	
	// Get the RN42 Bluetooth module into command mode
	gHostBuf[0] = '$';
	gHostBuf[1] = '$';
	gHostBuf[2] = '$';	
    HostSend(&gHostBuf[0], 3);
    
	delay_ms(50);
	
	// Send RN42 command and Bluetooth address to auto-pair
	gHostBuf[0] = 'C';	
	gHostBuf[1] = ',';	
	gHostBuf[2] = '0';
	gHostBuf[3] = '0';
	gHostBuf[4] = '0';
	gHostBuf[5] = '2';
	gHostBuf[6] = '7';
	gHostBuf[7] = '2';
	gHostBuf[8] = '3';
	gHostBuf[9] = '3';
	gHostBuf[10] = '3';
	gHostBuf[11] = 'C';
	gHostBuf[12] = '6';
	gHostBuf[13] = 'D';
	HostSend(&gHostBuf[0], 14);
	
	// This was to read the Bluetooth address from sector 0 EEPROM, didn't work
	// so for now the address is hard coded
/*	gHostBuf[0] = 'C';	
	gHostBuf[1] = ',';	
	HostSend(&gHostBuf[0], 2);
	
	ReadEepromLocal(0);
	HostSend(&gVehicleBuf[0][2], 8);
	
	ReadEepromLocal(1);
	HostSend(&gVehicleBuf[0][2], 4);*/
	
	delay_ms(3000);
	
	// Get the RN42 Bluetooth out of command mode
	//memcpy(gHostBuf, "---\r", 4);
	gHostBuf[0] = '-';
	gHostBuf[1] = '-';
	gHostBuf[2] = '-';
	gHostBuf[3] = '\r';
    HostSend(&gHostBuf[0], 4);
	
	gHostBufCnt = 0;
}

#endif

//------------------------------------------------------------------------------
// Function Name: HostUartISR
// Description: Interrupt function to handling incoming bytes from the host. 
//  Put the bytes into a global array to be picked up by the main loop. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
#int_rda
void HostUartISR()
{
	int8 junk;

	// if UART error occurred
	if (bit_test(RCSTA, OERR))
	{
        // per Microchip spec, clear CREN to clear OERR error
        bit_clear(RCSTA, CREN);	 // disable UART
        bit_set(RCSTA, CREN);	 // re-enable UART
        return;
    } 
    if (bit_test(RCSTA, FERR))
    {
		// per Microship spec, read RCREG to clear FERR	and discard byte
		junk = RCREG;	   // read and discard byte
		return;
	}

    // don't overrun host buffer
    if (gHostBufCnt < HOST_RECV_MSG_SIZE)
    {
        // get byte from UART and place in global array
        gHostBuf[gHostBufCnt] = RCREG; //fgetc(STREAM_HOST);

        // if this is the first byte it must be the destination addr
        // this helps get the scan tool re-sync'ed with host
        if (gHostBuf[0] == DESTINATION_ADDR)
            gHostBufCnt++;
		else
			gHostBufCnt = 0;  

        // keep reseting timer while chars coming in
        gHostRecvTimeout = HstGetTicks();
    }
    else
        // eat char from port to stop UART interrupt from firing
        junk = RCREG; //fgetc(STREAM_HOST);

}



