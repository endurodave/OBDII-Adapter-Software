//------------------------------------------------------------------------------
// File: Timer
//
// Purpose: High speed timer (HST) routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#ifndef TIMER_H
#define TIMER_H

#define TIMER1_TIMEOUT          0     // 1.6uS per tick with div set to 8
#define MS_TO_TICKS(mS) 		((mS * 10000) / 16)	  // 1.6uS per tick

// max 16-bit number
#define HST_TWO_MS              1250
#define HST_TEN_MS              6250
#define HST_TWENTY_FIVE_MS      15625
#define HST_FIFTY_MS            31250
#define HST_FIVE_HUNDRED_MS		312500	// NOTE: this number exceeds 16-bit timer and must be used with Int32
#define HST_ONE_S				625000	// NOTE: this number exceeds 16-bit timer and must be used with Int32

typedef int16 HstTimer; //TBD DEBUG
//typedef int32 HstTimer;
            
HstTimer HstGetTicks(void) { return get_timer1(); }
BOOLEAN HstTestTimer(HstTimer timer, HstTimer ticks);
BOOLEAN HstTestTimerRetrig(HstTimer *timer, HstTimer ticks);
HstTimer HstElapsedTime(HstTimer timer);

#endif
