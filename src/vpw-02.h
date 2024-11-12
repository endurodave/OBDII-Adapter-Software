#ifndef VPW_H
#define VPW_H

int8 VPWSend(int16* TxBufferPntr, int8 VPWTxCnt, int8 timeout);
int8 VPWRecv(int16* RxBufferPntr, int8* RxCntPntr, int8 timeout);

#endif
