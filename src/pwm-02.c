//------------------------------------------------------------------------------
// File: pwm-01.c
//
// Purpose: PWM Bus Interface
//
// Change History:
// Date		Author		Description
//          Bernie		Created
// August 2003 - updated for operation with the PIC18F248 
//------------------------------------------------------------------------------

#include "glob_bus.h"
#include "util.h"

// *************** INITIALIZE TIMER 2 ****************************
// 
//  TIMER 2 IS INITIALIZED TO RUN AT 1/4 THE OSCILLATOR RATE (5MHZ = 200NS / CNT)
//  ITS LEFT TO FREE RUN, WE SET UP THE PR2 COMPARE REGISTER SO THAT THE COUNTER
//  AUTOMATICALLY RESTARTS AFTER REACHING THE COUNT SET IN THE PR2 REGISTER.
// 
//  WE WILL LOOP ON THE OVERFLOW BIT TO KNOW WHEN THE COUNT HAS ROLLED OVER
// 
//  THIS COUNTER IS BEING USED TO TIME THE TRANSMITTED BIT CELLS:
//  IN PWM, THE TIMER IS SET TO ROLLOVER EVERY 8US (24US BITCELL /3 = 8US)
// 
//  Here is the code to test if we have reached or exceeded a given timer value

//timer2_wait_loop
// 	btfss		PIR1, TMR2IF	//  the bit will be set on rollover
// 	goto		timer2_wait_loop
// 	bcf	PIR1, 	TMR2IF			//  Yes we timed out, now clear the bit & continue
// 	continue with code

void tmr2_init() {
	setup_timer_2 (T2_DIV_BY_1, 39, 1);
				// setup Timer2, as an 8us period timer (3 * 8us = 24us) a bit cell
				// Set Prescaler 1:1
				// 39 = count for a 8 us period ( 8us/200ns = 40 -1 decimal)
				// Set Postscale Count 1:1
}


