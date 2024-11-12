//------------------------------------------------------------------------------
// File: Vehicle
//
// Purpose: Vehicle communication and parsing routines.
//
// TBD - bit_test() takes too much ROM, might be able to bit mask and save space.
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#include "Vehicle.h"
#include "Globals.h"
#include "Const.h"
#include "Timer.h"
#include "Util.h"
#include "PWM-02.h"
#include "VPW-02.h"
#include "CAN18XX8.h"
#include "CAN.h"

//#use rs232(baud=9600,xmit=PIN_B7,rcv=PIN_B5,parity=n,bits=8,stream=ISO_K_LINE_STREAM)  // laptop test only
#use rs232(baud=10400,xmit=PIN_B6,rcv=PIN_B5,invert,parity=n,bits=8,stream=ISO_K_LINE_STREAM)
#use rs232(baud=9600,xmit=PIN_B6,rcv=PIN_B5,invert,parity=n,bits=8,stream=ISO_K_9600_LINE_STREAM)
#use rs232(baud=10400,xmit=PIN_B7,invert,parity=n,bits=8,stream=ISO_L_LINE_STREAM)
#use rs232(baud=9600,xmit=PIN_B7,invert,parity=n,bits=8,stream=ISO_L_9600_LINE_STREAM)

void VehInitISO(int8 addr);
void VehSendISO();
void VehRecvISO();
void VehSendJ1850(int8 type);
void VehRecvJ1850(int8 type);
void CheckSendRecvErr(int8 err);
void CheckCRC(int8* msgSizes);
void VehSend5BPS(int8 byte);
void VehFastInit();
void VehReadBatteryVoltage();
void VehSetLLineXmitEnable();
void VehSetIsoChecksumEnable();
void VehSetIso9600BaudEnable();
void VehSetIso5msByteDelayEnable();
int8 timed_getc(int8* timedOut, int16 timeout);
void VehSendKLine(int8 b);
void VehSendLLine(int8 b);

