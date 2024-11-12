/*********************************************************************
 *              PIC18CXX8 CAN C Library Header File
 *********************************************************************
 * FileName:        CAN18CXX8.h
 * Dependencies:    None
 * Date:            09/06/00
 * Processor:       PIC18CXX8
 * Complier:        MPLAB 5.11.00
 * Company:         Microchip Technology, Inc.
 *
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 *  Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     9/6/00  Original        (Rev 1.0)
 * Nilesh Rajbharti     12/1/00 Fixed bugs.
 *                              (CANRegsToID, CANReceiveMessage,
 *                               CANSetMask + Added
 *                               CAN_CONFIG_DBL_BUFFER_ON/OFF option)
 * Nilesh Rajbharti     6/8/01  Renamed CAN_CONFIG_DBL_BUFFERED to
 *                              CAN_RX_DBL_BUFFERED
 *
 * Bernie Lafreniere	8/16/03 Start porting driver to run on CCS C with
 *								the PIC18F248 processor.
 *
 *********************************************************************/
#ifndef CAN18XX8_H          // To avoid duplicate inclusion
#define CAN18XX8_H

#bit RXB0CON_RX0DBEN    = RXB0CON.2	// PIC18XX8 CAN register.bit mapping
#bit RXB0CON_RXFUL      = RXB0CON.7
#bit RXB1CON_RXFUL      = RXB1CON.7
#bit TXB0CON_TXREQ      = TXB0CON.3

#bit TXB1CON_TXPRI0     = TXB1CON.0
#bit TXB1CON_TXPRI1     = TXB1CON.1
#bit TXB1CON_TXREQ      = TXB1CON.3
#bit TXB1CON_TXERR      = TXB1CON.4
#bit TXB1CON_TXLARB     = TXB1CON.5
#bit TXB1CON_TXABT      = TXB1CON.6

#bit TXB2CON_TXPRI0     = TXB2CON.0
#bit TXB2CON_TXPRI1     = TXB2CON.1
#bit TXB2CON_TXREQ      = TXB2CON.3
#bit TXB2CON_TXERR      = TXB2CON.4
#bit TXB2CON_TXLARB     = TXB2CON.5
#bit TXB2CON_TXABT      = TXB2CON.6

#bit BRGCON2_SAM        = BRGCON2.6
#bit BRGCON2_SEG2PHTS   = BRGCON2.7
#bit BRGCON3_WAKFIL     = BRGCON3.6
#bit PIR3_RXB0IF        = PIR3.0
#bit PIR3_RXB1IF        = PIR3.1
#bit PIR3_IRXIF         = PIR3.7
#bit COMSTAT_TXB0       = COMSTAT.5
#bit COMSTAT_TXBP       = COMSTAT.4
#bit COMSTAT_RXBP       = COMSTAT.3
#bit COMSTAT_RX1OVFL    = COMSTAT.6
#bit COMSTAT_RX0OVFL    = COMSTAT.7
#bit RXB0DLC_RTR        = RXB0DLC.6
#bit RXB0SIDL_EXID      = RXB0SIDL.3
#bit CANCON_ABAT        = CANCON.4

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * union CAN_MESSAGE_ID
 *
 * This union provides abstract data type for CAN message id.
 * It is used for both 11-bit and 29-bit message identifiers.
 * There are multiple union members to be able to access individual
 * parts of it.
 *
 ********************************************************************/
 // Parse-out 29-bit or 11-bit (saved in 32-bit number)
