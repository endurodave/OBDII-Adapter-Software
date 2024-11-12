//------------------------------------------------------------------------------
// File: Glob_bus.h
//
// Purpose: Bus Interface global variables.
//
// Change History:
// Date		Author		Description
//          Bernie		Created
//------------------------------------------------------------------------------
#ifndef GLOB_BUS_H
#define GLOB_BUS_H


//  ************ Global Bus Protocol Variable for VPW & PWM Routines ****************


int8	LinkControl;		// Data Link Control & Status Register
#locate LinkControl=0x26
	/* defines for the LinkControl variable bit positions */
#bit MsgRecvd = LinkControl.0		// An incomming message was received
#bit FrError = LinkControl.1		// A framing error was detected
#bit InFrResp = LinkControl.2		// An inframe response was received
#bit MsgSent = LinkControl.3		// The TX message has been sent
#bit MsgOverflow = LinkControl.4 	// A received message is longer than VEHICLE_MSG_SIZE
#bit NoBusActivity = LinkControl.5 	// Bus stuck, maximum bus acccess time exceeded


int8	LinkFlags;			// Local Data Link operation flags
#locate LinkFlags=0x27
#bit EOD			= LinkFlags.0	// End of Data Flag
#bit EOF 			= LinkFlags.1	// End of Frame Flag
#bit NxtBit 		= LinkFlags.2	// Value of next bit to transmit
#bit Act_phase_flag	= LinkFlags.3 	// VPW active phase flag

int8	ShiftReg;			// Data Byte shift register, need to be in Bank0
#locate ShiftReg=0x28
int8	MaskReg;			// Mask Register, needs to be in Bank0
#locate MaskReg=0x29
int8	PhaseTime;			// VPW active or passive phase time, needs to be in Bank0
#locate PhaseTime=0x2a
int8	BusByteCntr;		// Byte counter used in all PWM & VPW routines
#locate	BusByteCntr=0x2b
int8	BusBuffPntr_low;		// Buffer Pointer used in all PWM & VPW routines
#locate	BusBuffPntr_low=0x2c
int8	BusBuffPntr_high;		
#locate	BusBuffPntr_high=0x2d
int8	BusTimeOut;			// Used to determine if bus is stuck for error exit
#locate BusTimeOut=0x2e	
int16	BusBuffPntr;		// Buffer Pointer used in all PWM & VPW routines
#locate	BusBuffPntr=0x2f
int8	BSR_temp;			// Temporary storage for Bank Select Register
#locate	BSR_temp=0x31

#endif


