//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_lowspeed.c                                  $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_lows $
//
// Platform        C
//

#include  "stdconst.h"
#include  "modules.h"
#include  "c_lowspeed.iom.h"
#include  "c_input.iom.h"
#include  "c_lowspeed.h"
#include <string.h>
#include <hal_general.h>

static IOMAPLOWSPEED IOMapLowSpeed;
static VARSLOWSPEED  VarsLowSpeed;
static HEADER        **pHeaders;

const HEADER cLowSpeed = {
    0x000B0001L,
    "Low Speed",
    cLowSpeedInit,
    cLowSpeedCtrl,
    cLowSpeedExit,
    (void *) &IOMapLowSpeed,
    (void *) &VarsLowSpeed,
    (UWORD) sizeof(IOMapLowSpeed),
    (UWORD) sizeof(VarsLowSpeed),
    0x0000                      //Code size - not used so far
};

SBYTE cLowSpeedFastI2C(UBYTE ch);

void cLowSpeedInit(void *pHeader) {
    pHeaders = pHeader;

    IOMapLowSpeed.Active        = 0x00;
    IOMapLowSpeed.NoRestartMask = 0x00;
    IOMapLowSpeed.FastMask      = 0x00;
    IOMapLowSpeed.pFunc         = &cLowSpeedFastI2C;
}

void cLowSpeedLoadWriteBuffer(UBYTE ch) {
    VarsLowSpeed.OutputBuf[ch].OutPtr = 0;
    VarsLowSpeed.OutputBuf[ch].InPtr  = IOMapLowSpeed.OutBuf[ch].InPtr;

    memcpy(VarsLowSpeed.OutputBuf[ch].Buf,
           IOMapLowSpeed.OutBuf[ch].Buf,
           IOMapLowSpeed.OutBuf[ch].InPtr);

    IOMapLowSpeed.OutBuf[ch].OutPtr = IOMapLowSpeed.OutBuf[ch].InPtr;
}

void cLowSpeedCompleteRead(UBYTE ch) {
    UBYTE *iPos = &VarsLowSpeed.InputBuf[ch].Buf[0];
    UBYTE *iEnd = &VarsLowSpeed.InputBuf[ch].Buf[IOMapLowSpeed.InBuf[ch].BytesToRx];

    UBYTE *oBeg = &IOMapLowSpeed.InBuf[ch].Buf[0];
    UBYTE *oPos = &IOMapLowSpeed.InBuf[ch].Buf[IOMapLowSpeed.InBuf[ch].InPtr];
    UBYTE *oWrp = &IOMapLowSpeed.InBuf[ch].Buf[SIZE_OF_LSBUF];

    while (iPos != iEnd) {
        *oPos = *iPos;

        ++iPos;
        ++oPos;
        if (oPos == oWrp)
            oPos = oBeg;
    }

    IOMapLowSpeed.InBuf[ch].InPtr = oPos - oBeg;
}

void cLowSpeedFinished(UBYTE ch, UBYTE bDone) {
    IOMapLowSpeed.Active &= ~(1 << ch);
    if (bDone) {
        IOMapLowSpeed.ChannelState[ch] = LOWSPEED_IDLE;
    }
}

SBYTE cLowSpeedFastI2C(UBYTE ch) {
    SBYTE            failCode;
    hal_iic_result_t result;

    // 0. check for validity
    if (ch >= 4) {
        failCode = LOWSPEED_FAST_ERROR_INVALID_PORT;
        goto failure;
    }
    if (!Hal_IicHost_Present(ch)) {
        failCode = LOWSPEED_FAST_ERROR_FAULT;
        goto failure;
    }

    // 1. load write payload
    cLowSpeedLoadWriteBuffer(ch);

    // 2. write it
    result = Hal_IicDev_Start(VarsLowSpeed.Devices[ch],
                              VarsLowSpeed.OutputBuf[ch].Buf,
                              VarsLowSpeed.OutputBuf->InPtr);
    if (result == HAL_IIC_RESULT_ERROR) {
        failCode = LOWSPEED_FAST_ERROR_FAULT;
        goto failure;
    }
    if (result == HAL_IIC_RESULT_PROCESSING) {
        failCode = LOWSPEED_FAST_ERROR_BUSY;
        goto failure;
    }

    // 3. read back if needed
    if (IOMapLowSpeed.InBuf[ch].BytesToRx > 0) {
        do {
            result = Hal_IicDev_Poll(VarsLowSpeed.Devices[ch],
                                     VarsLowSpeed.InputBuf[ch].Buf,
                                     IOMapLowSpeed.InBuf[ch].BytesToRx);
        } while (result == HAL_IIC_RESULT_PROCESSING);
        if (result == HAL_IIC_RESULT_ERROR) {
            failCode = LOWSPEED_FAST_ERROR_FAULT;
            memset(VarsLowSpeed.InputBuf[ch].Buf, 0x00, SIZE_OF_LSBUF);
            VarsLowSpeed.InputBuf[ch].OutPtr = IOMapLowSpeed.InBuf[ch].BytesToRx;
            goto failure;
        } else {
            cLowSpeedCompleteRead(ch);
        }
    }

    // 4. finalize
    cLowSpeedFinished(ch, TRUE);
    return LOWSPEED_NO_ERROR;
failure:
    cLowSpeedFinished(ch, FALSE);
    return failCode;
}

