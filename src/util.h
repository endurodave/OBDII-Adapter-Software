//------------------------------------------------------------------------------
// File: Util
//
// Purpose: Misc utility routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef UTIL_H
#define UTIL_H

// starting address for the 4-byte EEPOM write protect table
#define EEPROM_WRITE_PROTECT_TABLE_ADDR 	252

// the logical sector size is arbitrary and has nothing to do with the PIC hardware
#define EEPROM_SECTOR_SIZE 	8

void ReadEeprom();
void ReadEepromLocal(int8 sector);
void WriteEeprom();
int8 IsEepromSectorWriteProtected(int8 sector);
int32 ReadEepromSectorWriteProtectTable();
void WriteEerpomSectorWriteProtectTable(int32 writeProtectBits);
void init_ports();
void Bus_If_Init(void);
void DecryptBlock(int8 *outbfr, int8 *inbfr);
int8 IsCmdCAN(int8 cmd);
int8 IsCmdLocal(int8 cmd);
int8 IsCmdFBGO(int8 cmd);
int8 IsCmdListen(int8 cmd);
int8 IsCmdNoResponse(int8 cmd);
int8 IsCmdFirstResp(int8 cmd);
int8 IsCmdStandardId(int8 cmd);
int8 IsCmd250kb(int8 cmd);
int8 checksum(int8* pMsg, int8 size);
int8 crc8(int8* buffer, int8 count);
int32 StrToInt32(int8* str);
int8 TraceStr(int8* pMsg, int8 size);
int8 Trace(int8 msg);

#endif
