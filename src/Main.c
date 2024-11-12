//------------------------------------------------------------------------------
// File: Main
//
// Purpose: Main module and C entry point main().
//
//  Notes: 
//  Software UART in CCS compiler does not implement a stop bit. Therefore
//  you must timeout a stopbit after a putc call. delay_us(500) has worked. 
//
//  TODO: 
//  1) Make an ISO command that doesn't wait 5mS per char for faster sampling. 
//  2) Check all TBDs
//  3) Check that all commands and errors are used/handled.
//  4) Create a security function and message using encryption.
//  5) VW Pass through mode does no bus message filtering? Or maybe just no init?
//  6) PWM needs to send an IFR.
//
// Change History:
// Date		Author		Description
//          Lafreniere	Created
//
// August 12, 2003 - Updated for compatibility with the PIC18F248, for CAN version of product
// Feburary 26, 2005 - Update to add the B0 adapter unlock command
// August 3, 2007 - Update to add DashDyno code, host UART speed increase and EERPOM commands.
// November, 22, 2008 - Updated to add support for Toyota enhanced KWP 9600 mode.
//------------------------------------------------------------------------------
#case       	// make compiler case sensitive

// define to enable debug code
//#define DEBUG   1   

// define to compile for DashDyno
#define DASH_DYNO
#define BLUETOOTH

#include <18F248.h>
#device ADC=10  // setup A/D for 10 bits

#include "a_18F248.h"
#include "hardware.h"
#include "Const.h"
#include "Globals.h"

#include "PWM-02.c"
#include "VPW-02.c"
#include "CAN18XX8.c"
#include "Vehicle.c"
#include "Host.c"
#include "CAN.c"
#include "Timer.c"
#include "Util.c"

//------------------------------------------------------------------------------
// Function Name: main
// Description: C program main entry point. 
// Parameters: 
// Output Parameters:
// Return Values: 
//------------------------------------------------------------------------------
void main()
{
	gFlags.adapterLocked = TRUE;
	gFlags.enableIso5msByteDelay = TRUE;
	gFlags.enableIsoChecksum = TRUE;
	
    init_ports();

	CanDisable();

    setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_8 );
    
    setup_adc_ports(RA0_ANALOG);
    
	gDevid1 = read_program_eeprom(DEVID1_ADDR);
	gDevid2 = read_program_eeprom(DEVID2_ADDR); 
	 
	// on old 18F248 part use the CCS built-in functions to setup ADC
	if (gDevid2 == VER_18F248)
	{
		setup_adc(ADC_CLOCK_DIV_64);
		set_adc_channel(0);
	}
	// on new 18F2480 part must setup manually as the ADC registers have changed
	else
	{		
		ADCON2 = 0xAD;
		ADCON0 = 0x01;
	}
    	
	enable_interrupts(GLOBAL);
    enable_interrupts(INT_RDA);
	enable_interrupts(INT_CANERR);
	enable_interrupts(INT_CANRX1);
	enable_interrupts(INT_CANRX0);
	enable_interrupts(INT_CANIRX);

   	//output_low(PIN_B4);
   	//output_high(PIN_B4);
	//TRACE("A"); // send byte to get debug port initialized
	
#ifdef BLUETOOTH
	HostInitBluetooth();
#endif

    // forever loop
    while(1)
    {
		// if a complete host message arrived
        if (gHostBufCnt >= HOST_RECV_MSG_SIZE)
        {
            // ensure serial interrupts don't disturb accurate VPW and PWM timing loops
            disable_interrupts(INT_RDA);
					   
			// incase an ISR set an error while adatper was idle, let's clear it
			gError = 0;

            // if the incoming message parses successfully
            HostParseMsg();
			
            // send the host's query message to the vehicle
            if (!gError)
		    {
            	gCurrentCmd = gHostBuf[IDX_HOST_COMMAND];
                VehSendRecv();
			}
                
            // if error generated, send host error message
            if (gError)
                HostSendError();

			// else if vehicle data to send, send host vehicle data message
            else if (!IsCmdLocal(gCurrentCmd))
               HostSendVehicleData();

			// else local data to send, send host local data message
			else
				HostSendLocalData();

            gHostBufCnt = 0;
          	gLastCmd = gCurrentCmd;
            
            // zero all vehicle buffer bytes for next time
            //memset(gVehicleBuf, 0x00, VEHICLE_MAX_MSGS * VEHICLE_MSG_SIZE);
            enable_interrupts(INT_RDA);
        }

        // if more than 25mS between chars, reset buf count. Could happen if
        // an incomplete host message is received or host is slow
        disable_interrupts(INT_RDA);
        if (HstTestTimer(gHostRecvTimeout, HST_TWENTY_FIVE_MS))
            gHostBufCnt = 0;
        enable_interrupts(INT_RDA);
    } 
}