typedef union _CAN_MESSAGE_ID
{
    int32 ID;

    struct
    {
        struct
        {
            int8 SIDL:3;        // SIDL<5:7>
            int8 SIDH:5;        // SIDH<0:4>
        } BYTE1;
        struct
        {
            int8 SIDHU:3;       // SIDH<5:7>
            int8 EIDL_LN:5;     // EIDL<0:4>
        } BYTE2;
        struct
        {
            int8 EIDL_UN:3;     // EIDL<5:7>
            int8 EIDH_LN:5;     // EIDH<0:4>
        } BYTE3;
        struct
        {
            int8 EIDH_UN:3;     // EIDH<5:7>
            int8 EIDHU:2;       // SIDL<0:1>
            int8 PAD:3;			// bcl added name "PAD" to make CCS happy
        } BYTE4;
    } ID_VALS;

    // This is to allow individual byte access within message id.
    struct
    {
        BYTE BYTE_1;
        BYTE BYTE_2;
        BYTE BYTE_3;
        BYTE BYTE_4;
    } BYTES;
} CAN_MESSAGE_ID;

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_TX_MSG_FLAGS
 *
 * This enumeration values define flags related to transmission of a
 * CAN message.  There could be more than one this flag
 * ANDed together to form multiple flags.
 *
 *********************************************************************/
 typedef enum CAN_TX_MSG_FLAGS
 {
    CAN_TX_PRIORITY_BITS= 0b00000011,
    CAN_TX_PRIORITY_0   = 0b11111100,   // XXXXXX00
    CAN_TX_PRIORITY_1   = 0b11111101,   // XXXXXX01
    CAN_TX_PRIORITY_2   = 0b11111110,   // XXXXXX10
    CAN_TX_PRIORITY_3   = 0b11111111,   // XXXXXX11

    CAN_TX_FRAME_BIT    = 0b00001000,
    CAN_TX_STD_FRAME    = 0b11111111,   // XXXXX1XX
    CAN_TX_XTD_FRAME    = 0b11110111,   // XXXXX0XX

    CAN_TX_RTR_BIT      = 0b01000000,
    CAN_TX_NO_RTR_FRAME = 0b11111111,   // X1XXXXXX
    CAN_TX_RTR_FRAME    = 0b10111111    // X0XXXXXX
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_RX_MSG_FLAGS
 *
 * These enumeration values define flags related to reception of a CAN
 * message.  There could be more than one these flags
 * ANDed together to form multiple flags.
 * If a particular bit is set, corresponding meaning is TRUE or else
 * it will be FALSE.
 *
 * e.g.
 *      if (MsgFlag & CAN_RX_OVERFLOW)
 *      {
 *          // Receiver overflow has occured.  We have lost previous
 *          // message.
 *          ...
 *      }
 *
 ********************************************************************/
typedef enum CAN_RX_MSG_FLAGS
{
    CAN_RX_FILTER_BITS  = 0b00000111,   // Use this to access filter
                                        // bits
    CAN_RX_FILTER_1     = 0b00000000,
    CAN_RX_FILTER_2     = 0b00000001,
    CAN_RX_FILTER_3     = 0b00000010,
    CAN_RX_FILTER_4     = 0b00000011,
    CAN_RX_FILTER_5     = 0b00000100,
    CAN_RX_FILTER_6     = 0b00000101,

    CAN_RX_OVERFLOW     = 0b00001000,   // Set if Overflowed else
                                        // cleared

    CAN_RX_INVALID_MSG  = 0b00010000,   // Set if invalid else
                                        // cleared

    CAN_RX_XTD_FRAME    = 0b00100000,   // Set if XTD message else
                                        // cleared

    CAN_RX_RTR_FRAME    = 0b01000000,   // Set if RTR message else
                                        // cleared

    CAN_RX_DBL_BUFFERED = 0b10000000    // Set if this message was
                                        // hardware double-buffered
};

//////////////////////////////////////////////////////////////////////


/*********************************************************************
 *
 * enum CAN_MASK
 *
 * These enumeration values define mask codes.  Routine CANSetMask()
 * requires this code as one of its arguments.  These enumerations
 * must be used by itself i.e. it cannot be ANDed to form multiple
 * values.
 *
 ********************************************************************/
typedef enum CAN_MASK
{
     CAN_MASK_B1,
     CAN_MASK_B2
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_FILTER
 *
 * These enumeration values define filter codes.  Routine CANSetFilter
 * requires this code as one of its arguments.  These enumerations
 * must be used by itself
 * i.e. it cannot be ANDed to form multiple values.
 *
 ********************************************************************/
typedef enum CAN_FILTER
{
    CAN_FILTER_B1_F1,
    CAN_FILTER_B1_F2,
    CAN_FILTER_B2_F1,
    CAN_FILTER_B2_F2,
    CAN_FILTER_B2_F3,
    CAN_FILTER_B2_F4
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_OP_MODE
 *
 * This enumeration values define codes related to CAN module
 * operation mode. CANSetOperationMode() routine requires this code.
 * These values must be used by itself
 * i.e. it cannot be ANDed to form * multiple values.
 *
 ********************************************************************/
typedef enum CAN_OP_MODE
{
    CAN_OP_MODE_BITS    = 0b11100000,   // Use this to access opmode
                                        // bits
    CAN_OP_MODE_NORMAL  = 0b00000000,
    CAN_OP_MODE_SLEEP   = 0b00100000,
    CAN_OP_MODE_LOOP    = 0b01000000,
    CAN_OP_MODE_LISTEN  = 0b01100000,
    CAN_OP_MODE_CONFIG  = 0b10000000
};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 *
 * enum CAN_CONFIG_FLAGS
 *
 * These enumeration values define flags related to configuring the CAN
 * module. Routines CANInitialize() and CANSetBaudRate() use these
 * codes. One or more of these values may be ANDed to form multiple
 * flags.
 *
 ********************************************************************/
//typedef enum CAN_CONFIG_FLAGS
//{
#define    CAN_CONFIG_DEFAULT           0b11111111,   // 11111111

#define    CAN_CONFIG_PHSEG2_PRG_BIT    0b00000001
#define    CAN_CONFIG_PHSEG2_PRG_ON     0b11111111   // XXXXXXX1
#define    CAN_CONFIG_PHSEG2_PRG_OFF    0b11111110   // XXXXXXX0

#define    CAN_CONFIG_LINE_FILTER_BIT   0b00000010
#define    CAN_CONFIG_LINE_FILTER_ON    0b11111111   // XXXXXX1X
#define    CAN_CONFIG_LINE_FILTER_OFF   0b11111101   // XXXXXX0X

#define    CAN_CONFIG_SAMPLE_BIT        0b00000100
#define    CAN_CONFIG_SAMPLE_ONCE       0b11111111   // XXXXX1XX
#define    CAN_CONFIG_SAMPLE_THRICE     0b11111011   // XXXXX0XX

#define    CAN_CONFIG_MSG_TYPE_BIT      0b00001000
#define    CAN_CONFIG_STD_MSG           0b11111111   // XXXX1XXX
#define    CAN_CONFIG_XTD_MSG           0b11110111   // XXXX0XXX

#define    CAN_CONFIG_DBL_BUFFER_BIT    0b00010000
#define    CAN_CONFIG_DBL_BUFFER_ON     0b11111111   // XXX1XXXX
#define    CAN_CONFIG_DBL_BUFFER_OFF    0b11101111   // XXX0XXXX

#define    CAN_CONFIG_MSG_BITS          0b01100000
#define    CAN_CONFIG_ALL_MSG           0b11111111   // X11XXXXX
#define    CAN_CONFIG_VALID_XTD_MSG     0b11011111   // X10XXXXX
#define    CAN_CONFIG_VALID_STD_MSG     0b10111111   // X01XXXXX
#define    CAN_CONFIG_ALL_VALID_MSG     0b10011111    // X00XXXXX
//};

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANInitialize( BYTE SJW,
 *                                      BYTE BRP,
 *                                      BYTE PHSEG1,
 *                                      BYTE PHSEG2,
 *                                      BYTE PROPSEG,
 *                                      BYTE config)
 *
 * PreCondition:    MCU must be in Configuration mode or else these
 *                  values will be ignored.
 *
 * Input:           SJW     - SJW value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 4)
 *                  BRP     - BRP value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 64)
 *                  PHSEG1  - PHSEG1 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PHSEG2  - PHSEG2 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PROPSEG - PROPSEG value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  flags   - Value of type enum CAN_CONFIG_FLAGS
 *
 * Output:          CAN bit rate is set.
 *                  All masks registers are set '0'
 *                  to allow all messages.
 *                  Filter registers are set according to flag value.
 *                  If (config & CAN_CONFIG_VALID_XTD_MSG)
 *                      Set all filters to XTD_MSG
 *                  Else if (config & CONFIG_VALID_STD_MSG)
 *                      Set all filters to STD_MSG
 *                  Else
 *                      Set half of the filters to STD while rests to
 *                      XTD_MSG.
 *
 * Side Effects:    All pending transmissions are aborted.
 ********************************************************************/
void CANInitialize(BYTE SJW,
                    BYTE BRP,
                    BYTE PHSEG1,
                    BYTE PHSEG2,
                    BYTE PROPSEG,
                    BYTE config);

//////////////////////////////////////////////////////////////////////


/*********************************************************************
 * Function:        void CANSetOperationMode(CAN_OP_MODE mode)
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum CAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a blocking call.  It will not return until
 *                  requested mode is set.
 ********************************************************************/
void CANSetOperationMode(CAN_OP_MODE mode);

//////////////////////////////////////////////////////////////////////


/*********************************************************************
 * Macro:           void CANSetOperationModeNoWait(CAN_OP_MODE mode)
 *
 * PreCondition:    None
 *
 * Input:           mode    - Operation mode code
 *                            must be of type enum CAN_OP_MODES
 *
 * Output:          MCU is set to requested mode
 *
 * Side Effects:    None
 *
 * Note:            This is a non-blocking call.
 *                  It does not verify that
 *                  CAN module is switched to requested mode or not.
 *                  Caller must use CANGetOperationMode() to verify
 *                  correct operation mode before performing mode
 *                  specific operation.
 *
 ********************************************************************/
#define CANSetOperationModeNoWait(mode) (CANCON = mode)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           CAN_OP_MODE CANSetOperationMode()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current operational mode of CAN module is returned
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetOperationMode() (CANCON & CAN_OP_MODE_BITS)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetBaudRate(BYTE SJW,
 *                                      BYTE BRP,
 *                                      BYTE PHSEG1,
 *                                      BYTE PHSEG2,
 *                                      BYTE PROPSEG,
 *                                      BYTE flags)
 *
 * PreCondition:    MCU must be in Configuration mode or else these
 *                  values will be ignored.
 *
 * Input:           SJW     - SJW value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 4)
 *                  BRP     - BRP value as defined in 18CXX8 datasheet
 *                              (Must be between 1 thru 64)
 *                  PHSEG1  - PHSEG1 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PHSEG2  - PHSEG2 value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  PROPSEG - PROPSEG value as defined in 18CXX8
 *                            datasheet
 *                              (Must be between 1 thru 8)
 *                  flags   - Value of type enum CAN_CONFIG_FLAGS
 *
 * Output:          CAN bit rate is set as per given values.
 *
 * Side Effects:    None
 *
 ********************************************************************/
