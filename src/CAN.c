//------------------------------------------------------------------------------
// File: CAN
//
// Purpose: CAN communication and parsing routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#include "CAN.h"
#include <stdlib.h>

#define SINGLE_FRAME        0x00
#define FIRST_FRAME         0x10
#define CONSECUTIVE_FRAME   0x20
#define FLOW_CONTROL        0x30

typedef enum CAN_SEGMENTED_RECV_MODE
{
	RECV_NONE,		// no segmented messages are pending (i.e. no FirstFrame messages rcvd)
	RECV_PENDING,	// one or more segmented messages are pending (i.e. waiting on ConsecutiveFrame(s))
	RECV_COMPLETE	// all segmented messages are received (i.e. all ConsecutiveFrame(s) rcvd)
};

static signed int16 gFirstFrameDataLength[MAX_ECU];
static int32 gFirstFrameIdentifier[MAX_ECU];
static int8 gFirstFrameRespondedECUs = 0;

static int32 gIdentifier = 0;
static int32 gStdMask 		= 0x000007f8;	  // 11-bit
static int32 gStdFilter 	= 0x000007e8;	  // 11-bit
static int32 gXtdMask		= 0x1fffff00;	  // 29-bit
static int32 gXtdFilter		= 0x18daf100;  	  // 29-bit

void CanInit();
void CanClearRxErrors();
void CanSend(int8* data);
void CanResetSegmentedRecvStats();
CAN_SEGMENTED_RECV_MODE CanIsSegmentedRecvComplete();

//------------------------------------------------------------------------------
// Function Name: VehSendCAN
// Description: Send the vehicle a message using CAN. 
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehSendCAN()
{
	// only call init when the CAN mode changes	or someone forces
    if (gLastCmd != gCurrentCmd || gFlags.forceInitCAN)
	{
		gFlags.forceInitCAN = FALSE;
        gFlags.canModeChanged = TRUE;
        CanInit();
	}
    else
    {
        gFlags.canModeChanged = FALSE;
    }
	
	// extract host indentifer from host message
	gIdentifier = StrToInt32(&gHostBuf[2]);
	
	// send host message to vehicle using CAN, only if not a listen only cmd
	if (!IsCmdListen(gCurrentCmd)) 
		CanSend(&gHostBuf[6]);
}