//------------------------------------------------------------------------------
// Function Name: VehSendRecv
// Description: Send host query and receive vehicle response. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendRecv()
{
    gVehicleBufCnt = 0;

	// turn off CAN module if current command is not a CAN command
	if (CANGetOperationMode() != CAN_OP_MODE_SLEEP)
	{
		if (!IsCmdCAN(gCurrentCmd))
			CanDisable();
	}

    if (IsCmdFirstResp(gCurrentCmd))
        gFlags.firstResp = TRUE;
    else
        gFlags.firstResp = FALSE;

    // send message to vehicle based on host command type
    if (gCurrentCmd == CMD_VPW ||
        gCurrentCmd == CMD_VPW_1ST_RESPONSE)
    {
        gFlags.comType = MODE_VPW;
        VehSendJ1850(MODE_VPW);
        if (gError == ERR_NONE)
            VehRecvJ1850(MODE_VPW);
    }
    else if (gCurrentCmd == CMD_PWM ||
        gCurrentCmd == CMD_PWM_WITH_IFR ||
        gCurrentCmd == CMD_PWM_1ST_RESPONSE)
    {
        gFlags.comType = MODE_PWM;
        VehSendJ1850(MODE_PWM);
        if (gError == ERR_NONE)
            VehRecvJ1850(MODE_PWM);
    }
    else if (gCurrentCmd == CMD_ISO_WITH_5_BAUD ||
        gCurrentCmd == CMD_KWP_WITH_5_BAUD)
    {
        // if ISO, use 0x33 for 5BPS addr, otherwise use target addr. ISO 14230
        // says use 0x33 also, but for future expansion get value from target addr
        if (gCurrentCmd == CMD_ISO_WITH_5_BAUD)
        {
            gFlags.comType = MODE_ISO;
            VehInitISO(0x33);
        }
        else
        {
            gFlags.comType = MODE_KWP;
            VehInitISO(gHostBuf[IDX_HOST_START_DATA+1]);
        }

        if (gError == ERR_NONE)
        {
            VehSendISO();
            VehRecvISO();
        }
    }
    else if (gCurrentCmd == CMD_ISO ||
        gCurrentCmd == CMD_ISO_1ST_RESPONSE ||
        gCurrentCmd == CMD_KWP ||
        gCurrentCmd == CMD_KWP_1ST_RESPONSE ||
		gCurrentCmd == CMD_ISO_NO_RESPONSE)
    {
        if (gCurrentCmd == CMD_ISO ||
            gCurrentCmd == CMD_ISO_1ST_RESPONSE)
            gFlags.comType = MODE_ISO;
        else
            gFlags.comType = MODE_KWP;
        VehSendISO();
		if (gCurrentCmd != CMD_ISO_NO_RESPONSE)
        	VehRecvISO();
    }
    else if (gCurrentCmd == CMD_KWP_FAST_INIT ||
			 gCurrentCmd == CMD_KWP_FAST_INIT_9600)
    {
        gFlags.comType = MODE_KWP;
        VehFastInit();
        VehSendISO();
        VehRecvISO();
    }
    else if (IsCmdCAN(gCurrentCmd))
    {
		// disable CAN RX interrupts while Send/Recv on CAN
		disable_interrupts(INT_CANRX1);
		disable_interrupts(INT_CANRX0);
        gFlags.comType = MODE_CAN;
        VehSendCAN();
        if (gError == ERR_NONE && IsCmdNoResponse(gCurrentCmd) == 0)
		    VehRecvCAN();

		enable_interrupts(INT_CANRX1);
		enable_interrupts(INT_CANRX0);
	}
	else if (gCurrentCmd == CMD_CAN_SET_STD_MASK_FILTER ||
			 gCurrentCmd == CMD_CAN_SET_XTD_MASK_FILTER)
		CanSetMaskFilter();
	else if (gCurrentCmd == CMD_CAN_SET_P2_CAN_TIMEOUT)
		CanSetP2CanTimeout();
	else if (gCurrentCmd == CMD_READ_BATTERY_VOLTAGE)
		VehReadBatteryVoltage();
	else if (gCurrentCmd == CMD_ISO_CHECKSUM_ENABLE)
		VehSetIsoChecksumEnable();
	else if (gCurrentCmd == CMD_ISO_5MS_BYTE_DELAY_ENABLE)
		VehSetIso5msByteDelayEnable();
	else if (gCurrentCmd == CMD_ISO_ENABLE_L_LINE_XMIT)
		VehSetLLineXmitEnable();
	else if (gCurrentCmd == CMD_ISO_SET_9600_BAUD)
		VehSetIso9600BaudEnable();
	else if (gCurrentCmd == CMD_READ_EEPROM)
		ReadEeprom();
	else if (gCurrentCmd == CMD_WRITE_EEPROM)
		WriteEeprom();
    else if (gCurrentCmd == CMD_UNLOCK_ADAPTER)
        gFlags.adapterLocked = FALSE;
    else
        gError = ERR_NOT_IMPLEMENTED;
}

//------------------------------------------------------------------------------
// Function Name: VehSendISO
// Description: Send a query using ISO.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendISO()
{
	int8 i;

    // send the data out the ISO RS-232 port
    for (i=0; i<gHostBuf[IDX_HOST_BYTE_COUNT]; i++)
    {
		VehSendKLine(gHostBuf[i+3]);
		VehSendLLine(gHostBuf[i+3]);
		if (gFlags.enableIso5msByteDelay == TRUE)
	        delay_ms(5);   // satisfy P4 timing per ISO 9141 (spec says 5mS)
    }

   
	if (gFlags.enableIsoChecksum == TRUE)
	{ 
		// calc and send message checksum
    	i = checksum(&gHostBuf[3], gHostBuf[IDX_HOST_BYTE_COUNT]);
		VehSendKLine(i);
		VehSendLLine(i);
	}
}

//------------------------------------------------------------------------------
// Function Name: VehSendKLine
// Description: Send 1 byte out the ISO K line.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendKLine(int8 b)
{
	if (gFlags.enableIso9600Baud == TRUE)
		fputc(b, ISO_K_9600_LINE_STREAM);
	else
	    fputc(b, ISO_K_LINE_STREAM);
}

