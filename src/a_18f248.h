#ifndef _18F248_H
#define _18F248_H

#NOLIST

// This header file defines configurations, registers, and other useful bits of
// information for the PIC18F248 microcontroller.  These names are taken to match 
// the data sheets as closely as possible.  
//
//==========================================================================
//
//       Revision History
//
//==========================================================================
//Rev:   Date:        Details:                                         Who:
//0.90  15 May 2001   Preliminary Release                               dzb
//0.99  29 June2001   Rev 1                                             dzb
//1.00  29 Oct.2001   Corrections & Additions			       cjh
//1.10  25 Jun 2002   Added CFGS as EECON1 bit 6 name                   pas 
//1.20  29 Jul 2003   Converted to operate with the CCS compiler		Bernie Lafreniere
//1.30  09 Mar 2004   Modified to operate with CCS						David Lafreniere
//
//==========================================================================
//       18Fxxx Family        Defines
//==========================================================================

#define FSR0             0
#define FSR1             1
#define FSR2             2

#define FAST             1

#define W                0
#define A                0
#define ACCESS           0
#define BANKED		     1

//==========================================================================
//       16Cxxx/17Cxxx Substitutions
//==========================================================================

  #define clrw clrf WREG       // PIC16Cxxx code substitution (WREG is addressable)
  #define CLRW CLRF WREG       // PIC16Cxxx code substitution (WREG is addressable)
  #define negw negf WREG       // PIC16Cxxx code substitution (WREG is addressable)
  #define NEGW NEGF WREG       // PIC16Cxxx code substitution (WREG is addressable)

//==========================================================================
//
//       Register Definitions
//
//==========================================================================

//----- Register Files -----------------------------------------------------
#byte TOSU              = 0x0FFF
#byte TOSH              = 0x0FFE
#byte TOSL              = 0x0FFD
#byte STKPTR            = 0x0FFC
#byte PCLATU            = 0x0FFB
#byte PCLATH            = 0x0FFA
#byte PCL               = 0x0FF9
#byte TBLPTRU           = 0x0FF8
#byte TBLPTRH           = 0x0FF7
#byte TBLPTRL           = 0x0FF6
#byte TABLAT            = 0x0FF5
#byte PRODH             = 0x0FF4
#byte PRODL             = 0x0FF3

#byte INTCON            = 0x0FF2
#byte INTCON1           = 0x0FF2
#byte INTCON2           = 0x0FF1
#byte INTCON3           = 0x0FF0

#byte INDF0             = 0x0FEF
#byte POSTINC0          = 0x0FEE
#byte POSTDEC0          = 0x0FED
#byte PREINC0           = 0x0FEC
#byte PLUSW0            = 0x0FEB
#byte FSR0H             = 0x0FEA
#byte FSR0L             = 0x0FE9
#byte WREG              = 0x0FE8

#byte INDF1             = 0x0FE7
#byte POSTINC1          = 0x0FE6
#byte POSTDEC1          = 0x0FE5
#byte PREINC1           = 0x0FE4
#byte PLUSW1            = 0x0FE3
#byte FSR1H             = 0x0FE2
#byte FSR1L             = 0x0FE1
#byte BSR               = 0x0FE0

#byte INDF2             = 0x0FDF
#byte POSTINC2          = 0x0FDE
#byte POSTDEC2          = 0x0FDD
#byte PREINC2           = 0x0FDC
#byte PLUSW2            = 0x0FDB
#byte FSR2H             = 0x0FDA
#byte FSR2L             = 0x0FD9
#byte STATUS            = 0x0FD8

#byte TMR0H             = 0x0FD7
#byte TMR0L             = 0x0FD6
#byte T0CON             = 0x0FD5

//RESERVED_0FD4     0x0FD4

#byte OSCCON            = 0x0FD3
#byte LVDCON            = 0x0FD2
#byte WDTCON            = 0x0FD1
#byte RCON              = 0x0FD0

#byte TMR1H             = 0x0FCF
#byte TMR1L             = 0x0FCE
#byte T1CON             = 0x0FCD
#byte TMR2              = 0x0FCC
#byte PR2               = 0x0FCB
#byte T2CON             = 0x0FCA

#byte SSPBUF            = 0x0FC9
#byte SSPADD            = 0x0FC8
#byte SSPSTAT           = 0x0FC7
#byte SSPCON1           = 0x0FC6
#byte SSPCON2           = 0x0FC5

#byte ADRESH            = 0x0FC4
#byte ADRESL            = 0x0FC3
#byte ADCON0            = 0x0FC2
#byte ADCON1            = 0x0FC1
#byte ADCON2            = 0x0FC0

#byte CCPR1H            = 0x0FBF
#byte CCPR1L            = 0x0FBE
#byte CCP1CON           = 0x0FBD
#byte ECCPR1H           = 0x0FBC
#byte ECCPR1L           = 0x0FBB
#byte ECCP1CON          = 0x0FBA

#byte ECCP1DEL          = 0x0FB7
#byte ECCPAS            = 0x0FB6
#byte CVRCON            = 0x0FB5
#byte CMCON             = 0x0FB4

#byte TMR3H             = 0x0FB3
#byte TMR3L             = 0x0FB2
#byte T3CON             = 0x0FB1

#byte PSPCON            = 0x0FB0

#byte SPBRG             = 0x0FAF
#byte RCREG             = 0x0FAE
#byte TXREG             = 0x0FAD
#byte TXSTA             = 0x0FAC
#byte RCSTA             = 0x0FAB

#byte EEADR             = 0x0FA9
#byte EEDATA            = 0x0FA8
#byte EECON2            = 0x0FA7
#byte EECON1            = 0x0FA6
#byte IPR3              = 0x0FA5
#byte PIR3              = 0x0FA4
#byte PIE3              = 0x0FA3

#byte IPR2              = 0x0FA2
#byte PIR2              = 0x0FA1
#byte PIE2              = 0x0FA0
#byte IPR1              = 0x0F9F
#byte PIR1              = 0x0F9E
#byte PIE1              = 0x0F9D

#byte TRISC             = 0x0F94
#byte TRISB             = 0x0F93
#byte TRISA             = 0x0F92

#byte LATC              = 0x0F8B
#byte LATB              = 0x0F8A
#byte LATA              = 0x0F89

#byte PORTC             = 0x0F82
#byte PORTB             = 0x0F81
#byte PORTA             = 0x0F80