// ************  Entry for Receiving a PWM message  ***********
// 
int8 PWMRecv(int16* RxBufferPntr, int8* RxCntPntr, int8 timeout) {
	BusTimeOut = timeout;   	// Maximum Bus Inactivity Time Out Counter 
								// approx 2 msec per count
	Bus_If_Init(); 				// Initialize local variables & timers
	BusBuffPntr = RxBufferPntr;
	BusBuffPntr_low = make8(BusBuffPntr, 0);  // load the pointer address for indirect addressing
	BusBuffPntr_high = make8(BusBuffPntr, 1);

	#asm
	movff	BusBuffPntr_low, FSR0L
	movff	BusBuffPntr_high, FSR0H

	bcf		PIR1, TMR2IF		// make sure timer 2 overflow bit is reset
wait_bus_activity1:
	btfss	PIR1, TMR2IF
	goto	wait_bus_activity2
	bcf		PIR1, TMR2IF
	decf	BusTimeOut, f		//check if Bus Access has timed out	
	btfsc	STATUS, Z
	goto	RxBusInactivityExit

wait_bus_activity2:
	clrf	TMR0L				// restart timer at 400ns per count
bus_active1:
	btfss 	PWM_In				// skips next inst if bus active
	goto	wait_bus_activity1
								// we have an active bus now
	movlw 	( 28000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// look for a minimum SOF pulse of 29us (in nanoseconds)
	btfss	STATUS, C			//  "C" is set if time equal or exceeded
	goto	bus_active1			// loop until we find a pulse longer than 28us
 
wait_SOF_drop:					// we have found a SOF, now wait for it to fall
	movlw 	( 38000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// Check if bus stays high longer than 38us (could be a break pulse)
	btfsc	STATUS, C			//  "C" is set if time equal or exceeded
	goto	FrError_Exit		// take the framing error exit if pulse is too long
	btfsc 	PWM_In				// check PWM_IN skip next inst when bus goes passive
	goto	wait_SOF_drop
								// SOF has dropped
	#endasm
	setup_timer_2( T2_DIV_BY_1, 255, 1 );			//  intialize timer2 for 200ns counting
	#asm 

wait_data_bit:					// now wait for rising edge of first data bit
	clrf	TMR2				// restart timer 2 to time bit cell
	btfsc 	PWM_In				// skips next inst as long as bus stays passive
	goto	first_bit
	movlw 	( 63000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// check if SOF has been down too long (frerror or break)
	btfss	STATUS, C			//  "C" is set if time equal or exceeded	
	goto 	wait_data_bit		// loop back & check bus state again
 	goto	FrError_Exit		// exit if 1st bit does not arrive in time

first_bit:						// start getting the data bits
get_data_bits:				
	movlw	0b00000001			// prime the RX shift register
	movwf	ShiftReg
get_inframe_response:			// entry point for getting the in-frame response bytes			
wait_bit_fall:
	btfss 	PWM_In				// skips next inst as long as bus stays active
	goto	the_bit_fell
	movlw 	( 22000 /200 )		// (desired time in ns)/200ns cylce time
	subwf	TMR2, W				// subtract from the current timer value
								// check if active phase of bit cell is up too long
	btfss	STATUS, C			//  "C" is set if time equal or exceeded	
	goto 	wait_bit_fall		// loop back & check bus state again
	goto	FrError_Exit		// it was up too long, take framing error exit
 
the_bit_fell:					// bit fell, now check if this was a one or a zero
	movlw 	( 12500 /200 )		// (desired time in ns)/200ns cylce time
	subwf	TMR2, W				// subtract from the current timer value
								// check if active phase of bit cell was longer than 12.5us
	btfss	STATUS, C			//  "C" is set if 12.5us has been equaled or exceeded
	goto	shift_in_a_1		// it was a 1 if less than 12.5us
	bcf		STATUS, C			// clear C becuse we want to rotate in a zero	
	goto 	shift_it_in			// it was a 0 if more than 12.5us


read_another_bit:
	clrf	TMR2				// clear timer to time next bit
	goto	wait_bit_fall

FrError_Exit:
	bsf		FrError 			// set framing error flag in link control register
	goto	RX_PWM_Exit

MsgError_Exit:
	bsf		MsgOverflow			// set overflow error flag in link control register
	goto	RX_PWM_Exit

RxBusInactivityExit:
	bsf		NoBusActivity		// set the error flag bit in LinkControl
	goto	RX_PWM_Exit2		// error exit if bus is stuck busy

					
shift_in_a_1:
	bsf		STATUS, C			// set C for rotation of a 1 into register
shift_it_in:
	rlcf	ShiftReg, F			// rotate the C bit into the LSB position (data comes in MSB first)
								// after 8 rotations, the first bit in will be in the MSB position
	btfss	STATUS, C			// register is full when the preset one bit comes out
	goto	wait_strt_nxt_bit  	// branch if register not full yet
								// shift register is full, save the byte

	incf	BusByteCntr, F		// add one to the received bytes count
	movlw	VEHICLE_MSG_SIZE+1	// check if we have exceeded the maximum msg size
	subwf	BusByteCntr, W
	btfsc	STATUS, C			// 'C' will be set when we are oversize
	goto	MsgError_Exit		// its an error if we are over the max byte count

	movf	ShiftReg, W			// shift register is full, save the accumulated byte to RxBuffer
 	movwf	INDF0
	incf	FSR0L, F			// advance indirect memory pointer to next byte	
	movlw	0b00000001			// prime the shift register again
	movwf	ShiftReg			
			

//  **** delete support for In Frame Response
//InFrame_Ck:					// now lets check if there is an inframe response
//	btfsc 	PWM_In				// check to see if inframe response active phase shows up			
//	goto 	Ck_InFrame_Status	// executes this inst if bus goes back active
//	movlw 	( 66000 /400 )		// (desired time in ns)/400ns cylce time
//	subwf	TMR0L, W			// subtract from the current timer value
//								// check if bus stays passive longer than 66us
//	btfss	STATUS, C			//  "C" is set if 66us has been equaled or exceeded
//	goto	InFrame_Ck			// loop back & wait until bus is inactive for 66us
//  ******************

//  **** delete support for In Frame Response
//Ck_InFrame_Status:
//	clrf	TMR0L				// restart timer0 to start timing 1st inframe response bit
//	btfsc	InFrResp			// test if we have already detected an inframe response
//	goto	FrError_Exit		// this is an error if already found an inframe response
//	bsf		InFrResp 			// Set the inframe response detected status flag
//	goto	get_inframe_response	// go receive the inframe message
//  **********************		

wait_strt_nxt_bit:
	btfsc 	PWM_In				// check to see if we get another data bit comming			
	goto 	read_another_bit	// executes this inst if bus is active
	movlw 	( 29500 /200 )		// (desired time in ns)/200ns cylce time
	subwf	TMR2, W				// subtract from the current timer value
								// check if bus stays passive longer than 29us
	btfss	STATUS, C			//  "C" is set if 29us has been equaled or exceeded
	goto	wait_strt_nxt_bit
	bsf		MsgRecvd			// set the message received flag
 
end_of_data_ck:					// now look for end of frame sequence
	btfsc 	PWM_In				// check to see that another active phase doesnt show up			
	goto 	FrError_Exit		// executes this inst if bus goes back active
								// if it does, its an error
	movlw 	( 40000 /200 )		// (desired time in ns)/200ns cylce time
	subwf	TMR2, W				// subtract from the current timer value
								// check if bus stays passive longer than 45us
	btfss	STATUS, C			//  "C" is set if 45us has been equaled or exceeded
	goto	end_of_data_ck		// loop back & wait until until the bus is inactive for 45us
RX_PWM_Exit:
	bsf		EOD					//  set the end of data flag
	bsf		EOF					//  set end of frame flag
RX_PWM_Exit2:
	#endasm
	*RxCntPntr = BusByteCntr;	// return count of received bytes
	return LinkControl;			// return status of the read operation
}

// ###################################################################
// ***************  Entry for PWM Transmit Routine  ******************	
// 
int8 PWMSend(int16* TxBufferPntr, int8 PWMTxCnt, int8 timeout) {
	BusTimeOut = timeout;   	// Maximum Bus Inactivity Time Out Counter 
								// approx 2 msec per count
    #asm
    lost_arbitration2:			// internal, lost arbitration re-entry point
    #endasm

	Bus_If_Init(); 				// Initialize local variables & timers

	BusBuffPntr = TxBufferPntr;
	BusBuffPntr_low = make8(BusBuffPntr, 0);  // load the pointer address for indirect addressing
	BusBuffPntr_high = make8(BusBuffPntr, 1);

	BusByteCntr = PWMTxCnt;		// load the the TxCnt (being pointed at)

	#asm
	bcf		PIR1, TMR2IF		// make sure timer 2 overflow bit is reset
	movff	BusBuffPntr_low, FSR0L
	movff	BusBuffPntr_high, FSR0H

EOF_wait1:
	movf	BusTimeOut, f		// check if Bus Access Time has timed out
	btfsc	STATUS, Z
	goto	TxBusBusyExit		// error exit if bus is stuck busy
	btfss	PIR1, TMR2IF		// check if bus timeout period counter has expired
	goto	EOF_wait3			// jump if timer has not timed out yet
	bcf		PIR1, TMR2IF		// reset timer overflow flag
	decf	BusTimeOut, f		// sub one timer tick from timeout count

EOF_wait3:
	clrf	TMR0L				// restart timer
EOF_wait2:
	btfsc 	PWM_In				// skips next when bus is passive
	goto	EOF_wait1			// loop back & clear timer whenever bus is active

	movlw 	( 70000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value 
								// check if bus stays passive longer than 70us for EOF
	btfss	STATUS, C			//  "C" is set if 70us has been equaled or exceeded
	goto	EOF_wait2			// loop as long as active, checking for 70us duration

IFS_wait:						// End of Frame found, now check if anyone else starts
								// after 70 us if any other node sends a SOF, we can
								// put one out as well, even if before the 96us is up
	btfsc 	PWM_In				// executes next inst is bus goes active
	goto	strt_transmission	// branch out if bus goes active
	movlw 	( 96000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// check if bus stays passive longer than 96us for IFS
	btfss	STATUS, C			//  "C" is set if 96us has been equaled or exceeded
	goto	IFS_wait			// continue checking til a SOF shows up, or the 96 us times out
 
strt_transmission:				// we have seen a SOF or the 96us IFS time has expired
								// in either case, we now drive our own 32us SOF pulse
	clrf	TMR0L				// restart the timer
	bcf		PORTA, PWM_P_Out	// set PWM Bus Active
	bsf		PORTA, PWM_N_Out	// P=0, N=1
	#endasm
	tmr2_init();				// setup Timer2, as an 8us period timer (3 * 8us = 24us) a bit cell
	
	#asm
SOF_delay:
	movlw 	( 30000 /400 )		// (desired time ns startup time)/400ns cylce time
								// nominal per spec is 32 us (use 1us shorter for decay time on bus)
	subwf	TMR0L, W			// subtract from the current timer value
								// check if bus stays passive longer than 32us for IFS
								//  subtracted 1 us for software & circuit delays
	btfss	STATUS, C			//  "C" is set if 32us has been equaled or exceeded
	goto	SOF_delay			// drive the SOF pulse for the full 32us
 
	bsf		PORTA, PWM_P_Out	// set PWM Bus Inactive
	bcf		PORTA, PWM_N_Out	// P=1, N=0			
								// drive BUS passive to terminate SOF pulse
 
start_sending_bits:
settle_delay:					// allow the bus to settle, per spec should be 1.7us max
	movlw 	( 34000 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// allow 2.4us settle time (34.4-32=2.4us) for bus to go passive
	btfss	STATUS, C			//  "C" is set if 35us has been equaled or exceeded
	goto	settle_delay		// wait the 2.4 us settle time
 
	btfsc 	PWM_In				// skips next inst if bus is still passive
	goto	lost_arbitration	// Oh-oh, the bus is being driven, we loose arbitration				
								// get the first bit to transmit
 	bcf		NxtBit				// Clear NxtBit in advance of test
	btfsc	INDF0, 7			// test the MSbit of the first byte in the buffer

	bsf		NxtBit				// if the MSB wasn't a zero, set a one into NxtBit
 
SOF_passive_delay:				// keep checking to make sure no one else starts driving the bus
	btfsc 	PWM_In				// skips next inst if bus is still passive
	goto	lost_arbitration	// Oh-oh, the bus is being driven, we loose arbitration
	movlw 	( 45500 /400 )		// (desired time in ns)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// wait for the 16us of passive time after the active SOF pulse
	btfss	STATUS, C			//  "C" is set if 47us has been equaled or exceeded
	goto	SOF_passive_delay	// wait the 16us of passive SOF time
 
	clrf	TMR2				// re-start (zero) timer2 to synchronize it with end of SOF time
	bcf		PIR1, TMR2IF		// clear timer 2 interrupt flag, in case it was set
	nop							// actual timer2 interrupts are not enabled, we will be testing the flag bit
	nop							// a few nop's to compenstate for TMR2 startup time after reset
	nop							//  of prescaller / postscaler caused by write to TMR2
	nop
	nop
 
strt_bit_cell:
	bcf		PORTA, PWM_P_Out	// set PWM Bus Active
	bsf		PORTA, PWM_N_Out	// P=0, N=1 for start of bit cell
	clrf	TMR0L				// restart timer 0
	btfss	NxtBit				// test if the next bit is a one or a zero
	goto	its_a_zero
its_a_one:
	btfss	PIR1, TMR2IF		// check if 1st 8us time period is complete yet
	goto	its_a_one
	bsf		PORTA, PWM_P_Out	// set PWM Bus Inactive
	bcf		PORTA, PWM_N_Out	// P=1, N=0			
								// for a one, go passive after 8us of active time
	bcf		PIR1, TMR2IF		// reset the timer overflow bit

wait_half_cell:
	movlw 	( 12500 /400 )		// (desired time ns startup time)/400ns cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// wait 4.5us of passive time (halfway between a one & a zero)
								// 8us one time + 4.5us = 12.5us 
	btfss	STATUS, C			//  "C" is set when 12.5us has been equaled or exceeded
	goto	wait_half_cell		// wait the 4.5us
	btfsc 	PWM_In				// skips next inst if bus is still passive
	goto	lost_arbitration	// Oh-oh, the bus is being driven, we loose arbitration
	goto	wait_2nd_half

TxBusBusyExit:
	bsf		NoBusActivity		// set the error flag bit in LinkControl
	goto	TxErrorExit
 
its_a_zero:						// a zero drives the bus active for 2, 8us timer periods
	btfss	PIR1, TMR2IF		// wait for 1st 8us time period to complete
	goto	its_a_zero
	bcf		PIR1, TMR2IF		// reset the timer overflow bit		
wait_2nd_half:			
	btfss	PIR1, TMR2IF		// wait for 2nd 8us time period to complete
	goto	wait_2nd_half
	bsf		PORTA, PWM_P_Out	// set PWM Bus Inactive
	bcf		PORTA, PWM_N_Out	// P=1, N=0
								// after the 16us go set BUS passive
	bcf		PIR1, TMR2IF		// reset the timer overflow bit
 
get_next_bit:					// get next bit to transmit
	bcf		STATUS, C			// make sure 'C' bit is clear before rotating
	rrcf	MaskReg, F			// Move Mask to next bit position
	btfsc	STATUS, C			// if C is set, the entire byte has been sent
	goto	get_next_byte		// setup the next byte
test_the_bit:					// check if the masked bit is a one or a zero
	movf	MaskReg, W			// fetch the Mask Register
	andwf	INDF0, W			// and the mask with current byte being transmitted (indirect)
	bcf		NxtBit
	btfss	STATUS, Z			// skip next instruction if bit was a zero
	bsf		NxtBit				// NxtBit is now set up for the next bit to be sent, now
 
wait_cell_passive:				// wait for the 8us of passive time that marks end of bit cell
	btfsc  	PWM_In				// skips next inst if bus is still passive
	goto	lost_arbitration	// Oh-oh, the bus is being driven, we loose arbitration
	btfss	PIR1, TMR2IF		// wait for 3rd & final 8us time period to complete (24us bit cell)
	goto	wait_cell_passive
	bcf		PIR1, TMR2IF		// reset the timer overflow bit
	goto	strt_bit_cell		// go back to transmit the next bit
 
get_next_byte:
	incf	FSR0L, F			// increment the TX buffer pointer
	decf	BusByteCntr, F		// sub one from the count of bytes to be sent
	btfsc	STATUS, Z			// have we sent them all yet
	goto	all_sent			// exit loop if we are done sending
	movlw	0x80
	movwf	MaskReg				// put a one back into the MSB position
	goto	test_the_bit		// go transmit the MSB bit of this new byte

lost_arbitration:
	movf	BusTimeOut, f		// Check the bus access time out count to see
	btfss	STATUS, Z			// if its already zero	
	decf	BusTimeOut, f		// reduce bus time out on each lost arbitration cycle
	goto	lost_arbitration2			
 
all_sent:
	bsf		MsgSent				// set the Message Sent Flag
//	movlw	RxBuffer
//	movwf	FSR0L				// setup RxBuffer pointer in case of in-frame response
//	movlw	0b00000001			// prime first RxBuffer byte as a shift register
//	movwf	INDF0

TX_PWM_Exit:
	bsf		EOD					//  set the end of data flag
	bsf		EOF					//  set end of frame flag
TxErrorExit:
	#endasm
	return LinkControl;			// return the status of the transmit operation
}
	

// ##############   End of pwm-xx.asm    ########################


