//------------------------------------------------------------------------------
// File: Timer
//
// Purpose: High speed timer (HST) routines. 
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//------------------------------------------------------------------------------
#include "Timer.h"

//------------------------------------------------------------------------------
// Function Name: HstTestTimer
// Description: Test a timer for timeout. 
// Parameters: timer - A timer that was read at start of delay.
//	    ticks - Number of ticks to test.
// Output Parameters:
// Return Values: If the number of ticks in 'ticks' has passed since the 
//	    initial reading of 'timer' return TRUE, else return	FALSE.
//------------------------------------------------------------------------------
BOOLEAN HstTestTimer(HstTimer timer, HstTimer ticks)
{
    // check to see if 'ticks' number of ticks have passed
    if (get_timer1() - timer >= ticks)
        return TRUE;
    else
        return FALSE;
}

//------------------------------------------------------------------------------
// Function Name: HstTestTimerRetrig
// Description: Test a timer for timeout, retrigger the timer. 
//	    This routine will then retrigger the given timer, so that subsequent reads 
//	    will again read false until the number of ticks in 'ticks' has again passed.
// Parameters: timer - Pointer to a timer that was read at start of delay.
//	    ticks - Number of ticks to test.
// Output Parameters:
// Return Values: If the number of ticks in 'ticks' has passed since the 
//	    initial reading of 'timer' return TRUE, else return	FALSE.
//------------------------------------------------------------------------------
BOOLEAN HstTestTimerRetrig(HstTimer *timer, HstTimer ticks)
{
    // check to see if 'ticks' number of ticks have passed
    if (get_timer1() - *timer >= ticks)
    {   
    	*timer += ticks;      // bring Timer up to date without accumulative errors
        return TRUE;
    }
    else
        return FALSE;
}

//-----------------------------------------------------------------------------
// Function Name: HstElapsedTime
// Description: Calculates the elapsed time. 
// Parameters: timer - Pointer to a timer that was read at start of delay.
// Output Parameters:
// Return Values: The elapsed time in ticks. 
//------------------------------------------------------------------------------
HstTimer HstElapsedTime(HstTimer timer)
{
    return (get_timer1() - timer);
}