#byte TXERRCNT          = 0x0F76
#byte RXERRCNT          = 0x0F75
#byte COMSTAT           = 0x0F74
#byte CIOCON            = 0x0F73
#byte BRGCON3           = 0x0F72
#byte BRGCON2           = 0x0F71
#byte BRGCON1           = 0x0F70

#byte CANCON            = 0x0F6F
#byte CANSTAT           = 0x0F6E
#byte RXB0D7            = 0x0F6D
#byte RXB0D6            = 0x0F6C
#byte RXB0D5            = 0x0F6B
#byte RXB0D4            = 0x0F6A
#byte RXB0D3            = 0x0F69
#byte RXB0D2            = 0x0F68
#byte RXB0D1            = 0x0F67
#byte RXB0D0            = 0x0F66
#byte RXB0DLC           = 0x0F65
#byte RXB0EIDL          = 0x0F64
#byte RXB0EIDH          = 0x0F63
#byte RXB0SIDL          = 0x0F62
#byte RXB0SIDH          = 0x0F61
#byte RXB0CON           = 0x0F60

#byte CANSTATRO1        = 0x0F5E
#byte RXB1D7            = 0x0F5D
#byte RXB1D6            = 0x0F5C
#byte RXB1D5            = 0x0F5B
#byte RXB1D4            = 0x0F5A
#byte RXB1D3            = 0x0F59
#byte RXB1D2            = 0x0F58
#byte RXB1D1            = 0x0F57
#byte RXB1D0            = 0x0F56
#byte RXB1DLC           = 0x0F55
#byte RXB1EIDL          = 0x0F54
#byte RXB1EIDH          = 0x0F53
#byte RXB1SIDL          = 0x0F52
#byte RXB1SIDH          = 0x0F51
#byte RXB1CON           = 0x0F50

#byte CANSTATRO2        = 0x0F4E
#byte TXB0D7            = 0x0F4D
#byte TXB0D6            = 0x0F4C
#byte TXB0D5            = 0x0F4B
#byte TXB0D4            = 0x0F4A
#byte TXB0D3            = 0x0F49
#byte TXB0D2            = 0x0F48
#byte TXB0D1            = 0x0F47
#byte TXB0D0            = 0x0F46
#byte TXB0DLC           = 0x0F45
#byte TXB0EIDL          = 0x0F44
#byte TXB0EIDH          = 0x0F43
#byte TXB0SIDL          = 0x0F42
#byte TXB0SIDH          = 0x0F41
#byte TXB0CON           = 0x0F40

#byte CANSTATRO3        = 0x0F3E
#byte TXB1D7            = 0x0F3D
#byte TXB1D6            = 0x0F3C
#byte TXB1D5            = 0x0F3B
#byte TXB1D4            = 0x0F3A
#byte TXB1D3            = 0x0F39
#byte TXB1D2            = 0x0F38
#byte TXB1D1            = 0x0F37
#byte TXB1D0            = 0x0F36
#byte TXB1DLC           = 0x0F35
#byte TXB1EIDL          = 0x0F34
#byte TXB1EIDH          = 0x0F33
#byte TXB1SIDL          = 0x0F32
#byte TXB1SIDH          = 0x0F31
#byte TXB1CON           = 0x0F30

#byte CANSTATRO4        = 0x0F2E
#byte TXB2D7            = 0x0F2D
#byte TXB2D6            = 0x0F2C
#byte TXB2D5            = 0x0F2B
#byte TXB2D4            = 0x0F2A
#byte TXB2D3            = 0x0F29
#byte TXB2D2            = 0x0F28
#byte TXB2D1            = 0x0F27
#byte TXB2D0            = 0x0F26
#byte TXB2DLC           = 0x0F25
#byte TXB2EIDL          = 0x0F24
#byte TXB2EIDH          = 0x0F23
#byte TXB2SIDL          = 0x0F22
#byte TXB2SIDH          = 0x0F21
#byte TXB2CON           = 0x0F20

#byte RXM1EIDL          = 0x0F1F
#byte RXM1EIDH          = 0x0F1E
#byte RXM1SIDL          = 0x0F1D
#byte RXM1SIDH          = 0x0F1C

#byte RXM0EIDL          = 0x0F1B
#byte RXM0EIDH          = 0x0F1A
#byte RXM0SIDL          = 0x0F19
#byte RXM0SIDH          = 0x0F18

#byte RXF5EIDL          = 0x0F17
#byte RXF5EIDH          = 0x0F16
#byte RXF5SIDL          = 0x0F15
#byte RXF5SIDH          = 0x0F14

#byte RXF4EIDL          = 0x0F13
#byte RXF4EIDH          = 0x0F12
#byte RXF4SIDL          = 0x0F11
#byte RXF4SIDH          = 0x0F10

#byte RXF3EIDL          = 0x0F0F
#byte RXF3EIDH          = 0x0F0E
#byte RXF3SIDL          = 0x0F0D
#byte RXF3SIDH          = 0x0F0C

#byte RXF2EIDL          = 0x0F0B
#byte RXF2EIDH          = 0x0F0A
#byte RXF2SIDL          = 0x0F09
#byte RXF2SIDH          = 0x0F08

#byte RXF1EIDL          = 0x0F07
#byte RXF1EIDH          = 0x0F06
#byte RXF1SIDL          = 0x0F05
#byte RXF1SIDH          = 0x0F04

#byte RXF0EIDL          = 0x0F03
#byte RXF0EIDH          = 0x0F02
#byte RXF0SIDL          = 0x0F01
#byte RXF0SIDH          = 0x0F00

//----- STKPTR Bits --------------------------------------------------------
#define STKFUL            0x0007
#define STKUNF            0x0006
#define SP4	         	0x0004
#define SP3	          	0x0003
#define SP2	          	0x0002
#define SP1	         	 0x0001
#define SP0	         	 0x0000

//----- INTCON Bits --------------------------------------------------------
#define GIE               0x0007
#define GIEH              0x0007
#define PEIE              0x0006
#define GIEL              0x0006
#define TMR0IE            0x0005
#define T0IE              0x0005      // For backward compatibility
#define INT0IE            0x0004
#define INT0E             0x0004      // For backward compatibility
#define RBIE              0x0003
#define TMR0IF            0x0002
#define T0IF              0x0002      // For backward compatibility
#define INT0IF            0x0001
#define INT0F             0x0001      // For backward compatibility
#define RBIF              0x0000