void cLowSpeedCtrl(void) {
    int port;
    for (port = 0; port < NO_OF_LOWSPEED_COM_CH; port++) {
        if (Hal_IicHost_Present(port))
            Hal_IicDev_Tick(VarsLowSpeed.Devices[port]);
    }

    if (IOMapLowSpeed.Active == 0)
        return;

    hal_iic_result_t result;

    for (port = 0; port < NO_OF_LOWSPEED_COM_CHANNEL; port++) {

        switch (IOMapLowSpeed.ChannelState[port]) {
        case LOWSPEED_IDLE:
            break;

        case LOWSPEED_INIT:
            if (!(pMapInput->Inputs[port].SensorType == LOWSPEED) &&
                !(pMapInput->Inputs[port].SensorType == LOWSPEED_9V)) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_CH_NOT_READY;
                break;
            }

            if (!Hal_IicHost_Present(port)) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_CH_NOT_READY;
                break;
            }

            if (IOMapLowSpeed.FastMask & (1 << port)) {
                cLowSpeedFastI2C(port);
            } else {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_LOAD_BUFFER;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_NO_ERROR;
            }
            break;

        case LOWSPEED_LOAD_BUFFER:
            if (!(pMapInput->Inputs[port].SensorType == LOWSPEED) &&
                !(pMapInput->Inputs[port].SensorType == LOWSPEED_9V)) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_CH_NOT_READY;
                break;
            }

            cLowSpeedLoadWriteBuffer(port);

            result = Hal_IicDev_Start(VarsLowSpeed.Devices[port],
                                      VarsLowSpeed.OutputBuf[port].Buf,
                                      VarsLowSpeed.OutputBuf->InPtr);

            if (result == HAL_IIC_RESULT_DONE) {
                VarsLowSpeed.RxTimeCnt[port]     = 0;
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_COMMUNICATING;
                IOMapLowSpeed.Mode[port]         = LOWSPEED_TRANSMITTING;

            } else if (result == HAL_IIC_RESULT_PROCESSING) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_CH_NOT_READY;

            } else if (result == HAL_IIC_RESULT_ERROR) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_TX_ERROR;
            }
            break;

        case LOWSPEED_COMMUNICATING:
            if (!(pMapInput->Inputs[port].SensorType == LOWSPEED) &&
                !(pMapInput->Inputs[port].SensorType == LOWSPEED_9V)) {
                IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                IOMapLowSpeed.ErrorType[port]    = LOWSPEED_CH_NOT_READY;
                break;
            }

            if (IOMapLowSpeed.Mode[port] == LOWSPEED_TRANSMITTING) {
                if (IOMapLowSpeed.InBuf[port].BytesToRx != 0) {
                    IOMapLowSpeed.Mode[port] = LOWSPEED_RECEIVING;
                } else {
                    IOMapLowSpeed.Mode[port]         = LOWSPEED_DATA_RECEIVED;
                    IOMapLowSpeed.ChannelState[port] = LOWSPEED_DONE;
                }
            }

            if (IOMapLowSpeed.Mode[port] == LOWSPEED_RECEIVING) {
                VarsLowSpeed.RxTimeCnt[port]++;
                if (VarsLowSpeed.RxTimeCnt[port] > LOWSPEED_RX_TIMEOUT) {
                    Hal_IicDev_Cancel(VarsLowSpeed.Devices[port]);
                    IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                    IOMapLowSpeed.ErrorType[port]    = LOWSPEED_RX_ERROR;
                }
                result = Hal_IicDev_Poll(VarsLowSpeed.Devices[port],
                                         VarsLowSpeed.InputBuf[port].Buf,
                                         IOMapLowSpeed.InBuf[port].BytesToRx);

                if (result == HAL_IIC_RESULT_DONE) {
                    cLowSpeedCompleteRead(port);
                    IOMapLowSpeed.Mode[port]         = LOWSPEED_DATA_RECEIVED;
                    IOMapLowSpeed.ChannelState[port] = LOWSPEED_DONE;
                }
                if (result == HAL_IIC_RESULT_ERROR) {
                    //There was and error in receiving data from the device
                    memset(VarsLowSpeed.InputBuf[port].Buf, 0x00, SIZE_OF_LSBUF);
                    VarsLowSpeed.InputBuf[port].OutPtr = IOMapLowSpeed.InBuf[port].BytesToRx;
                    IOMapLowSpeed.ChannelState[port] = LOWSPEED_ERROR;
                    IOMapLowSpeed.ErrorType[port]    = LOWSPEED_RX_ERROR;
                }
            }
            break;

        case LOWSPEED_ERROR: {
            cLowSpeedFinished(port, FALSE);
        }
            break;
        case LOWSPEED_DONE: {
            cLowSpeedFinished(port, TRUE);
        }
            break;
        default:
            break;
        }
    }
}

void cLowSpeedExit(void) {
    for (int port = 0; port < NO_OF_LOWSPEED_COM_CH; port++) {
        if (!VarsLowSpeed.Devices[port])
            continue;

        if (IOMapLowSpeed.Active & (1 << port)) {
            Hal_IicDev_Cancel(VarsLowSpeed.Devices[port]);
            IOMapLowSpeed.Active &= ~(1 << port);
        }
        Hal_IicHost_Detach(port);
    }
}

bool Hal_IicHost_Attach(hal_iic_dev_t *device, int port) {
    if (port >= 4 || port < 0)
        return false;
    if (VarsLowSpeed.Devices[port])
        return false;

    if (Hal_IicDev_JustAttached(device)) {
        VarsLowSpeed.Devices[port] = device;
        return true;
    }
    return false;
}

bool Hal_IicHost_Detach(int port) {
    if (port >= 4 || port < 0)
        return false;
    if (!VarsLowSpeed.Devices[port])
        return true;

    Hal_IicDev_Cancel(VarsLowSpeed.Devices[port]);
    Hal_IicDev_JustDetached(VarsLowSpeed.Devices[port]);
    VarsLowSpeed.Devices[port] = NULL;
    return true;
}

bool Hal_IicHost_Present(int port) {
    if (port >= 4 || port < 0)
        return false;
    return VarsLowSpeed.Devices[port] != NULL;
}