//------------------------------------------------------------------------------
// Function Name: VehSendLLine
// Description: Send 1 byte out the ISO L line.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendLLine(int8 b)
{
	if (gFlags.enableLLineXmit == TRUE)
	{
		if (gFlags.enableIso9600Baud == TRUE)
			fputc(b, ISO_L_9600_LINE_STREAM);
		else
			fputc(b, ISO_L_LINE_STREAM);
	}
}

//------------------------------------------------------------------------------
// Function Name: VehRecvISO
// Description: Receive one or more messages using ISO.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehRecvISO()
{
    int8 charTimeout;
    int8 cnt;   
    HstTimer recvTimer;
    int8 msgSize[VEHICLE_MAX_MSGS];

    recvTimer = HstGetTicks();

    // get all ISO messages. 50mS of no data terminates the receive
    do 
    {
        // is there a char coming in the RS-232 port?
        if (kbhit())
        {
            // get one entire ISO OBD II message. once first char arrives, 
            // each char will be max 20mS apart
            for (cnt=0; cnt<VEHICLE_MSG_SIZE; cnt++)
            {
                gVehicleBuf[gVehicleBufCnt][cnt] = timed_getc(&charTimeout, 2000);
                if (charTimeout)
                    break;
            }

            // if no chars arrived and not a single buffer filled
            if (cnt == 0 && gVehicleBufCnt == 0)
            {
                gError = ERR_ISO_NO_RESPONSE;
                return;
            }

            // Message filter: if message is not a response for scan tool, ignore it. 
            // Mode byte bit 6 = 0 are requests, bit 6 = 1 are responses
			// This filter is not necessary, and could interfere with KWP fast init messages
            //if (gVehicleBuf[gVehicleBufCnt][3] != (gHostBuf[6] | 0x40))
            //    continue;

            // successfully obtained a message (checksum not checked yet)
            msgSize[gVehicleBufCnt] = cnt;
            if (++gVehicleBufCnt >= VEHICLE_MAX_MSGS)
                break;

            // if host only wants one response message
            if (gFlags.firstResp)
                break;

            // reset timer for next message
            recvTimer = HstGetTicks();
        }
    } while (!HstTestTimer(recvTimer, HST_FIFTY_MS));

    if (gVehicleBufCnt == 0)
    {
        gError = ERR_ISO_NO_RESPONSE;
        return;
    }

	if (gFlags.enableIsoChecksum == TRUE)
	{
		// error check each received message using checksum
    	for (cnt=0; cnt<gVehicleBufCnt; cnt++)
	    {       
    	    // does received checksum match what was sent?        
        	if (gVehicleBuf[cnt][msgSize[cnt]-1] != 
	            checksum(&gVehicleBuf[cnt][0], msgSize[cnt]-1))
    	    {
	            gError = ERR_ISO_BAD_CHECKSUM;
    	        return;
	        }
	    }
	}
}

//------------------------------------------------------------------------------
// Function Name: VehInitISO
// Description: Performs the ISO initialization sequence. 
// Parameters: addr - 5BPS ISO initialization address. 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehInitISO(int8 addr)
{
    int8 b, i, timeout;	

    VehSend5BPS(addr);
	gErrorExtraData1 = addr;

    // get sychronization byte
    b = timed_getc(&timeout, 30000);
	if (timeout == TRUE)
	{
		gError = ERR_ISO_SYNC_BYTE_NOT_RECEIVED;
		return;
	}  
    if (b != 0x55)  
    {
        gError = ERR_ISO_NO_SYNC;
		gErrorExtraData2 = b;
        return;
    }

    // get two keywords bytes (allow any key bytes)
    for (i=0; i<2; i++)
    {
        b = timed_getc(&timeout, 2000);
        if (timeout)
        {
            gError = ERR_ISO_NO_KEYBYTE;    
            return;
        }
    }

    delay_ms(35);  // satisfy W4 per ISO 9141		

    // send inv second keyword
	VehSendKLine(~b);
	gErrorExtraData2 = b;

    // get address
    b = timed_getc(&timeout, 5000);
	if (timeout == TRUE)
	{
		gError = ERR_ISO_INV_ADDR_NOT_RECEIVED;
		return;
	}      
    if (b != ~addr)       
    {
        gError = ERR_ISO_INCORRECT_INV_ADDR;
		gErrorExtraData2 = b;
        return;
    }

    delay_ms(60);   // satisfy P3 timing per ISO 9141 (55ms really, don't shave too close)
}