//----- INTCON2 Bits --------------------------------------------------------
#define NOT_RBPU          0x0007
#define RBPU              0x0007
#define INTEDG0           0x0006
#define INTEDG1           0x0005
#define TMR0IP            0x0002
#define T0IP              0x0002      // For compatibility with T0IE and T0IF
#define RBIP              0x0000

//----- INTCON3 Bits --------------------------------------------------------
#define INT2IP		  0x0007
#define INT2P		  0x0007
#define INT1IP            0x0006
#define INT1P             0x0006
#define INT2IE		  0x0004
#define INT2E		  0x0004
#define INT1IE            0x0003
#define INT1E             0x0003
#define INT2IF		  0x0001
#define INT2F		  0x0001
#define INT1IF            0x0000
#define INT1F             0x0000

//----- STATUS Bits --------------------------------------------------------
#define N                 0x0004
#define OV                0x0003
#define Z                 0x0002
#define DC                0x0001
#define C                 0x0000

//----- T0CON Bits ---------------------------------------------------------
#define TMR0ON            0x0007
#define T08BIT            0x0006
#define T0CS              0x0005
#define T0SE              0x0004
#define PSA               0x0003
#define T0PS2             0x0002
#define T0PS1             0x0001
#define T0PS0             0x0000

//----- OSCCON Bits ---------------------------------------------------------
#define SCS               0x0000

//----- LVDCON Bits ---------------------------------------------------------
#define IVRST             0x0005
#define LVDEN             0x0004
#define LVDL3             0x0003
#define LVDL2             0x0002
#define LVDL1             0x0001
#define LVDL0             0x0000

//----- WDTCON Bits ---------------------------------------------------------
#define SWDTE             0x0000
#define SWDTEN            0x0000

//----- RCON Bits -----------------------------------------------------------
#define IPEN              0x0007
#define NOT_RI            0x0004
#define RI                0x0004
#define NOT_TO            0x0003
#define TO                0x0003
#define NOT_PD            0x0002
#define PD                0x0002
#define NOT_POR           0x0001
#define POR               0x0001
#define NOT_BOR           0x0000
#define BOR               0x0000

//----- T1CON Bits ---------------------------------------------------------
#define RD16              0x0007
#define T1CKPS1           0x0005
#define T1CKPS0           0x0004
#define T1OSCEN           0x0003
#define NOT_T1SYNC        0x0002
#define T1SYNC            0x0002
#define T1INSYNC          0x0002    // Backward compatibility only
#define TMR1CS            0x0001
#define TMR1ON            0x0000

//----- T2CON Bits ---------------------------------------------------------
#define TOUTPS3           0x0006
#define TOUTPS2           0x0005
#define TOUTPS1           0x0004
#define TOUTPS0           0x0003
#define TMR2ON            0x0002
#define T2CKPS1           0x0001
#define T2CKPS0           0x0000

//----- SSPSTAT Bits -------------------------------------------------------
#define SMP               0x0007
#define CKE               0x0006
#define D                 0x0005
#define I2C_DAT           0x0005
#define NOT_A             0x0005
#define NOT_ADDRESS       0x0005
#define D_A               0x0005
#define DATA_ADDRESS      0x0005
#define P                 0x0004
#define I2C_STOP          0x0004
#define S                 0x0003
#define I2C_START         0x0003
#define R                 0x0002
#define I2C_READ          0x0002
#define NOT_W             0x0002
#define NOT_WRITE         0x0002
#define R_W               0x0002
#define READ_WRITE        0x0002
#define UA                0x0001
#define BF                0x0000

//----- SSPCON1 Bits --------------------------------------------------------
#define WCOL              0x0007
#define SSPOV             0x0006
#define SSPEN             0x0005
#define CKP               0x0004
#define SSPM3             0x0003
#define SSPM2             0x0002
#define SSPM1             0x0001
#define SSPM0             0x0000

//----- SSPCON2 Bits --------------------------------------------------------
#define GCEN              0x0007
#define ACKSTAT           0x0006
#define ACKDT             0x0005
#define ACKEN             0x0004
#define RCEN              0x0003
#define PEN               0x0002
#define RSEN              0x0001
#define SEN               0x0000

//----- ADCON0 Bits --------------------------------------------------------
#define ADCS1             0x0007
#define ADCS0             0x0006
#define CHS2              0x0005
#define CHS1              0x0004
#define CHS0              0x0003
#define GO                0x0002
#define NOT_DONE          0x0002
#define DONE              0x0002
#define GO_DONE           0x0002
#define ADON              0x0000

//----- ADCON1 Bits --------------------------------------------------------
#define ADFM              0x0007
#define ADCS2             0x0006
#define PCFG3             0x0003
#define PCFG2             0x0002
#define PCFG1             0x0001
#define PCFG0             0x0000

//----- CCP1CON Bits -------------------------------------------------------
#define DC1B1             0x0005
#define CCP1X             0x0005      // For backward compatibility
#define DC1B0             0x0004
#define CCP1Y             0x0004      // For backward compatibility
#define CCP1M3            0x0003
#define CCP1M2            0x0002
#define CCP1M1            0x0001
#define CCP1M0            0x0000

//----- ECCP1CON Bits ------------------------------------------------------
#define EPWM1M1           0x0007
#define EPWM1M0           0x0006
#define EDC2B1            0x0005
#define EDC2B0            0x0004
#define ECCP1M3           0x0003
#define ECCP1M2           0x0002
#define ECCP1M1           0x0001
#define ECCP1M0           0x0000

//----- ECCPAS Bits --------------------------------------------------------
#define ECCPASE           0x0007
#define ECCPAS2           0x0006
#define ECCPAS1           0x0005
#define ECCPAS0           0x0004
#define PSSAC1            0x0003
#define PSSAC0            0x0002
#define PSSBD1            0x0001
#define PSSBD0            0x0000

//----- CVRCON Bits --------------------------------------------------------
#define CVREN             0x0007
#define CVROE             0x0006      // For backward compatibility
#define CVRR              0x0005
#define CVRSS             0x0004      // For backward compatibility
#define CVR3              0x0003
#define CVR2              0x0002
#define CVR1              0x0001
#define CVR0              0x0000

//----- CMCON Bits ---------------------------------------------------------
#define C2OUT             0x0007
#define C1OUT             0x0006      // For backward compatibility
#define C2INV             0x0005
#define C1INV             0x0004      // For backward compatibility
#define CIS               0x0003
#define CM2               0x0002
#define CM1               0x0001
#define CM0               0x0000

