//------------------------------------------------------------------------------
// File: vpw-01.c
//
// Purpose: VPW Bus Interface
//
// Change History:
// Date		Author		Description
//          Bernie		Created
// August 2003 - updated for operation with the PIC18F248 
//------------------------------------------------------------------------------

#include "glob_bus.h"
#include "Util.h"

// ************  Entry for Receiving a VPW message  ***********

int8 VPWRecv (int16* RxBufferPntr, int8* RxCntPntr, int8 timeout) {
	BusTimeOut = timeout;   	// Maximum Bus Inactivity Time Out Counter 
								// approx 2 msec per count
	Bus_If_Init(); 				// Initialize local variables & timers
	BusBuffPntr = RxBufferPntr;
	BusBuffPntr_low = make8(BusBuffPntr, 0);  // load the pointer address for indirect addressing
	BusBuffPntr_high = make8(BusBuffPntr, 1);

	setup_timer_0( RTCC_INTERNAL | RTCC_DIV_8 );	//  intialize & start timer0 at 1.6us per count

	#asm
//	movff	BSR_temp, BSR		// Save current setting of Bank Select Register
///	movlb	0					//Set BSR to bank zero 
	movff	BusBuffPntr_low, FSR0L
	movff	BusBuffPntr_high, FSR0H

	bcf		PIR1, TMR2IF		// make sure timer 2 overflow bit is reset
wait_bus_activity_v:
	btfss	PIR1, TMR2IF
	goto	wait_bus_activity2_v
	bcf		PIR1, TMR2IF
	decf	BusTimeOut, f		//check if Bus Access has timed out (300 msec max)	
	btfsc	STATUS, Z
	goto	Rx_BusInactivityExit

wait_bus_activity2_v:
	clrf	TMR0L				//  restart timer0
bus_active1_v:
	btfss PORTA, VPW_In			//  skips next inst if bus active
	goto	wait_bus_activity_v
								//  we have an active bus now
								// calculate the count of time to compare to in nanoseconds
	movlw 	( 163000/1600 )		// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
								// The "C" bit is a one when the time has been equaled or exceeded	
								//  look for a minimum SOF pulse of 163us (in nanoseconds)
	btfss	STATUS, C			//  "C" is set if time equal or exceeded
	goto	bus_active1_v		//  loop until we find a pulse longer than 163us
 
wait_SOF_drop_v:				//  we have found a SOF, now wait for it to fall
								//  Check if bus stays high longer than 239us (could be a break pulse)
	movlw 	( 239000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value									
	btfsc	STATUS, C			//  "C" is set if time equal or exceeded
	goto	FrError_Exit_v		//  take the framming error exit if pulse is too long
	btfsc	PORTA, VPW_In		//  skip next inst when bus goes passive
	goto	wait_SOF_drop_v
								//  SOF has dropped

first_bit_v:					//  start getting the data bits
get_inframe_response_v:			//  entry point for getting the in-frame response bytes
	clrf	TMR0L				//  restart timer 0 to time bit cell		
	bcf		Act_phase_flag
get_data_bits_v:				
	movlw	0b00000001			//  prime the shift register
	movwf	ShiftReg


passive_phase:					//  decode if a one or a zero during this passive phase period
								//  in passive phase, 64us nom pulse is a zero (34us min - 96us max)
								//  in passive phase, 128us nom pulse is a one (96us min - 163us max)
	btfss	PORTA, VPW_In		//  wait for the current passive phase to end
	goto	still_passive		//  if still passive go check that if it has exceeded max time of 163us
	movf	TMR0L, W			//  Fetch current value of timer, the duration of this pulse
	movwf	PhaseTime			//  temporarily save it
	clrf	TMR0L				//  reset the timer to start timing the phase that just started
	bsf		Act_phase_flag  	//  mark the polarity of this new phase that we just detected
								//  check to see if pulse was too short, under 34us long
	movlw 	( 35000 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	PhaseTime, W		// subtract from the measured phase time
	btfss	STATUS, C			//  "C" is set if 34us has been equaled or exceeded
	goto	FrError_Exit_v		//  its an error if pulse was too short
								//  check if the phase time was less than 96us
	movlw 	( 95500 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	PhaseTime, W		// subtract from the measured phase time	
	btfss	STATUS, C			//  "C" is set if 96us has been equaled or exceeded		
	goto	shift_in_a_0_v		//  short passive is a zero
	goto	shift_in_a_1_v		//  long passive is a one
 
still_passive:
								//  check if bus stays passive longer than 164us
	movlw 	( 164000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set if 163us has been equaled or exceeded
	goto	passive_phase		//  under 163, loop back & check bus polarity again
	goto	end_of_data_ck_v	//  been down for 163us or more, go check if its the end of data	


active_phase:					//  decode if a one or a zero during this active phase period
								//  in active phase, 64us nom pulse is a one (34us min - 96us max)
								//  in active phase, 128us nom pulse is a zero (96us min - 163us max)
	btfsc	PORTA, VPW_In		//  wait for the current active phase to end
	goto	still_active		//  if still active go check that if it has exceeded max time of 163us
	movf	TMR0L, W			//  Fetch current value of timer, the duration of this pulse
	movwf	PhaseTime			//  temporarily save it
	clrf	TMR0L				//  reset the timer to start timing the phase that just started
	bcf		Act_phase_flag  	//  mark the polarity of this new passive phase that we just detected
								//  check to see if pulse was too short, under 34us long
	movlw 	( 33500 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	PhaseTime, W		// subtract from the measured phase time
	btfss	STATUS, C			//  "C" is set if 34us has been equaled or exceeded
	goto	FrError_Exit_v		//  its an error if pulse was too short
								//  check if the phase time was less than 96us
	movlw 	( 95500 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	PhaseTime, W		// subtract from the measured phase time
	btfss	STATUS, C			//  "C" is set if 96us has been equaled or exceeded		
	goto	shift_in_a_1_v		//  short active is a one
	goto	shift_in_a_0_v		//  long passive is a zero

still_active:
								//  check if bus stays passive longer than 164us
	movlw 	( 164000 /1600 )	// (desired time in ns) /1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set if 163us has been equaled or exceeded
	goto	active_phase		//  under 163, loop back & check bus polarity again
	goto	FrError_Exit_v		//  been active for 163us or more, must be an error or Break	

 
shift_in_a_0_v:					//  clear C for rotating a zero into register
	bcf		STATUS, C
	goto	shift_it_in_v
					
shift_in_a_1_v:	
	bsf		STATUS, C			//  set C for rotating a 1 into register
shift_it_in_v:
	rlcf	ShiftReg, F			//  rotate the C bit into the LSB position (data comes in MSB first)
								//  after 8 rotations, the first bit in will be in the MSB position
	btfss	STATUS, C			//  register is full when the preset one bit comes out
	goto	get_nxt_bit_v	  	//  branch if register not full yet
								//  shift register is full, save the byte

	incf	BusByteCntr, F		//  add one to the received bytes count
	movlw	VEHICLE_MSG_SIZE+1	//  check if we have exceeded the maximum of 12 bytes
	subwf	BusByteCntr, W
	btfsc	STATUS, C			//  'C' will be set when we are over maximum record length
	goto	MsgLen_Exit_v		//  its an error if we are over the msg size limit

	movf	ShiftReg, W			// shift register is full, save the accumulated byte to RxBuffer
	movwf	INDF0
	incf	FSR0L, F			//  advance indirect memory pointer to next byte	
	movlw	0b00000001			//  prime the RxBuffer byte as a shift register
	movwf	ShiftReg			//  indirect write to the buffer

get_nxt_bit_v:
	btfss	Act_phase_flag  	//  branch to either active or passive phase bit decode
	goto	passive_phase		//  in VPW acive & passive phases alternate on every bit
	goto	active_phase
 
end_of_data_ck_v:				
	bsf		MsgRecvd			//  set the message received flag
	bsf		EOD					//  set the end of data flag
// ***  Following Statements Removed to elliminate In-Frame Response Checking ****
// ***  Per discussion with David 8-13-2002
//InFrame_Ck_v:					//  now lets check if there is an inframe response
//	btfsc	PORTA, VPW_In		//  check to see if inframe response active phase shows up			
//	goto 	Ck_InFrame_Status_v	//  executes this inst if bus goes back active
//								//  check if bus stays passive longer than 239us
//	movlw 	( 239000 /1600 )	// (desired time in ns)/1.6us cylce time
//	subwf	TMR0L, W			// subtract from the current timer value
//	btfss	STATUS, C			//  "C" is set if 239us has been equaled or exceeded
//	goto	InFrame_Ck_v		//  loop back & wait until bus is inactive for 239us
// ***
	bsf		EOF					//  we have identified an end of frame
	goto	VPW_RX_Exit			//  return to the calling "C" application
	

// ***  Following Statements Removed to elliminate In-Frame Response Checking ****
// ***  Per discussion with David 8-13-2002 
//Ck_InFrame_Status_v:
//	clrf	TMR0L				//  restart timer0 to start timing 1st inframe response bit
//	btfsc	InFrResp			//  test if we have already detected an inframe response
//	goto	FrError_Exit_v		//  this is an error if already found an inframe response
//	bsf		InFrResp 			//  Set the inframe response detected status flag
// 
//wait_NB_done:					//  wait for the VPW normalization bit to end (throw it away)
//	btfss	PORTA, VPW_In
//	goto	NB_done
//								//  test to make sure the maximum pulse time of 96us is not exceeded
//	movlw 	( 99000 /1600 )		// (desired time in ns)/1.6us cylce time
//	subwf	TMR0L, W			// subtract from the current timer value
//	btfsc	STATUS, C			//  "C" is set if 99us has been equaled or exceeded
//	goto	FrError_Exit_v		//  take error exit if pulse is too long
//	goto	wait_NB_done		//  loop until pulse ends
//
//
//NB_done:						//  the NB terminated, now lets make sure it was correct duration
//								//  its an error if bus was active less than 34us
//	movlw 	( 33000 )/1600 )	// (desired time in ns)/1.6us cylce time
//	subwf	TMR0L, W			// subtract from the current timer value
//	btfss	STATUS, C			//  "C" is set if 34us has been equaled or exceeded
//	goto	FrError_Exit_v		//  take error exit if too short	
//	goto	get_inframe_response_v	//  go receive the inframe message
// ****

MsgLen_Exit_v:
	bsf		MsgOverflow			// set overflow error flag in link control register	
	goto	VPW_RX_Exit
	
Rx_BusInactivityExit:
	bsf		NoBusActivity		// set the error flag bit in LinkControl
	goto	VPW_RX_Exit			// error exit if bus is stuck busy

FrError_Exit_v:					//  time exceeded or other problem if we get here
	bsf		FrError 			//  set framing error flag in control register
VPW_RX_Exit:
//	movff	BSR_temp, BSR		// Restore setting of Bank Select Register
	#endasm

	*RxCntPntr = BusByteCntr;	// return count of received bytes
	return LinkControl;			// return status for the calling application
}		
 
// ###################################################################
// ***************  Entry for VPW Transmit Routine  ******************	
//
int8 VPWSend (int16* TxBufferPntr, int8 VPWTxCnt, int8 timeout) {
	BusTimeOut = timeout;   	// Maximum Bus Inactivity Time Out Counter 
								// approx 2 msec per count
    #asm
    lost_arbitration_v2:		// internal, lost arbitration re-entry point
    #endasm
    
	Bus_If_Init(); 				// Initialize local variables & timers
	BusBuffPntr = TxBufferPntr;
	BusBuffPntr_low = make8(BusBuffPntr, 0);  // load the pointer address for indirect addressing
	BusBuffPntr_high = make8(BusBuffPntr, 1);

	#asm
//	movff	BSR_temp, BSR		// Save current setting of Bank Select Register
//	movlb	0					//Set BSR to bank zero
	movff	BusBuffPntr_low, FSR0L
	movff	BusBuffPntr_high, FSR0H
	#endasm

	BusByteCntr = VPWTxCnt;		// load the the TxCnt (being pointed at)

	setup_timer_0( RTCC_INTERNAL | RTCC_DIV_8 ); // initialize timer0 to run at 200ns / count

	#asm
	bcf		PIR1, TMR2IF		// make sure timer 2 overflow bit is reset
EOF_wait1_v:
	btfss	PIR1, TMR2IF
	goto	EOF_wait3_v			// jump if timer has not timed out yet
	bcf		PIR1, TMR2IF
	movf	BusTimeOut, f		// check if Bus Access has timed out	
	btfsc	STATUS, Z
	goto	TxBusBusyExit_v		// error exit if bus is stuck busy
	decf	BusTimeOut, f		// sub one timer tick from timeout count

EOF_wait3_v:
	clrf	TMR0L				// restart timer
EOF_wait2_v:
	btfsc	PORTA, VPW_In		// skips next when bus is inactive
	goto	EOF_wait1_v			// loop back until we see an inactive bus

								// check if bus stays passive longer than 239us for EOF
	movlw 	( 239000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			// "C" is set if 239us has been equaled or exceeded
	goto	EOF_wait2_v			// loop as long as passive

IFS_wait_v:						// End of Frame found, now check if anyone else starts
								// after 239 us, if any other node sends a SOF, we can
								// put one out as well, even before the 300us IFS is up
	btfsc	PORTA, VPW_In		// executes next inst is bus goes active again
	goto	strt_transmission_v	// branch out if bus goes active
								// check if bus stays passive longer than 300us for IFS
	movlw 	( 300000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			// "C" is set if 300us has been equaled or exceeded
	goto	IFS_wait_v			// continue checking til a SOF shows up, or the 300 us times out

strt_transmission_v:			// we have seen a SOF or the 300us IFS time has expired
								// in either case, we now drive our own 200us SOF pulse
	clrf	TMR0L				// restart the timer
	bsf		PORTB, VPW_Out		// drive VPW BUS active
	
SOF_delay_v:
								// Leave bus active for 200us
	movlw 	( 200000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			// "C" is set if 32us has been equaled or exceeded
	goto	SOF_delay_v			// drive the SOF pulse for the full 200us

	bcf		PORTB, VPW_Out		// drive BUS passive to terminate SOF pulse
	clrf	TMR0L				// restart the timer

start_sending_bits_v:
settle_delay_v:					// allow the bus to settle, per Moto 33390 spec should be 17.7us max
								// allow 21us settle time for bus to go passive
	movlw 	( 21000 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			// "C" is set if 35us has been equaled or exceeded
	goto	settle_delay_v		// wait the 21 us settle time

	btfsc	PORTA, VPW_In		// skips next inst if bus is still passive
	goto	lost_arbitration_v	// Oh-oh, the bus is being driven, we loose arbitration				
								// get the first bit to transmit
 	bcf		NxtBit				// NxtBit is already zero because of being reset at start
	btfsc	INDF0, 7			// test the MSbit of the first byte in the buffer
	bsf		NxtBit				// if the MSB wasn't a zero, set a one into NxtBit

TX_passive_phase:
	btfsc	NxtBit				// Check if next bit is a zero or a one
	goto	passive_1			// branch if its a one
passive_0:
	call	get_next_bit_v		// setup the next bit to be transmitted
passive_0_ck:
	btfsc	PORTA, VPW_In		// check if someone else may be driving the bus
	goto	lost_arbitration_v	// exit routine if someone else is driving the bus
								// leave bus active for 64us for a passive 0 pulse
	movlw 	( 64000 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			// "C" is set when 64us has been equaled or exceeded
	goto	passive_0_ck		// loop until pulse is finished
 
end_passive:
	btfsc	MsgSent				//  test if we are done transmitting before driving bus active
	goto	VPW_TX_end_of_data  //  go execute end of data routine if done
	clrf	TMR0L				//  reset the timer
	bsf		PORTB, VPW_Out		//  set the bus active for next phase
	goto	TX_active_phase		//  now lets continue with the active phase
 
passive_1:
	call	get_next_bit_v		//  setup the next bit to be transmitted
passive_1_ck:
	btfsc	PORTA, VPW_In		//  check if someone else may be driving the bus
	goto	lost_arbitration_v	//  exit routine if someone else is driving the bus
								//  leave bus passive for 128us for a passive 1 pulse
	movlw 	( 128000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set when 128us has been equaled or exceeded
	goto	passive_1_ck		//  loop until pulse is finished
	goto	end_passive			//  go get ready for next phase	
 
lost_arbitration_v:
	movf	BusTimeOut, f		// Check the bus access time out count to see
	btfss	STATUS, Z			// if its already zero	
	decf	BusTimeOut, f		// reduce bus time out on each lost arbitration cycle
	goto	lost_arbitration_v2	
 
TX_active_phase:
	btfsc	NxtBit				//  Check if next bit is a zero or a one
	goto	active_1			//  branch if its a one
active_0:
								//  drive bus active for 128us for an active 0 pulse
	movlw 	( 128000 /1600 )	// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set when 128us has been equaled or exceeded
	goto	active_0			//  loop until pulse is finished
 
end_active:
	clrf	TMR0L				//  reset the timer
	bcf		PORTB, VPW_Out		//  set the bus passive for next phase
	call	get_next_bit_v		//  setup the next bit to be transmitted
	btfsc	MsgSent				//  test if we are done transmitting yet
	goto	VPW_TX_end_of_data	//  go execute end of data routine if done
passive_delay_v:				//  allow the bus to settle, per Moto 33390 spec should be 17.7us max
								//  allow 21us settle time for bus to go passive
	movlw 	( 21000 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set if 21us has been equaled or exceeded
	goto	passive_delay_v		//  wait the 21 us settle time before testing for lost arbitration
	goto	TX_passive_phase	//  now lets continue with the passive phase
 
active_1:
								//  drive bus active for 64us for an active 1 pulse
	movlw 	( 64000 /1600 )		// (desired time in ns)/1.6us cylce time
	subwf	TMR0L, W			// subtract from the current timer value
	btfss	STATUS, C			//  "C" is set when 64us has been equaled or exceeded
	goto	active_1			//  loop until pulse is finished
	goto	end_active			//  go get ready for next phase	
 
get_next_bit_v:					//  get next bit to transmit
	bcf		STATUS, C			//  make sure 'C' bit is clear before rotating
	rrcf	MaskReg, F			//  Move Mask to next bit position
	btfsc	STATUS, C			//  if C is set, the entire byte has been sent
	goto	get_next_byte_v		//  setup the next byte
test_the_bit_v:					//  check if the masked bit is a one or a zero
	movf	MaskReg, W			//  fetch the Mask Register
	andwf	INDF0, W			//  and the mask with current byte being transmitted (indirect)
	bcf		NxtBit
	btfss	STATUS, Z			//  skip next instruction if bit was a zero
	bsf		NxtBit				//  NxtBit is now set up for the next bit to be sent
	return
 
get_next_byte_v:
	incf	FSR0L, F			//  increment the TX buffer pointer
	decf	BusByteCntr, F		//  sub one from the count of bytes to be sent
	btfsc	STATUS, Z			//  have we sent them all yet
	goto	all_sent_v			//  exit loop if we are done sending
	movlw	0x80
	movwf	MaskReg				//  put a one back into the MSB position
	goto	test_the_bit_v		//  setup NxtBit for the MSB bit of this new byte

TxBusBusyExit_v:
	bsf		NoBusActivity		// set the error flag bit in LinkControl
	goto	TxErrorExit_v			
 
all_sent_v:
	bsf		MsgSent				//  set the Message Sent Flag
								//  Support for in-frame response was removed per David
//	movlw	RxBuffer
//	movwf	FSR0L				//  setup RxBuffer pointer in case of in-frame response
//	movlw	0b00000001			//  prime first RxBuffer byte as a shift register
//	movwf	INDF0
	return

VPW_TX_end_of_data:
	bsf		EOD					//  set the end of data flag
	bsf		EOF					//  set end of frame flag
TxErrorExit_v:
//	movff	BSR_temp, BSR		// Restore setting of Bank Select Register
	#endasm

	return LinkControl;			// return status for the calling application
}	 
								
 
// ##############   End of vpw-xx.c    ########################