//------------------------------------------------------------------------------
// Function Name: VehSendJ1850
// Description: Send a message using J1850. 
// Parameters: type - VPW or PWM. 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendJ1850(int8 type)
{
    int8 err;
    
    // tack on the CRC to end of message
    gHostBuf[IDX_HOST_START_DATA + gHostBuf[IDX_HOST_BYTE_COUNT]] =
        crc8(&gHostBuf[IDX_HOST_START_DATA], gHostBuf[IDX_HOST_BYTE_COUNT]);

    // send J1850 message to vehicle (max 100mS timeout to get on bus)
    if (type == MODE_VPW)
        err = VPWSend(&gHostBuf[IDX_HOST_START_DATA], gHostBuf[IDX_HOST_BYTE_COUNT]+1, 50);
    else  
        err = PWMSend(&gHostBuf[IDX_HOST_START_DATA], gHostBuf[IDX_HOST_BYTE_COUNT]+1, 50);
        
    CheckSendRecvErr(err);
}

//------------------------------------------------------------------------------
// Function Name: VehRecvJ1850
// Description: Recv a message using J1850. If 1st response, this routine should 
//  exit on the first message. If not 1st response, this routine should exit
//  after VEHICLE_MAX_MSGS received or a recv timeout occurs.  
//  Note: The VPWRecv and PWMRecv funcs must pass in a buffer located in bank 0. 
// Parameters: type - VPW or PWM. 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehRecvJ1850(int8 type)
{
    int8 cpyCnt, err, size;
    int8 msgSizes[VEHICLE_MAX_MSGS];
    int32 totalRecvTimeout;
    int16 recvTimeout;
    
    totalRecvTimeout = 0;
    recvTimeout = HstGetTicks();

    gVehicleBufCnt = 0;
    while (1)
    {
        // get message from vehicle. Per J2190 if no response in 100mS, then timeout
        if (type == MODE_VPW)
            err = VPWRecv(&gVehicleBuf[gVehicleBufCnt][0], &size, 50);  // 50 * 2mS = 100mS timeout
        else
            err = PWMRecv(&gVehicleBuf[gVehicleBufCnt][0], &size, 50);
            
        // has 150mS passed since VPWRecv/PWMRecv call?
		// each msg 100mS appart is the spec, but some vehicles take longer. This check allows for more waiting time.
        totalRecvTimeout += (int32)HstElapsedTime(recvTimeout);
        if (totalRecvTimeout > 93750)      // 150mS in 1.6uS ticks. 
        {
            gError = ERR_J1850_NO_RESPONSE;
            break;
        }
        recvTimeout = HstGetTicks();
        
        // if no bus activity (i.e. no response) then try again
        if (bit_test(err, 5))
            continue;

        CheckSendRecvErr(err);
        if (gError)
            break;
            
        // Message filter: if message is not a response for scan tool, ignore it. 
        // Mode byte bit 6 = 0 are requests, bit 6 = 1 are responses
        if (gVehicleBuf[gVehicleBufCnt][3] != (gHostBuf[6] | 0x40))
            continue;
        
        // successfully obtained a message (CRC not checked yet)
        msgSizes[gVehicleBufCnt] = size;

		// valid msg for scan tool received, so reset total timeout. Each message arriving can be 100mS appart.
    	totalRecvTimeout = 0;
	
        if (++gVehicleBufCnt >= VEHICLE_MAX_MSGS)
            break;

        // if host wanted only 1 response, then break early
        if (gFlags.firstResp)
            break;
    }

    // if vehicle obtained at least 1 message then timed out, that is okay
    if (gVehicleBufCnt && gError == ERR_J1850_NO_RESPONSE)
        gError = ERR_NONE;

    // check the CRC of the incoming messages
    CheckCRC(msgSizes);	
}

