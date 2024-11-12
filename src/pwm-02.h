#ifndef PWM_H
#define PWM_H

int8 PWMSend(int16* TxBufferPntr, int8 PWMTxCnt, int8 timeout);
int8 PWMRecv(int16* RxBufferPntr, int8* RxCntPntr, int8 timeout);

#endif
