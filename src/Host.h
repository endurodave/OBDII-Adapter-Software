//------------------------------------------------------------------------------
// File: Host
//
// Purpose: Host communication and parsing routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef HOST_H
#define HOST_H

BOOLEAN HostParseMsg();
void HostSendVehicleData();
void HostSendLocalData();
void HostSendError();

#endif