//----- T3CON Bits ---------------------------------------------------------
//#define RD16              0x0007 (duplicate definition with T1CON
#define T3ECCP1           0x0006
#define T3CKPS1           0x0005
#define T3CKPS0           0x0004
#define T3CCP1            0x0003
#define NOT_T3SYNC        0x0002
#define T3SYNC            0x0002
#define T3INSYNC          0x0002    // Backward compatibility only
#define TMR3CS            0x0001
#define TMR3ON            0x0000

//----- PSPCON Bits --------------------------------------------------------
#define IBF               0x0007
#define OBF               0x0006
#define IBOV              0x0005
#define PSPMODE           0x0004

//----- TXSTA Bits ---------------------------------------------------------
#define CSRC              0x0007
#define TX9               0x0006
#define NOT_TX8           0x0006    // Backward compatibility only
#define TX8_9             0x0006    // Backward compatibility only
#define TXEN              0x0005
#define SYNC              0x0004
#define BRGH              0x0002
#define TRMT              0x0001
#define TX9D              0x0000
#define TXD8              0x0000    // Backward compatibility only

//----- RCSTA Bits ---------------------------------------------------------
#define SPEN              0x0007
#define RX9               0x0006
#define RC9               0x0006    // Backward compatibility only
#define NOT_RC8           0x0006    // Backward compatibility only
#define RC8_9             0x0006    // Backward compatibility only
#define SREN              0x0005
#define CREN              0x0004
#define ADDEN             0x0003
#define FERR              0x0002
#define OERR              0x0001
#define RX9D              0x0000
#define RCD8              0x0000    // Backward compatibility only

//----- EECON1 Bits --------------------------------------------------------
#define EEPGD             0x0007
#define EEFS              0x0006    // Backward compatability only
#define CFGS              0x0006
#define FREE              0x0004
#define WRERR             0x0003
#define WREN              0x0002
#define WR                0x0001
#define RD                0x0000

//----- IPR3 Bits ----------------------------------------------------------
#define IRXIP             0x0007
#define WAKIP             0x0006
#define ERRIP             0x0005
#define TXB2IP            0x0004
#define TXB1IP            0x0003
#define TXB0IP            0x0002
#define RXB1IP            0x0001
#define RXB0IP            0x0000

//----- PIR3 Bits ----------------------------------------------------------
#define IRXIF             0x0007
#define WAKIF             0x0006
#define ERRIF             0x0005
#define TXB2IF            0x0004
#define TXB1IF            0x0003
#define TXB0IF            0x0002
#define RXB1IF            0x0001
#define RXB0IF            0x0000

//----- PIE3 Bits ----------------------------------------------------------
#define IRXIE             0x0007
#define WAKIE             0x0006
#define ERRIE             0x0005
#define TXB2IE            0x0004
#define TXB1IE            0x0003
#define TXB0IE            0x0002
#define RXB1IE            0x0001
#define RXB0IE            0x0000

//----- IPR2 Bits ----------------------------------------------------------
#define CMIP              0x0006
#define EEIP              0x0004
#define BCLIP             0x0003
#define LVDIP             0x0002
#define TMR3IP            0x0001
#define ECCP1IP           0x0000

//----- PIR2 Bits ----------------------------------------------------------
#define CMIF              0x0006
#define EEIF              0x0004
#define BCLIF             0x0003
#define LVDIF             0x0002
#define TMR3IF            0x0001
#define ECCP1IF           0x0000

//----- PIE2 Bits ----------------------------------------------------------
#define CMIE              0x0006
#define EEIE              0x0004
#define BCLIE             0x0003
#define LVDIE             0x0002
#define TMR3IE            0x0001
#define ECCP1IE           0x0000

//----- IPR1 Bits ----------------------------------------------------------
#define PSPIP             0x0007
#define ADIP              0x0006
#define RCIP              0x0005
#define TXIP              0x0004
#define SSPIP             0x0003
#define CCP1IP            0x0002
#define TMR2IP            0x0001
#define TMR1IP            0x0000

//----- PIR1 Bits ----------------------------------------------------------
#define PSPIF             0x0007
#define ADIF              0x0006
#define RCIF              0x0005
#define TXIF              0x0004
#define SSPIF             0x0003
#define CCP1IF            0x0002
#define TMR2IF            0x0001
#define TMR1IF            0x0000

//----- PIE1 Bits ----------------------------------------------------------
#define PSPIE             0x0007
#define ADIE              0x0006
#define RCIE              0x0005
#define TXIE              0x0004
#define SSPIE             0x0003
#define CCP1IE            0x0002
#define TMR2IE            0x0001
#define TMR1IE            0x0000

//----- COMSTAT Bits ---------------------------------------------------------
#define RX1OVFL           0x0007
#define RXB0OVFL          0x0007
#define RX2OVFL           0x0006
#define RXB1OVFL          0x0006
#define TXBO              0x0005
#define TXBP              0x0004
#define RXBP              0x0003
#define TXWARN            0x0002
#define RXWARN            0x0001
#define EWARN             0x0000

//----- CIOCON Bits -----------------------------------------------------------
#define ENDRHI            0x0005
#define CANCAP            0x0004

//----- BRGCON3 Bits ----------------------------------------------------------
#define WAKFIL            0x0006
#define SEG2PH2           0x0002
#define SEG2PH1           0x0001
#define SEG2PH0           0x0000

//----- BRGCON2 Bits -----------------------------------------------------------
#define SEG2PHTS          0x0007
#define SAM               0x0006
#define SEG1PH2           0x0005
#define SEG1PH1           0x0004
#define SEG1PH0           0x0003
#define PRSEG2            0x0002
#define PRSEG1            0x0001
#define PRSEG0            0x0000

//----- BRGCON1 Bits ------------------------------------------------------------
#define SJW1              0x0007
#define SJW0              0x0006
#define BRP5              0x0005
#define BRP4              0x0004
#define BRP3              0x0003
#define BRP2              0x0002
#define BRP1              0x0001
#define BRP0              0x0000

//----- CANCON Bits --------------------------------------------------------
#define REQOP2            0x0007
#define REQOP1            0x0006
#define REQOP0            0x0005
#define ABAT              0x0004
#define WIN2              0x0003
#define WIN1              0x0002
#define WIN0              0x0001