//------------------------------------------------------------------------------
// Function Name: CheckSendRecvErr
// Description: Test for VPW and PWM send/recv errors. 
// Parameters: err - error byte returned from either PWMSend, PWMRecv, VPWSend,
//  VPWRecv.
//  Sets the global error byte to error is detected.
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CheckSendRecvErr(int8 err)
{
    // test for recv errors
    if (bit_test(err, 1))
        gError = ERR_J1850_FRAMING_ERROR;
    else if (bit_test(err, 5))
        gError = ERR_J1850_NO_RESPONSE;
    else if (bit_test(err, 4))
        gError = ERR_J1850_MSG_OVERFLOW;
}

//------------------------------------------------------------------------------
// Function Name: CheckCRC
// Description: Check J1850 CRC on incoming messages. Sets global error flag
//  if error found.  
// Parameters: msgSize - an array of message sizes. Array can't exceed 
//  VEHICLE_MAX_MSGS.
// Output Parameters:
// Return Values: 
//-----------------------------------------------------------------------------
void CheckCRC(int8* msgSizes)
{
    int8 cnt;
    
    // error check each received message using checksum
    for (cnt=0; cnt<gVehicleBufCnt; cnt++)
    {       
        // does received checksum match what was sent?        
        if (gVehicleBuf[cnt][msgSizes[cnt]-1] != 
            crc8(&gVehicleBuf[cnt][0], msgSizes[cnt]-1))
        {
            gError = ERR_J1850_BAD_CRC;
            return;
        }
    }
}

//------------------------------------------------------------------------------
// Function Name: VehSend5BPS
// Description: Sends one address byte out on the ISO K and L lines at at rate
//  of 5 BPS.
//  NOTE: Logic values are inverted (i.e. 1 = low and 0 = high) by the scan tool
//  hardaware.
// Parameters: byte - target address byte to send.
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSend5BPS(int8 byte)
{
    int8 bit;

    // TBD - KWP time W5 delay here?
    // ensure ISO 9141 time W0 of 2mS satisfied
    output_bit(PIN_B6, 0);  
    output_bit(PIN_B7, 0);
    delay_ms(2);

    // send out one address byte at 5 BPS
    // start bit
    output_bit(PIN_B6, 1);
    output_bit(PIN_B7, 1);
    delay_ms(200);

    // send byte starting with LSB
    for (bit=0; bit<8; bit++)
    {
        if (bit_test(byte, 0))
        {
            output_bit(PIN_B6, 0);
            output_bit(PIN_B7, 0);
            delay_ms(200);
        }
        else
        {
            output_bit(PIN_B6, 1);
            output_bit(PIN_B7, 1);
            delay_ms(200);
        }
        // go to next bit
        byte = byte >> 1;
    } 

    // stop bit
    output_bit(PIN_B6, 0);
    output_bit(PIN_B7, 0);
    delay_ms(200);
}

//------------------------------------------------------------------------------
// Function Name: VehFastInit
// Description: Perform the KWP fast initalization on K and L lines. 
//  NOTE: Logic values are inverted (i.e. 1 = low and 0 = high) by the scan tool
//  hardaware.
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehFastInit()
{
    //TBD - W5 delay here?

    // TiniL from 14230-2 spec
    output_bit(PIN_B6, 1);
    output_bit(PIN_B7, 1);
	if (gCurrentCmd == CMD_KWP_FAST_INIT_9600)
	{
		gFlags.enableIso9600Baud = TRUE;
    	delay_ms(35);
	}
	else
	{
		gFlags.enableIso9600Baud = FALSE;
    	delay_ms(25);
	}

    // TiniH from 14230-2 spec
    output_bit(PIN_B6, 0);
    output_bit(PIN_B7, 0);
	if (gCurrentCmd == CMD_KWP_FAST_INIT_9600)
		delay_ms(15);
	else
    	delay_ms(25);

    // TBD - toggle low here?
}

