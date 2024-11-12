#NOLIST
//  Revised July 29, 2003 for CAN bus and PIC18F248 processor
// 
// ******************  HARDWARE SETUP DEFINITIONS ********************
// 
//   This file: 	hardware.h 
// 	Is the file that defines the pin / function assignments
// 	and the internal peripheral hardware feature assignments
// 	for use in the Auterra OBDII Scan Tool Adapter.
// 
// 
//     I/O Port A Pinout:
// 	RA0	VBATT-A/D	A to D input to measure battery voltage (Multiply A/D Reading by 4.743)
// 	RA1	PWM_N_Out	PWM- output, goes high for Active Bus 	
// 	RA2	PWM_In+		This is the PWM input from the external LM393 comparator
// 	RA3	PWM_P_Out	PWM+ output, goes low for Active Bus	
// 	RA4	VPW_In		VPW mode input from the Moto MC33390 chip
// 	RA5	unused		Set as an output for potential debug usage
// 	RA6	XTAL_OSC2	Crystal Output Will use a 20Mhz crystal
// 	---	XTAL_OSC1	Crystal Input (not RA7 which the PIC18F248 does not have)
//
// SET_TRIS_A ( 0x55 ); 	// This goes into TRISA, a 1 sets a port pin for input
							// Ports are actually setup by "init_ports()" function

//     I/O Port B Pinout:
// 	RB0	VPW_4x		Output, The 4X loop control for the Moto MC33390 VPW I/F chip
// 	RB1	VPW_Out		Goes high for active bus
// 	RB2	CAN-TX		CAN bus interface output pin
// 	RB3	CAN-RX		CAN bus interface input pin
// 	RB4	unused		set as an output for debugging purposes
// 	RB5	ISO_K_RX	K line input, and in-crcuit programming Clock Input
// 	RB6	ISO_K_TX	K line output, goes high for active bus
// 	RB7	ISO_L_TX	L line output, goes high for active bus
//	
 
// SET_TRIS_B ( 0x28 );	  	// This goes into TRISB, a 1 sets a port pin for input
							// Ports are actually setup by "init_ports()" function

//     I/O Port C Pinout:
// 	RC0	unused
// 	RC1	unused
// 	RC2	unused
// 	RC3	unused
// 	RC4	unused		
// 	RC5	unused
// 	RC6	TX_OUT		Serial Port Output, to the Palm / PC serial interface
// 	RC7	RX_In		Serial Port Input, from the Palm / PC serial interface
//	
 
// SET_TRIS_C ( 0xBF );	  	// This goes into TRISC, a 1 sets a port pin for input
							// Ports are actually setup by "init_ports()" function

// 
//   Signal Name / Value Defines
 
// 	Port RA:
#bit	PWM_In = 0x0F80.2	// PWM bus input pin on PORTA
#define PWM_P_Out		3	// A zero sets the bus active
#define	PWM_N_Out		1	// A one sets the bus active (these two bits are used together)
#define	VPW_In			4	// VPW input pin

// 	Port RB:
#define	VPW_4x			0	// A one selects the VPW 4X speed mode (leave at Zero)
#define	VPW_Out			1	// A one sets bus active
//		CAN_TX			2	// CAN controller TX output
//		CAN_RX			3	// CAN controller RX input
//		TX_debug		4	// Debug mode RS232 output on PORTB
#define	ISO_K_TX		6	// A one sets bus active
#define	ISO_K_RX		5	// ISO K input pin
#define	ISO_L_TX		7	// A one sets bus active

//
//    CLOCK Speed / Delay:
#use delay (clock=20000000)

 
//    PIC18F248 TIMER USAGE:
// 	Timer 0 is used by the PWM, VPW & ISO Routines
//			In 8 bit mode, Timer0 is equivalent to Timer0 in the PIC16F628.
// 	Timer 1 is used by the main "C" code program
//			Timer1 is equivalent to Timer1 in the PIC16F628
// 	Timer 2 is used by the VPW Routine
//			Timer2 is equivalent to Timer2 in the PIC16F628
//	Timer 3 is new with the PIC18F248 and is not currently used
// 
//    EEPROM:
// 	May be used to store device serial numbers or other non-volatile variables.
// 
//    Hardware USART:
// 	Is under the control of the Main "C" code program.  The USART has its own
// 	baud rate generator timer, and does not require the use of Timers 0,1,2 or 3.
// 
//    RAM File Allocation:
// 	To maximize the speed of the PWM bus protocol interface, the bus
// 	interface subroutines will have first priority of RAM utilization in
// 	Bank 0.  This minimizes the number of delays that are required for
// 	switching Bank select bits to access variables and hardware registers.
//
//	The PIC18 part has several limprovements over the PIC16 in regards to static RAM
//	accessing.  For one, all the Special Function Registers now exist in bank-15, and
//	are not co-mingled with chunks of RAM.  Each RAM bank is 256 bytes in size.
// 
// 
// Processor Configuration Words, 6 words, or 12 bytes of configuration data.
// See page 265 of the PIC18FXX8 data sheet for full details.
// See 18F248.h header file for list of CCS compiler settable fuses.
// Location 300000 and up  (This is programed into the part above normal memory space)
//

//#fuses HS, PROTECT, NOOSCSEN, NOBROWNOUT, NOWDT, PUT, CPD, NOLVP, WRT, WRTD, WRTB, CPB, WRTC, EBTR, EBTRB
#fuses HS, PROTECT, NOOSCSEN, NOBROWNOUT, NOWDT, PUT, NOLVP, WRT, WRTB, CPB, WRTC, EBTR, EBTRB  // removed CPD and WRTD to read/write EEPROM

	// declares the processor configuration bits
// 
//
// *************** End of hardware.h  *************************
#LIST	