//----- CANSTAT Bits -------------------------------------------------------
#define OPMODE2           0x0007
#define OPMODE1           0x0006
#define OPMODE0           0x0005
#define ICODE2            0x0003
#define ICODE1            0x0002
#define ICODE0            0x0001

//----- RXBnCON Bits -------------------------------------------------------
#define RXFUL             0x0007
#define RXM1              0x0006
#define RXM0              0x0005
#define RXRTRRO           0x0003
#define RXB0DBEN          0x0002
#define FILHIT2           0x0002
#define JTOFF             0x0001
#define FILHIT1           0x0001
#define FILHIT0           0x0000

//----- TXBnCON Bits -------------------------------------------------------
#define TXABT             0x0006
#define TXLARB            0x0005
#define TXERR             0x0004
#define TXREQ             0x0003
#define TXPRI1            0x0001
#define TXPRI0            0x0000

//----- TXERRCNT Bits ----------------------------------------------------------
#define TEC7              0x0007
#define TEC6              0x0006
#define TEC5              0x0005
#define TEC4              0x0004
#define TEC3              0x0003
#define TEC2              0x0002
#define TEC1              0x0001
#define TEC0              0x0000

//----- RXERRCNT Bits ----------------------------------------------------------
#define REC7              0x0007
#define REC6              0x0006
#define REC5              0x0005
#define REC4              0x0004
#define REC3              0x0003
#define REC2              0x0002
#define REC1              0x0001
#define REC0              0x0000

//----- RXBnDLC and TXBnDLC Bits -----------------------------------------------
#define RXRTR             0x0006
#define TXRTR             0x0006
#define RESB1             0x0005
#define RESB0             0x0004
#define DLC3              0x0003
#define DLC2              0x0002
#define DLC1              0x0001
#define DLC0              0x0000

//----- RXBnEIDL, RXFnEIDL, RXMnEIDL, and TXBnEIDL Bits -------------------------
#define EID7              0x0007
#define EID6              0x0006
#define EID5              0x0005
#define EID4              0x0004
#define EID3              0x0003
#define EID2              0x0002
#define EID1              0x0001
#define EID0              0x0000

//----- RXBnEIDH, RXFnEIDH, RXMnEIDH, and TXBnEIDH Bits -------------------------
#define EID15             0x0007
#define EID14             0x0006
#define EID13             0x0005
#define EID12             0x0004
#define EID11             0x0003
#define EID10             0x0002
#define EID9              0x0001
#define EID8              0x0000

//----- RXBnSIDL, RXFnSIDL, RXMnSIDL, and TXBnSIDL Bits ----------------------
#define SID2              0x0007
#define SID1              0x0006
#define SID0              0x0005
#define SRR               0x0004
#define EXID              0x0003
#define EXIDE             0x0003
#define EXIDEN            0x0003
#define EID17             0x0001
#define EID16             0x0000

//----- RXBnSIDH, RXFnSIDH, RXMnSIDH, and TXBnSIDH Bits ----------------------
#define SID10             0x0007
#define SID9              0x0006
#define SID8              0x0005
#define SID7              0x0004
#define SID6              0x0003
#define SID5              0x0002
#define SID4              0x0001
#define SID3              0x0001

//----- RXB0D7 Bits ----------------------------------------------------------
#define RB0D77            0x0007
#define RB0D76            0x0006
#define RB0D75            0x0005
#define RB0D74            0x0004
#define RB0D73            0x0003
#define RB0D72            0x0002
#define RB0D71            0x0001
#define RB0D70            0x0000

//----- RXB0D6 Bits ----------------------------------------------------------
#define RB0D67            0x0007
#define RB0D66            0x0006
#define RB0D65            0x0005
#define RB0D64            0x0004
#define RB0D63            0x0003
#define RB0D62            0x0002
#define RB0D61            0x0001
#define RB0D60            0x0000

//----- RXB0D5 Bits ----------------------------------------------------------
#define RB0D57            0x0007
#define RB0D56            0x0006
#define RB0D55            0x0005
#define RB0D54            0x0004
#define RB0D53            0x0003
#define RB0D52            0x0002
#define RB0D51            0x0001
#define RB0D50            0x0000

//----- RXB0D4 Bits ----------------------------------------------------------
#define RB0D47            0x0007
#define RB0D46            0x0006
#define RB0D45            0x0005
#define RB0D44            0x0004
#define RB0D43            0x0003
#define RB0D42            0x0002
#define RB0D41            0x0001
#define RB0D40            0x0000

//----- RXB0D3 Bits ----------------------------------------------------------
#define RB0D37            0x0007
#define RB0D36            0x0006
#define RB0D35            0x0005
#define RB0D34            0x0004
#define RB0D33            0x0003
#define RB0D32            0x0002
#define RB0D31            0x0001
#define RB0D30            0x0000

//----- RXB0D2 Bits ----------------------------------------------------------
#define RB0D27            0x0007
#define RB0D26            0x0006
#define RB0D25            0x0005
#define RB0D24            0x0004
#define RB0D23            0x0003
#define RB0D22            0x0002
#define RB0D21            0x0001
#define RB0D20            0x0000

//----- RXB0D1 Bits ----------------------------------------------------------
#define RB0D17            0x0007
#define RB0D16            0x0006
#define RB0D15            0x0005
#define RB0D14            0x0004
#define RB0D13            0x0003
#define RB0D12            0x0002
#define RB0D11            0x0001
#define RB0D10            0x0000

//----- RXB0D0 Bits ----------------------------------------------------------
#define RB0D07            0x0007
#define RB0D06            0x0006
#define RB0D05            0x0005
#define RB0D04            0x0004
#define RB0D03            0x0003
#define RB0D02            0x0002
#define RB0D01            0x0001
#define RB0D00            0x0000

//----- RXB1D7 Bits ----------------------------------------------------------
#define RXB1D77            0x0007
#define RXB1D76            0x0006
#define RXB1D75            0x0005
#define RXB1D74            0x0004
#define RXB1D73            0x0003
#define RXB1D72            0x0002
#define RXB1D71            0x0001
#define RXB1D70            0x0000

//----- RXB1D6 Bits ----------------------------------------------------------
#define RXB1D67            0x0007
#define RXB1D66            0x0006
#define RXB1D65            0x0005
#define RXB1D64            0x0004
#define RXB1D63            0x0003
#define RXB1D62            0x0002
#define RXB1D61            0x0001
#define RXB1D60            0x0000

