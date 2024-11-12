//------------------------------------------------------------------------------
// File: CAN
//
// Purpose: CAN communication and parsing routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef CAN_H
#define CAN_H

void VehSendCAN();
void VehRecvCAN();
void CanEmptyRxBuffer();
void CanSetMaskFilter();
void CanSetP2CanTimeout();
void CanDisable();

#endif