//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_lowspeed.iom                                $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_lows $
//
// Platform        C
//

#ifndef   CLOWSPEED_IOM
#define   CLOWSPEED_IOM

#define   pMapLowSpeed ((IOMAPLOWSPEED*)(pHeaders[ENTRY_LOWSPEED]->pIOMap))

#define   NO_OF_LOWSPEED_COM_CHANNEL  4
#define   NO_OF_LSBUF                  NO_OF_LOWSPEED_COM_CHANNEL
#define   SIZE_OF_LSBUF                  16

//Constants referring to LowSpeedDeviceType
enum {
    ULTRA_SONIC = 2,
    CUSTOM_LS_DEVICE
};

// Constants referring to ChannelState
enum {
    LOWSPEED_IDLE,
    LOWSPEED_INIT,
    LOWSPEED_LOAD_BUFFER,
    LOWSPEED_COMMUNICATING,
    LOWSPEED_ERROR,
    LOWSPEED_DONE
};

// Constants referring to Mode
enum {
    LOWSPEED_TRANSMITTING = 1,
    LOWSPEED_RECEIVING,
    LOWSPEED_DATA_RECEIVED
};

// Constants referring to ErrorType
enum {
    LOWSPEED_NO_ERROR          = 0,
    LOWSPEED_CH_NOT_READY,
    LOWSPEED_TX_ERROR,
    LOWSPEED_RX_ERROR,
    LOWSPEED_FAST_ERROR_INVALID_PORT = -1,
    LOWSPEED_FAST_ERROR_BUSY = -2,
    LOWSPEED_FAST_ERROR_FAULT = -3,
    LOWSPEED_FAST_ERROR_INVALID_LENGTH = -4,
    LOWSPEED_FAST_ERROR_BUS_BUSY = -5
};

typedef struct {
    UBYTE Buf[SIZE_OF_LSBUF];
    UBYTE InPtr;
    UBYTE OutPtr;
    UBYTE BytesToRx;
} LSBUF;

typedef struct {
    LSBUF InBuf[NO_OF_LSBUF];
    LSBUF OutBuf[NO_OF_LSBUF];
    UBYTE Mode[NO_OF_LSBUF];
    UBYTE ChannelState[NO_OF_LSBUF];
    UBYTE ErrorType[NO_OF_LSBUF];
    UBYTE Active;
    UBYTE FastMask;      // synchronous hw i2c transactions
    UBYTE NoRestartMask; // do not restart bus for reading data
    UBYTE Spare1;

    SBYTE (*pFunc)(UBYTE);
} IOMAPLOWSPEED;


#endif