//----- RXB1D5 Bits ----------------------------------------------------------
#define RXB1D57            0x0007
#define RXB1D56            0x0006
#define RXB1D55            0x0005
#define RXB1D54            0x0004
#define RXB1D53            0x0003
#define RXB1D52            0x0002
#define RXB1D51            0x0001
#define RXB1D50            0x0000


//----- RXB1D4 Bits ----------------------------------------------------------
#define RXB1D47            0x0007
#define RXB1D46            0x0006
#define RXB1D45            0x0005
#define RXB1D44            0x0004
#define RXB1D43            0x0003
#define RXB1D42            0x0002
#define RXB1D41            0x0001
#define RXB1D40            0x0000


//----- RXB1D3 Bits ----------------------------------------------------------
#define RXB1D37            0x0007
#define RXB1D36            0x0006
#define RXB1D35            0x0005
#define RXB1D34            0x0004
#define RXB1D33            0x0003
#define RXB1D32            0x0002
#define RXB1D31            0x0001
#define RXB1D30            0x0000


//----- RXB1D2 Bits ----------------------------------------------------------
#define RXB1D27            0x0007
#define RXB1D26            0x0006
#define RXB1D25            0x0005
#define RXB1D24            0x0004
#define RXB1D23            0x0003
#define RXB1D22            0x0002
#define RXB1D21            0x0001
#define RXB1D20            0x0000


//----- RXB1D1 Bits ----------------------------------------------------------
#define RXB1D17            0x0007
#define RXB1D16            0x0006
#define RXB1D15            0x0005
#define RXB1D14            0x0004
#define RXB1D13            0x0003
#define RXB1D12            0x0002
#define RXB1D11            0x0001
#define RXB1D10            0x0000

//----- RXB1D0 Bits ----------------------------------------------------------
#define RXB1D07            0x0007
#define RXB1D06            0x0006
#define RXB1D05            0x0005
#define RXB1D04            0x0004
#define RXB1D03            0x0003
#define RXB1D02            0x0002
#define RXB1D01            0x0001
#define RXB1D00            0x0000


//----- TXB2D7 Bits ----------------------------------------------------------
#define TXB2D77            0x0007
#define TXB2D76            0x0006
#define TXB2D75            0x0005
#define TXB2D74            0x0004
#define TXB2D73            0x0003
#define TXB2D72            0x0002
#define TXB2D71            0x0001
#define TXB2D70            0x0000

//----- TXB2D6 Bits ----------------------------------------------------------
#define TXB2D67            0x0007
#define TXB2D66            0x0006
#define TXB2D65            0x0005
#define TXB2D64            0x0004
#define TXB2D63            0x0003
#define TXB2D62            0x0002
#define TXB2D61            0x0001
#define TXB2D60            0x0000

//----- TXB2D5 Bits ----------------------------------------------------------
#define TXB2D57            0x0007
#define TXB2D56            0x0006
#define TXB2D55            0x0005
#define TXB2D54            0x0004
#define TXB2D53            0x0003
#define TXB2D52            0x0002
#define TXB2D51            0x0001
#define TXB2D50            0x0000

//----- TXB2D4 Bits ----------------------------------------------------------
#define TXB2D47            0x0007
#define TXB2D46            0x0006
#define TXB2D45            0x0005
#define TXB2D44            0x0004
#define TXB2D43            0x0003
#define TXB2D42            0x0002
#define TXB2D41            0x0001
#define TXB2D40            0x0000

//----- TXB2D3 Bits ----------------------------------------------------------
#define TXB2D37            0x0007
#define TXB2D36            0x0006
#define TXB2D35            0x0005
#define TXB2D34            0x0004
#define TXB2D33            0x0003
#define TXB2D32            0x0002
#define TXB2D31            0x0001
#define TXB2D30            0x0000

//----- TXB2D2 Bits ----------------------------------------------------------
#define TXB2D27            0x0007
#define TXB2D26            0x0006
#define TXB2D25            0x0005
#define TXB2D24            0x0004
#define TXB2D23            0x0003
#define TXB2D22            0x0002
#define TXB2D21            0x0001
#define TXB2D20            0x0000

//----- TXB2D1 Bits ----------------------------------------------------------
#define TXB2D17            0x0007
#define TXB2D16            0x0006
#define TXB2D15            0x0005
#define TXB2D14            0x0004
#define TXB2D13            0x0003
#define TXB2D12            0x0002
#define TXB2D11            0x0001
#define TXB2D10            0x0000

//----- TXB2D0 Bits ----------------------------------------------------------
#define TXB2D07            0x0007
#define TXB2D06            0x0006
#define TXB2D05            0x0005
#define TXB2D04            0x0004
#define TXB2D03            0x0003
#define TXB2D02            0x0002
#define TXB2D01            0x0001
#define TXB2D00            0x0000


//----- TXB1D7 Bits ----------------------------------------------------------
#define TXB1D77            0x0007
#define TXB1D76            0x0006
#define TXB1D75            0x0005
#define TXB1D74            0x0004
#define TXB1D73            0x0003
#define TXB1D72            0x0002
#define TXB1D71            0x0001
#define TXB1D70            0x0000

//----- TXB1D6 Bits ----------------------------------------------------------
#define TXB1D67            0x0007
#define TXB1D66            0x0006
#define TXB1D65            0x0005
#define TXB1D64            0x0004
#define TXB1D63            0x0003
#define TXB1D62            0x0002
#define TXB1D61            0x0001
#define TXB1D60            0x0000

//----- TXB1D5 Bits ----------------------------------------------------------
#define TXB1D57            0x0007
#define TXB1D56            0x0006
#define TXB1D55            0x0005
#define TXB1D54            0x0004
#define TXB1D53            0x0003
#define TXB1D52            0x0002
#define TXB1D51            0x0001
#define TXB1D50            0x0000

//----- TXB1D4 Bits ----------------------------------------------------------
#define TXB1D47            0x0007
#define TXB1D46            0x0006
#define TXB1D45            0x0005
#define TXB1D44            0x0004
#define TXB1D43            0x0003
#define TXB1D42            0x0002
#define TXB1D41            0x0001
#define TXB1D40            0x0000

//----- TXB1D3 Bits ----------------------------------------------------------
#define TXB1D37            0x0007
#define TXB1D36            0x0006
#define TXB1D35            0x0005
#define TXB1D34            0x0004
#define TXB1D33            0x0003
#define TXB1D32            0x0002
#define TXB1D31            0x0001
#define TXB1D30            0x0000