void CANSetBaudRate (BYTE SJW,
                     BYTE BRP,
                     BYTE PHSEG1,
                     BYTE PHSEG2,
                     BYTE PROPSEG,
                     BYTE flags);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetMask(enum CAN_MASK code,
 *                                  int32 val,
 *                                  BYTE type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values
 *                  will be ignored.
 *
 * Input:           code    - One of CAN_MASK value
 *                  val     - Actual mask register value.
 *                  type    - Type of message to filter either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          Given value is bit adjusted to appropriate buffer
 *                  mask registers.
 *
 * Side Effects:    None
 ********************************************************************/
void CANSetMask (CAN_MASK code,
                int32 val,
                BYTE type);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        void CANSetFilter(enum CAN_FILTER code,
 *                                    int32 val,
 *                                    BYTE type)
 *
 * PreCondition:    MCU must be in Configuration mode.  If not, all
 *                  values will be ignored.
 *
 * Input:           code    - One of CAN_FILTER value
 *                  val     - Actual filter register value.
 *                  type    - Type of message to filter either
 *                            CAN_CONFIG_XTD_MSG or CAN_CONFIG_STD_MSG
 *
 * Output:          Given value is bit adjusted to appropriate buffer
 *                  filter registers.
 *
 * Side Effects:    None
 ********************************************************************/
void CANSetFilter ( CAN_FILTER code,
                    int32 val,
                    BYTE type);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        BOOL CANSendMessage(int32 id,
 *                                  BYTE *Data,
 *                                  BYTE DataLen,
 *                                  enum CAN_TX_MSG_FLAGS MsgFlags)
 *
 * PreCondition:    None
 *
 * Input:           id          - CAN message identifier.
 *                                Only 11 or 29 bits may be used
 *                                depending on standard or extended
 *                                message type.
 *                  Data        - Data bytes of upto 8 bytes in length
 *                  DataLen     - Data length from 1 thru 8.
 *                  MsgFlags    - One or CAN_TX_MSG_FLAGS values ANDed
 *                                together
 *
 * Output:          If at least one empty transmit buffer is found,
 *                  given message is queued to be transmitted. If none
 *                  found FALSE value is returned.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOLEAN CANSendMessage (int32 id,
                     BYTE *Data,
                     BYTE DataLen,
                     CAN_TX_MSG_FLAGS MsgFlags);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Function:        BOOL CANReceiveMessage(int32 *id,
 *                                  BYTE *Data,
 *                                  BYTE *DataLen,
 *                                  enum CAN_RX_MSG_FLAGS *MsgFlags)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          id          - CAN message identifier.
 *                  Data        - Data bytes of upto 8 bytes in length
 *                  DataLen     - Data length from 1 thru 8.
 *                  MsgFlags    - One or CAN_RX_MSG_FLAGS values ANDed
 *                                together
 *
 * Output:          If at least one full receive buffer is found,
 *                  it is extrated and returned. If none found FALSE
 *                  value is returned.
 *
 * Side Effects:    None
 *
 ********************************************************************/
BOOLEAN CANReceiveMessage(int32* id,
                        BYTE *Data,
                        BYTE *DataLen,
                        CAN_RX_MSG_FLAGS *MsgFlags);

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANGetTxErrorCount()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current transmit error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetTxErrorCount()    (TXERRCNT)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANGetRxErrorCount()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Current receive error count as defined by
 *                  CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANGetRxErrorCount()    (RXERRCNT)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsBusOff()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN Module is off due to excessive error
 *                  FALSE is it is not off.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsBusOff()           (COMSTAT_TXB0)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsTxPassive()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN transmit module is error passive as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsTxPassive()        (COMSTAT_TXBP)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BYTE CANIsRxPassive()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if CAN receive module is error active as
 *                  defined by CAN specifications.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsRxPassive()        (COMSTAT_RXBP)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           void CANAbortAll()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANAbortAll()           (CANCON_ABAT = 1)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL  CANIsRxReady()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if at least one of the CAN receive buffer is
 *                  empty FALSE if none receive buffers are empty.
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsRxReady()          (RXB0CON_RXFUL || RXB1CON_RXFUL)

//////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Macro:           BOOL CANIsTxReady()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE if at least one CAN transmit buffer is empty
 *                  FALSE if all CAN transmit buffers are full
 *
 * Side Effects:    None
 *
 ********************************************************************/
#define CANIsTxReady()          (!TXB0CON_TXREQ || \
                                 !TXB1CON_TXREQ || \
                                 !TXB2CON_TXREQ )

#endif      // CAN18XX8_H

