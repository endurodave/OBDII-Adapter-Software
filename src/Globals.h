//------------------------------------------------------------------------------
// File: Globals
//
// Purpose: Project global variables.
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef GLOBALS_H
#define GLOBALS_H

#include "Const.h"
#include "Timer.h"
                 
typedef struct 
{
    int8 comType : 3;
    int8 firstResp : 1;
	int8 forceInitCAN : 1;
    int8 canModeChanged : 1;  // true when mode changes (e.g. 250kb to 500kb, or SI to EI)
    int8 adapterLocked : 1;
	int8 enableLLineXmit : 1; 		// transmit at 10.4 on both K and L lines (default is K line only)
	int8 enableIso9600Baud : 1;		// transmit ISO at 9600 if true, false means 10.4k (default is 10.4k)
	int8 enableIso5msByteDelay : 1; // true means 5mS between each byte sent to vehicle, false no delay (default is true)
	int8 enableIsoChecksum : 1;		// add checksum byte to ISO messages if true, no byte added if false (default is true)
} Flags;

int8 gDevid1;
int8 gDevid2;

Flags gFlags = { MODE_ISO, FALSE };
int8 gError = ERR_NONE;
int8 gErrorExtraData1 = 0;
int8 gErrorExtraData2 = 0;
int8 gErrorExtraData3 = 0;

int8 gHostBuf[HOST_RECV_MSG_SIZE];
//*** #locate gHostBuf=0x2d  // pic18f248 should not require ram to be managed
int8 gHostBufCnt = 0;
int16 gHostRecvTimeout = 0;

int8 gVehicleBuf[VEHICLE_MAX_MSGS][VEHICLE_MSG_SIZE];
//*** #locate gVehicleBuf=0xa0

int8 gVehicleBufCnt = 0;

int8 gCurrentCmd = 0;   // current command sent from host
int8 gLastCmd = 0;      // last command sent from host

int32 gP2can = HST_FIFTY_MS;

// decyption variables
int16  gLfsr;
//int8 gLast;

#endif