//----- TXB1D2 Bits ----------------------------------------------------------
#define TXB1D27            0x0007
#define TXB1D26            0x0006
#define TXB1D25            0x0005
#define TXB1D24            0x0004
#define TBB1D23            0x0003
#define TXB1D22            0x0002
#define TXB1D21            0x0001
#define TXB1D20            0x0000

//----- TXB1D1 Bits ----------------------------------------------------------
#define TXB1D17            0x0007
#define TXB1D16            0x0006
#define TXB1D15            0x0005
#define TXB1D14            0x0004
#define TXB1D13            0x0003
#define TXB1D12            0x0002
#define TXB1D11            0x0001
#define TXB1D10            0x0000

//----- TXB1D0 Bits ----------------------------------------------------------
#define TXB1D07            0x0007
#define TXB1D06            0x0006
#define TXB1D05            0x0005
#define TXB1D04            0x0004
#define TXB1D03            0x0003
#define TXB1D02            0x0002
#define TXB1D01            0x0001
#define TXB1D00            0x0000


//----- TXB0D7 Bits ----------------------------------------------------------
#define TXB0D77            0x0007
#define TXB0D76            0x0006
#define TXB0D75            0x0005
#define TXB0D74            0x0004
#define TXB0D73            0x0003
#define TXB0D72            0x0002
#define TXB0D71            0x0001
#define TXB0D70            0x0000

//----- TXB0D6 Bits ----------------------------------------------------------
#define TXB0D67            0x0007
#define TXB0D66            0x0006
#define TXB0D65            0x0005
#define TXB0D64            0x0004
#define TXB0D63            0x0003
#define TXB0D62            0x0002
#define TXB0D61            0x0001
#define TXB0D60            0x0000

//----- TXB0D5 Bits ----------------------------------------------------------
#define TXB0D57            0x0007
#define TXB0D56            0x0006
#define TXB0D55            0x0005
#define TXB0D54            0x0004
#define TXB0D53            0x0003
#define TXB0D52            0x0002
#define TXB0D51            0x0001
#define TXB0D50            0x0000

//----- TXB0D4 Bits ----------------------------------------------------------
#define TXB0D47            0x0007
#define TXB0D46            0x0006
#define TXB0D45            0x0005
#define TXB0D44            0x0004
#define TXB0D43            0x0003
#define TXB0D42            0x0002
#define TXB0D41            0x0001
#define TXB0D40            0x0000

//----- TXB0D3 Bits ----------------------------------------------------------
#define TXB0D37            0x0007
#define TXB0D36            0x0006
#define TXB0D35            0x0005
#define TXB0D34            0x0004
#define TXB0D33            0x0003
#define TXB0D32            0x0002
#define TXB0D31            0x0001
#define TXB0D30            0x0000

//----- TXB0D2 Bits ----------------------------------------------------------
#define TXB0D27            0x0007
#define TXB0D26            0x0006
#define TXB0D25            0x0005
#define TXB0D24            0x0004
#define TXB0D23            0x0003
#define TXB0D22            0x0002
#define TXB0D21            0x0001
#define TXB0D20            0x0000

//----- TXB0D1 Bits ----------------------------------------------------------
#define TXB0D17            0x0007
#define TXB0D16            0x0006
#define TXB0D15            0x0005
#define TXB0D14            0x0004
#define TXB0D13            0x0003
#define TXB0D12            0x0002
#define TXB0D11            0x0001
#define TXB0D10            0x0000

//----- TXB0D0 Bits ----------------------------------------------------------
#define TXB0D07            0x0007
#define TXB0D06            0x0006
#define TXB0D05            0x0005
#define TXB0D04            0x0004
#define TXB0D03            0x0003
#define TXB0D02            0x0002
#define TXB0D01            0x0001
#define TXB0D00            0x0000


//==========================================================================
//
//       I/O Pin Name Definitions
//
//==========================================================================

//----- PORTA ------------------------------------------------------------------
#define RA0               0
#define AN0               0
#define RA1               1
#define AN1               1
#define RA2               2
#define AN2               2
#define VREFM             2
#define RA3               3
#define AN3               3
#define VREFP             3
#define RA4               4
#define T0CKI             4
#define RA5               5
#define AN4               5
#define SS                5
#define NOT_SS            5
#define LVDIN             5
#define RA6               6
#define OSC2              6
#define CLKO              6

//----- PORTB ------------------------------------------------------------------
#define RB0               0
#define INT0              0
#define RB1               1
#define INT1              1
#define RB2               2
#define CANTX             2
#define RB3               3
#define CANRX             3
#define RB4               4
#define RB5               5
#define PGM               5
#define RB6               6
#define PGC               6
#define RB7               7
#define PGD               7

//----- PORTC ------------------------------------------------------------------
#define RC0               0
#define T1OSO             0
#define T1CKI             0
#define RC1               1
#define T1OSI             1
#define RC2               2
#define CCP1              2
#define RC3               3
#define SCK               3
#define SCL               3
#define RC4               4
#define SDI               4
#define SDA               4
#define RC5               5
#define SDO               5
#define RC6               6
#define TX                6
#define CK                6
#define RC7               7
#define RX                7
//****DT                7      //*** Not Available due to conflict with
                                 //***    Define Table (DT) directive

//==========================================================================
//
//       RAM Definition
//
//==========================================================================

//	__MAXRAM 0xFFF
//	__BADRAM 0x300-0xEFF
//	__BADRAM 0xFD4,0xFC0,0xFB9,0xFB8,0xFAA,0xF97-0xF9C
//	__BADRAM 0xF8E-0xF91,0xF85-0xF88, 0xF79-0xF7F,0xF77
//	__BADRAM 0xF5F,0xF4F,0xF3F,0xF2F
//
//==========================================================================
//
//       Configuration Bits
//
//     Data Sheet    Include File                  Address
//     CONFIG1H    = Configuration Byte 1H         300001h
//     CONFIG2L    = Configuration Byte 2L         300002h
//     CONFIG2H    = Configuration Byte 2H         300003h
//     CONFIG4L    = Configuration Byte 4L         300006h
//
//==========================================================================

//Configuration Byte 1H Options
#define _OSCS_ON_1H         0xDF	// Oscillator Switch enable
#define _OSCS_OFF_1H        0xFF