//------------------------------------------------------------------------------
// Function Name: VehRecvCAN
// Description: Receive a message using CAN.
//
//	This routine obtains each FirstFrame message from the ECU(s), if any, and 
//	responds with to the ECU with a FlowControl	message automatically without 
//	intervention from the host. The FirstFrame tells the adapter how many more 
//	bytes to expect	in ConsecutiveFrame(s) to follow. This routine waits for
//  all the bytes to arrive via ConsecutiveFrame(s) before returning.
//
//	All	FirstFrame and SingleFrame from all ECUs must arrive in 50mS. Then the 
//	adapter must wait up to 1S (N_Cr) for reception of the next ConsecutiveFrame.
//
//	This routine polls for CAN responses and does not use RX interrupts.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void VehRecvCAN()
{
	int8 FLOW_CONTROL_MSG[8] = { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    int16 recvTimeout;		
    int32 totalRecvTimeout;	   // J1979 P2can timeout
	int32 totalErrorTimeout;   // ISO 15765-2 N_Cr timeout
    int32 newMessageID;			
    int8 newMessageLen;
    CAN_RX_MSG_FLAGS newMessageFlags;
	int8 ecu;
	int16 elapsedTime;
	int32 firstRespECU;

    recvTimeout = HstGetTicks();
	totalRecvTimeout = 0;
	totalErrorTimeout = 0;
	CanResetSegmentedRecvStats();

    while (1)
	{
		elapsedTime = (int32)HstElapsedTime(recvTimeout);
        recvTimeout += elapsedTime;
        totalRecvTimeout += elapsedTime;
        totalErrorTimeout += elapsedTime;

		// if CAN error ISR sets an error while receiving messages then exit
		if (gError)  
			return;

		// else if timer exceeds P2can and no segmented messages are pending then exit
        else if (totalRecvTimeout > gP2can && CanIsSegmentedRecvComplete() != RECV_PENDING)  
			break;

		// if time since last message exceeds 1 second (ISO 15765-2 N_Cr) while waiting for a segmented 
		// message (i.e. ConsecutiveFrame) to arrive then exit
        else if (totalErrorTimeout > HST_ONE_S && CanIsSegmentedRecvComplete() == RECV_PENDING)
			break;
    
        // receive 1 CAN 8-byte message 
        if (CANReceiveMessage(&newMessageID, &gVehicleBuf[gVehicleBufCnt][3], 
	                          &newMessageLen, &newMessageFlags))
        {	
			// keep resetting CF error timeout as long as messages are arriving
			totalErrorTimeout = 0;

            if (newMessageFlags & CAN_RX_OVERFLOW)
		    {
    		    gError = ERR_CAN_RX_OVERFLOW;
    		    return;
		    }
			else if (newMessageLen != 8)
			{
				// some vehicle's like the Pontiac G6 2005 returns non-8 byte messages (which is a violation of
				// the specs. So just ignore if not exactly 8
			    //gError = ERR_CAN_RX_MESSAGE_LEN_WRONG;  TBD test
				//return;
			}

			// save	ID of first ECU to respond
			if (gVehicleBufCnt == 0)
				firstRespECU = newMessageID;
			
			// what CAN frame type?
			if ((gVehicleBuf[gVehicleBufCnt][3] & 0xF0) == FIRST_FRAME)
			{
    			// if vehicle sends a FirstFrame message, respond with a FlowControl message
				// must send within <100mS (ISO 15765-4 N_Br)
			    CanSend(FLOW_CONTROL_MSG);
			    if (gError != ERR_NONE)
				    return;

				// get the data length specifying the total user bytes to receive on segmented msg
                gFirstFrameDataLength[gFirstFrameRespondedECUs] = (gVehicleBuf[gVehicleBufCnt][3] & 0x0F) << 8;
                gFirstFrameDataLength[gFirstFrameRespondedECUs] |= gVehicleBuf[gVehicleBufCnt][4];
			    gFirstFrameDataLength[gFirstFrameRespondedECUs] -= 6;     // FirstFrame has 6 user data bytes
            	gFirstFrameIdentifier[gFirstFrameRespondedECUs] = newMessageID;
				gFirstFrameRespondedECUs++;
			}
			else if ((gVehicleBuf[gVehicleBufCnt][3] & 0xF0) == CONSECUTIVE_FRAME)
			{
				// search for received message identifier in list
				for (ecu=0; ecu<MAX_ECU; ecu++)
				{
					if (newMessageID == gFirstFrameIdentifier[ecu])
					{
						// found idendifier in list, decrement its remaining byte count
			    		gFirstFrameDataLength[ecu] -= 7;     // ConsecutiveFrame has, at most, 7 user data bytes
						break;
					}
				}
			}

			// add three bytes of identifier into host message
            gVehicleBuf[gVehicleBufCnt][0]	= (int8)(newMessageID >> 16);
            gVehicleBuf[gVehicleBufCnt][1]	= (int8)(newMessageID >> 8);
            gVehicleBuf[gVehicleBufCnt][2]	= (int8)(newMessageID);

			// if first response enable and not the 1st ECU, then toss this message by not
			// incrementing the gVehicleBufCnt below. Only accumulate messages from the
			// 1st ECU when host specifies 1st response cmd	
			if (gFlags.firstResp && firstRespECU != newMessageID)
				continue;
		
            // successfully obtained a message, inc buffer count
            if (++gVehicleBufCnt >= VEHICLE_MAX_MSGS)
                break;	   // all buffers full
				          
            // if host wants one response message and entire segmented msg, if any, is received
            if (gFlags.firstResp && gFirstFrameDataLength[0] <= 0)
                break;
		}
	}

    // if no CAN message received and no error already set
    if (gVehicleBufCnt == 0 && !gError)
	{
		// set error and save contents of COMSTAT register
	    gError = ERR_CAN_NO_RESPONSE;
		gErrorExtraData1 = COMSTAT;
	}
}

//------------------------------------------------------------------------------
// Function Name: CanSend
// Description: Send CAN message using the current interface and current 
//  identifier.
// Parameters:  
//  data - 8 OBD II frame data message to send
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanSend(BYTE* data)
{
    // is this a standard identifier message?
    if (IsCmdStandardId(gCurrentCmd))
	{
        // send standard id CAN message
	    if (!CANSendMessage(gIdentifier, data, 8, CAN_TX_PRIORITY_0  
	                                            & CAN_TX_STD_FRAME
                                                & CAN_TX_NO_RTR_FRAME))
            gError = ERR_CAN_TX_BUFFER_UNAVAIL;
	}
	else
	{
        // send extended id CAN message
	    if (!CANSendMessage(gIdentifier, data, 8, CAN_TX_PRIORITY_0  
	                                            & CAN_TX_XTD_FRAME
                                                & CAN_TX_NO_RTR_FRAME))
            gError = ERR_CAN_TX_BUFFER_UNAVAIL;
	}
}

//------------------------------------------------------------------------------
// Function Name: VehRecvCAN
// Description: Use RBX1 to double buffer RBX0.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanInit()
{
    // is this a standard id message?
    if (IsCmdStandardId(gCurrentCmd))
	{
	    // is this 250kb?
        if (IsCmd250kb(gCurrentCmd))
		{
		    // 250kb standard message initialization
	        CANInitialize(3, 2, 8, 4, 7, CAN_CONFIG_PHSEG2_PRG_ON 
                                         & CAN_CONFIG_SAMPLE_THRICE
                                         & CAN_CONFIG_VALID_STD_MSG
                                         & CAN_CONFIG_DBL_BUFFER_ON);	
		}
		else
		{
		    // 500kb standard message initialization
            CANInitialize(3, 1, 8, 4, 7, CAN_CONFIG_PHSEG2_PRG_ON 
                                        & CAN_CONFIG_SAMPLE_THRICE
                                        & CAN_CONFIG_VALID_STD_MSG
                                        & CAN_CONFIG_DBL_BUFFER_ON);
		}
									 
        // Set CAN module back into the configuration mode									     
	    CANSetOperationMode(CAN_OP_MODE_CONFIG);		
					
		// 11bit ECU Identifier range 0x7e8 to 0x7ef
	    CANSetMask(CAN_MASK_B1, gStdMask, CAN_CONFIG_STD_MSG);  
	    CANSetFilter(CAN_FILTER_B1_F1, gStdFilter, CAN_CONFIG_STD_MSG);

		// disable all other filters and masks by setting them to opposite
		// of the used mask (i.e. used mask is STD, then unused set to XTD)
	    CANSetMask(CAN_MASK_B2, 0xFFFFFFFF, CAN_CONFIG_STD_MSG);
	    CANSetFilter(CAN_FILTER_B1_F2, 0x00000000, CAN_CONFIG_XTD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F1, 0x00000000, CAN_CONFIG_XTD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F2, 0x00000000, CAN_CONFIG_XTD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F3, 0x00000000, CAN_CONFIG_XTD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F4, 0x00000000, CAN_CONFIG_XTD_MSG);
	}
	// else extended message
	else
	{
	    // is this 250kb?
        if (IsCmd250kb(gCurrentCmd))
		{
		    // 250kb extended message initialization
	        CANInitialize(3, 2, 8, 4, 7, CAN_CONFIG_PHSEG2_PRG_ON 
                                         & CAN_CONFIG_SAMPLE_THRICE
                                         & CAN_CONFIG_VALID_XTD_MSG
                                         & CAN_CONFIG_DBL_BUFFER_ON);	
		}
		else
		{
		    // 500kb extended message initialization
            CANInitialize(3, 1, 8, 4, 7, CAN_CONFIG_PHSEG2_PRG_ON 
                                        & CAN_CONFIG_SAMPLE_THRICE
                                        & CAN_CONFIG_VALID_XTD_MSG
                                        & CAN_CONFIG_DBL_BUFFER_ON);
		}
										
        // Set CAN module back into the configuration mode										
        CANSetOperationMode(CAN_OP_MODE_CONFIG);					

        // 29bit ECU Identifier range 18daf100 to 18daf1ff
		CANSetMask(CAN_MASK_B1, gXtdMask, CAN_CONFIG_XTD_MSG);	
        CANSetFilter(CAN_FILTER_B1_F1, gXtdFilter, CAN_CONFIG_XTD_MSG);

		// disable all other filters and masks by setting them to opposite
		// of the used mask (i.e. used mask is STD, then unused set to XTD)
	    CANSetMask(CAN_MASK_B2, 0xFFFFFFFF, CAN_CONFIG_XTD_MSG);
	    CANSetFilter(CAN_FILTER_B1_F2, 0x00000000, CAN_CONFIG_STD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F1, 0x00000000, CAN_CONFIG_STD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F2, 0x00000000, CAN_CONFIG_STD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F3, 0x00000000, CAN_CONFIG_STD_MSG);
	    CANSetFilter(CAN_FILTER_B2_F4, 0x00000000, CAN_CONFIG_STD_MSG);
	}
	
	// Set CAN module for normal RX & TX operation
	CANSetOperationMode(CAN_OP_MODE_NORMAL);

	// enable CAN output driver
   	output_low(PIN_C3);
}

//------------------------------------------------------------------------------
// Function Name: CanDisable
// Description: Disable CAN module, abort messages, and disable the external 
//	CAN hardware driver chip.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanDisable()
{
	// disable the external CAN output driver MCP2551 chip. This is required
	// because the CAN module in sleep or listen is in the wrong logic state
	// causing other CAN modules on the bus to complain about being clobbered
   	output_high(PIN_C3);

	// abort all pending messages, if any
	CANAbortAll();

	// put CAN module to sleep
 	// This call makes the adapter lockup sometimes (e.g. Toyota Tacoma 2006), offers no value so no need to use
	//CANSetOperationModeNoWait(CAN_OP_MODE_SLEEP);

	// on next CAN message, module needs to be reinitialized
	gFlags.forceInitCAN = TRUE;
}

//------------------------------------------------------------------------------
// Function Name: CanEmptyRxBuffer
// Description: Just discard any messages within the RX buffer.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanEmptyRxBuffer()
{
    int32 newMessageID;			
    int8 newMessageLen;
    CAN_RX_MSG_FLAGS newMessageFlags;
	
    if (IsCmdCAN(gLastCmd))
        // receive and discard a CAN message, if any
        CANReceiveMessage(&newMessageID, 0, &newMessageLen, &newMessageFlags);
}

//------------------------------------------------------------------------------
// Function Name: CanSetMaskFilter
// Description: Set the STD or XTD mask and filter based upon host message.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanSetMaskFilter()
{
	gFlags.forceInitCAN = TRUE;

	if (gHostBuf[IDX_HOST_COMMAND] == CMD_CAN_SET_STD_MASK_FILTER)
	{
		gStdMask = StrToInt32(&gHostBuf[2]);
		gStdFilter = StrToInt32(&gHostBuf[6]);
	}
	else
	{
		gXtdMask = StrToInt32(&gHostBuf[2]);
		gXtdFilter = StrToInt32(&gHostBuf[6]);
	}
}

//------------------------------------------------------------------------------
// Function Name: CanSetP2CanTimeout
// Description: Set P2can timeout value to used to indicate how long the adapter
//	should wait for incoming ECU messages.
//
//  Host should never send any mS value larger than 214748mS otherwise coversion
//	to ticks with MS_TO_TICKS math will overflow.
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanSetP2CanTimeout()
{
	// get host's new mS value
	gP2can = StrToInt32(&gHostBuf[2]);

	// convert mS to timer ticks 
	gP2can = MS_TO_TICKS(gP2can);
}		   

//------------------------------------------------------------------------------
// Function Name: CANClearRxErrors
// Description: Clears any errors and resets the RX receive buffer status.
//	NOTE: This routine not tested!
// Parameters:  
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanClearRxErrors()
{
    // Set CAN module back into the configuration mode									     
	CANSetOperationMode(CAN_OP_MODE_CONFIG);		
	
    RXB0CON_RXFUL = 0;
    RXB1CON_RXFUL = 0;
    COMSTAT_RX0OVFL = 0;
    COMSTAT_RX1OVFL = 0;
    PIR3_RXB0IF = 0;
    PIR3_RXB1IF = 0;
	
	// Set CAN module for normal RX & TX operation
	CANSetOperationMode(CAN_OP_MODE_NORMAL);					
}

//------------------------------------------------------------------------------
// Function Name: CanResetSegmentedRecvStats
// Description: Reset the segmented message tracking variables.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void CanResetSegmentedRecvStats()
{
	gFirstFrameRespondedECUs = 0;
	gFirstFrameDataLength[0] = 0;  // only the 1st one reset to 0 required
}

//------------------------------------------------------------------------------
// Function Name: CanIsSegmentedRecvComplete
// Description: Returns the status of the segmented message receive (i.e. 
//	ConsecutiveFrames). 
// Parameters: 
// Output Parameters:
// Return Values: RECV_NONE if no consecutive messages are being tracked, 
//	RECV_PENDING if one or more messages are still arriving, and RECV_COMPLETE
//	when all segemented messages have arrived. 
//------------------------------------------------------------------------------
int8 CanIsSegmentedRecvComplete()
{
	int8 cnt;
	if (gFirstFrameRespondedECUs == 0)
		return RECV_NONE;

	// search each segmented message byte count in list to check if all are <= 0 
	for (cnt=0; cnt<gFirstFrameRespondedECUs; cnt++)
	{
		if (gFirstFrameDataLength[cnt] > 0)
			return RECV_PENDING;
	}
	return RECV_COMPLETE;
}

//------------------------------------------------------------------------------
// Function Name: CanErrorIsr
// Description: Interrupt function to handle CAN bus errors. Sets the global
//	error flag to be handled by the main loop. Stops the CAN module from
//	continuing to retry transmissions. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
#INT_CANERR
void CanErrorIsr()
{
	// if only warning bits are set, just return and ignore
	//if (!(gErrorExtraData1 & 0xF8))
	//	return;

	if (gError == ERR_NONE)
	{
		// set error and save contents of COMSTAT register
		gError = ERR_CAN_ERROR_INTERRUPT;
		gErrorExtraData1 = COMSTAT;
	}
	
	CanDisable();
	
	// clear any overflow errors, if any
	bit_clear(COMSTAT, RXB0OVFL);
	bit_clear(COMSTAT, RXB1OVFL);
}

//------------------------------------------------------------------------------
// Function Name: CanIrxIsr
// Description: Interrupt function to handle CAN message TX and RX errors. Sets 
//	the global error flag to be handled by the main loop. Stops the CAN module from
//	continuing to retry transmissions.
//
//	This ISR fires on TX and RX errors. This ISR is used during auto-detection.
//	This ISRs job is to get off the bus as soon as possible and not clobber 
//	other vehicle's	communcation. When auto-detecting, we may transmit at the 
//	wrong speed and	trash other communications. If we didn't get off the bus 
//	fast enough on the the Mazda 6 2004 we'd generate U0100 and U0121 errors.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
#INT_CANIRX
void CanIrxIsr()
{
	// only FBGO commands will do a fast bus get off
    if (!IsCmdFBGO(gCurrentCmd))
	{
    	// acknowledge interrupt by clearing status
		bit_clear(PIR3, IRXIF); 
        return;
	} 

	if (gError == ERR_NONE)
	{
	    gError = ERR_CAN_INVALID_MSG_INTERRUPT;
		gErrorExtraData1 = TXB0CON;
		gErrorExtraData2 = TXB1CON;
		gErrorExtraData3 = TXB2CON;
	}
              
	CanDisable();
    
    // acknowledge interrupt by clearing status
    bit_clear(PIR3, IRXIF);
} 

//------------------------------------------------------------------------------
// Function Name: CanRx1 CanRx2
// Description: CAN receive interrupt functions. When the adapter is idle and
//	not actively obtaining a CAN message, then just discard any received messages.
//  One of these ISR(s) will fire every time a CAN message arrives. However,
// 	the adapter receives messages by polling when servicing a host request.
//	Therefore, these ISRs must be disabled during receive.
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
#INT_CANRX1
void CanRx1()
{
	CanEmptyRxBuffer();  
}
#INT_CANRX0
void CanRx0()
{
	CanEmptyRxBuffer();  
}