//------------------------------------------------------------------------------
// Function Name: VehSetIso5msByteDelayEnable
// Description: ISO 9141 states the interbyte message timing to the vehicle
//	must be 5mS. When enabled, the 5mS between each byte is inserted, when 
//	disabled no delay is inserted and the data is sent as fast as possible.
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSetIso5msByteDelayEnable()
{
	if (gHostBuf[2] == 0)
		gFlags.enableIso5msByteDelay = FALSE;
	else
		gFlags.enableIso5msByteDelay = TRUE;
}

//------------------------------------------------------------------------------
// Function Name: VehSetLLineXmitEnable
// Description: Sets the L line transmit either true of false. 
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSetLLineXmitEnable()
{
	if (gHostBuf[2] == 0)
		gFlags.enableLLineXmit = FALSE;
	else
		gFlags.enableLLineXmit = TRUE;
}

//------------------------------------------------------------------------------
// Function Name: VehIsoSet9600BaudEnable
// Description: Sets ISO baud rate to 9600 or 10.4k 
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSetIso9600BaudEnable()
{
	if (gHostBuf[2] == 0)
		gFlags.enableIso9600Baud = FALSE;
	else
		gFlags.enableIso9600Baud = TRUE;
}

//------------------------------------------------------------------------------
// Function Name: VehSetIsoChecksumEnable
// Description: Sets adding ISO checksum to the end of each messsage, or to
//	not add the checksum byte. 
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSetIsoChecksumEnable()
{
	if (gHostBuf[2] == 0)
		gFlags.enableIsoChecksum = FALSE;
	else
		gFlags.enableIsoChecksum = TRUE;
}

//------------------------------------------------------------------------------
// Function Name: VehReadBatteryVoltage
// Description: Read the battery voltage via the A/D converter. A/D is assumed
//	to be already setup and on the right channel before this function is called.
// Parameters:
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehReadBatteryVoltage()
{
	int16 voltage;
	
	if (gDevid2 == VER_18F248)
	{	
		// on old 18F248 part, we can use the CCS compiler built in function
		set_adc_channel(0);
		delay_us(10);
		voltage = read_adc();
		
		// put A/D reading into buffer for transmission to host
		gVehicleBuf[0][0] = (int8)(voltage >> 8);
		gVehicleBuf[0][1] = (int8)voltage;
	}
	else
	{
		// on new 18F2480 part, the register/bit locations changed so must do manually
		
		// start ADC conversion
		bit_set(ADCON0, 1);
		
		// wait for conversion to complete
		while (bit_test(ADCON0, 1));
		
		// read A/D reading into buffer for transmission to host
		gVehicleBuf[0][0] = ADRESH;
		gVehicleBuf[0][1] = ADRESL;
	}	
}

//------------------------------------------------------------------------------
// Function Name: timed_getc
// Description: Get a character from the ISO UART using a timeout. If a 
//  character doesn't arrive in the timeout specified the function returns.
//  NOTE: timeout units of measure is 10's of uS. (e.g. 100 = 1000uS)
// Parameters: timedOut - TRUE if routine timed out with no char, FALSE otherwise.
//  timeout - the timeout to wait for the char in 10's of uS.
// Output Parameters:
// Return Values: The received character. Valid only if timedOut is FALSE.
//------------------------------------------------------------------------------
int8 timed_getc(BOOLEAN* timedOut, int16 timeout) 
{
    int16 time;

    *timedOut = FALSE;
    time=0;

    // wait up to timeout for a character to arrive
    while(!kbhit()&&(++time<timeout))   
        delay_us(10);

    if(kbhit())
	{
		if (gFlags.enableIso9600Baud == TRUE)
	        return(fgetc(ISO_K_9600_LINE_STREAM));
		else
			return(fgetc(ISO_K_LINE_STREAM));
	}
    else
    {
        *timedOut=TRUE;
        return(0);
    } 
}