#define _LP_OSC_1H          0xF8	// Oscillator type
#define _XT_OSC_1H          0xF9
#define _HS_OSC_1H          0xFA
#define _RC_OSC_1H          0xFB
#define _EC_OSC_1H          0xFC	// External Clock w/OSC2 output divide by 4
#define _ECIO_OSC_1H        0xFD	// w/OSC2 as an IO pin (RA6)
#define _HSPLL_OSC_1H       0xFE	// HS PLL
#define _RCIO_OSC_1H        0xFF	// RC w/OSC2 as an IO pin (RA6)

//Configuration Byte 2L Options
#define _BOR_ON_2L          0xFF	// Brown-Out Reset enable
#define _BOR_OFF_2L         0xFD
#define _PWRT_OFF_2L        0xFF	// Power-Up Timer enable
#define _PWRT_ON_2L         0xFE
#define _BORV_25_2L         0xFF	// BOR Voltage - 2.5v
#define _BORV_27_2L         0xFB	//               2.7v
#define _BORV_42_2L         0xF7 	//               4.2v
#define _BORV_45_2L         0xF3	//               4.5v

//Configuration Byte 2H Options
#define _WDT_ON_2H          0xFF	// Watch Dog Timer enable
#define _WDT_OFF_2H         0xFE
#define _WDTPS_128_2H       0xFF	// Watch Dog Timer PostScaler count
#define _WDTPS_64_2H        0xFD
#define _WDTPS_32_2H        0xFB
#define _WDTPS_16_2H        0xF9
#define _WDTPS_8_2H         0xF7
#define _WDTPS_4_2H         0xF5
#define _WDTPS_2_2H         0xF3
#define _WDTPS_1_2H         0xF1

//Configuration Byte 4L Options
#define _DEBUG_ON_4L        0x7F    // Enabled
#define _DEBUG_OFF_4L       0xFF    // Disabled

#define _LVP_ON_4L          0xFF    // Enabled
#define _LVP_OFF_4L         0xFB    // Disabled

#define _STVR_ON_4L         0xFF	// Stack over/underflow Reset enable
#define _STVR_OFF_4L        0xFE

//Configuration Byte 5L Options
#define _CP0_ON_5L             0xFE    // Enabled
#define _CP0_OFF_5L            0xFF    // Disabled

#define _CP1_ON_5L             0xFD    // Enabled
#define _CP1_OFF_5L            0xFF    // Disabled

//Configuration Byte 5H Options
#define _CPB_ON_5H             0xBF    // Enabled
#define _CPB_OFF_5H            0xFF    // Disabled

#define _CPD_ON_5H             0x7F    // Enabled
#define _CPD_OFF_5H            0xFF    // Disabled

//Configuration Byte 6L Options
#define _WRT0_ON_6L            0xFE    // Enabled
#define _WRT0_OFF_6L           0xFF    // Disabled

#define _WRT1_ON_6L            0xFD    // Enabled
#define _WRT1_OFF_6L           0xFF    // Disabled

//Configuration Byte 6H Options
#define _WRTB_ON_6H            0xBF    // Enabled
#define _WRTB_OFF_6H           0xFF    // Disabled

#define _WRTC_ON_6H            0xDF    // Enabled
#define _WRTC_OFF_6H           0xFF    // Disabled

#define _WRTD_ON_6H            0x7F    // Enabled
#define _WRTD_OFF_6H           0xFF    // Disabled

//Configuration Byte 7L Options
#define _EBTR0_ON_7L           0xFE    // Enabled
#define _EBTR0_OFF_7L          0xFF    // Disabled

#define _EBTR1_ON_7L           0xFD    // Enabled
#define _EBTR1_OFF_7L          0xFF    // Disabled

//Configuration Byte 7H Options
#define _EBTRB_ON_7H           0xBF    // Enabled
#define _EBTRB_OFF_7H          0xFF    // Disabled

// To use the Configuration Bits, place the following lines in your source code
//  in the following format, and change the configuration value to the desired 
//  setting (such as _BOR_ON_2L).  These are currently commented out here
//  and each __CONFIG line should have the preceding semicolon removed when
//  pasted into your source code.

//  The following is a assignment of address values for all of the configuration
//  registers for the purpose of table reads
#define _CONFIG1H           0x300001
#define _CONFIG2L           0x300002
#define _CONFIG2H           0x300003
#define _CONFIG4L           0x300006
#define _CONFIG5L           0x300008
#define _CONFIG5H           0x300009
#define _CONFIG6L           0x30000A
#define _CONFIG6H           0x30000B
#define _CONFIG7L           0x30000C
#define _CONFIG7H           0x30000D
#define _DEVID1             0x3FFFFE
#define _DEVID2             0x3FFFFF
#define _IDLOC0             0x200000
#define _IDLOC1             0x200001
#define _IDLOC2             0x200002
#define _IDLOC3             0x200003
#define _IDLOC4             0x200004
#define _IDLOC5             0x200005
#define _IDLOC6             0x200006
#define _IDLOC7             0x200007

//Program Configuration Register 1H
//		__CONFIG    _CONFIG1H, _OSCS_OFF_1H & _RCIO_OSC_1H

//Program Configuration Register 2L
//		__CONFIG    _CONFIG2L, _BOR_ON_2L & _BORV_25_2L & _PWRT_OFF_2L

//Program Configuration Register 2H
//		__CONFIG    _CONFIG2H, _WDT_ON_2H & _WDTPS_128_2H

//Program Configuration Register 4L
//		__CONFIG    _CONFIG4L, _STVR_ON_4L

//ID Locations Register 0
//		__IDLOCS    _IDLOC0, <expression>

//ID Locations Register 1
//		__IDLOCS    _IDLOC1, <expression>

//ID Locations Register 2
//		__IDLOCS    _IDLOC2, <expression>

//ID Locations Register 3
//		__IDLOCS    _IDLOC3, <expression>

//ID Locations Register 4
//		__IDLOCS    _IDLOC4, <expression>

//ID Locations Register 5
//		__IDLOCS    _IDLOC5, <expression>

//ID Locations Register 6
//		__IDLOCS    _IDLOC6, <expression>

//ID Locations Register 7
//		__IDLOCS    _IDLOC7, <expression>

//Device ID registers hold device ID and revision number and can only be read
//Device ID Register 1
//               DEV2, DEV1, DEV0, REV4, REV3, REV2, REV1, REV0
//Device ID Register 2
//               DEV10, DEV9, DEV8, DEV7, DEV6, DEV5, DEV4, DEV3


//==========================================================================
#LIST

#endif
