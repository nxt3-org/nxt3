//
// Date init       14.12.2004
//
// Revision date   $Date:: 24-04-08 14:33                                    $
//
// Filename        $Workfile:: d_bt.c                                        $
//
// Version         $Revision:: 3                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_bt.c $
//
// Platform        C
//


#include  "stdconst.h"
#include  "modules.h"
#include  "m_sched.h"
#include  "d_bt.h"
#include  <string.h>

enum
{
  BT_FAST_TIMEOUT   = 500,
  BT_CMD_TIMEOUT_2S = 2000,
  BT_TIMEOUT_30S    = 30000
};

void      dBtInit(void) {
}

void      dBtSetBcResetPinLow(void) {
}

void      dBtSetBcResetPinHigh(void) {
}

void      dBtStartADConverter(void) {
}

void      dBtInitReceive(UBYTE *InputBuffer, UBYTE Mode, UBYTE NoLengthBytes) {
}

void      dBtSetArm7CmdSignal(void) {
}

void      dBtClearArm7CmdSignal(void) {
}

UBYTE     dBtGetBc4CmdSignal(void) {
  return FALSE;
}


UWORD     dBtTxEnd(void) {
  return TRUE;
}

UWORD     dBtCheckForTxBuf(void) {
  return 0;
}

void      dBtSendMsg(UBYTE *OutputBuffer, UBYTE BytesToSend, UWORD MsgSize) {
}

void      dBtSend(UBYTE *OutputBuffer, UBYTE BytesToSend) {
}

UWORD     dBtReceivedData(UWORD *pLength, UWORD *pBytesToGo, UBYTE NoLengthBytes) {
  return 0;
}

void      dBtResetTimeOut(void) {
}

void      dBtClearTimeOut(void) {
}

void      dBtSendBtCmd(UBYTE Cmd, UBYTE Param1, UBYTE Param2, UBYTE *pBdAddr, UBYTE *pName, UBYTE *pCod, UBYTE *pPin) {
  switch (Cmd) {
    case MSG_BEGIN_INQUIRY:
      break;

    case MSG_CANCEL_INQUIRY:
      break;

    case MSG_CONNECT:
      break;

    case MSG_OPEN_PORT:
      break;

    case MSG_LOOKUP_NAME:
      break;

    case MSG_ADD_DEVICE:
      break;

    case MSG_REMOVE_DEVICE:
      break;

    case MSG_DUMP_LIST:
      break;

    case MSG_CLOSE_CONNECTION:
      break;

    case MSG_ACCEPT_CONNECTION:
      break;

    case MSG_PIN_CODE:
      break;

    case MSG_OPEN_STREAM:
      break;

    case MSG_START_HEART:
      break;

    case MSG_SET_DISCOVERABLE:
      break;

    case MSG_CLOSE_PORT:
      break;

    case MSG_SET_FRIENDLY_NAME:
      break;

    case MSG_GET_LINK_QUALITY:
      break;

    case MSG_SET_FACTORY_SETTINGS:
      break;

    case MSG_GET_LOCAL_ADDR:
      break;

    case MSG_GET_FRIENDLY_NAME:
      break;

    case MSG_GET_DISCOVERABLE:
      break;

    case MSG_GET_PORT_OPEN:
      break;

    case MSG_GET_VERSION:
      break;

    case MSG_GET_BRICK_STATUSBYTE:
      break;

    case MSG_SET_BRICK_STATUSBYTE:
      break;
  }
}



void      dBtExit(void) {
}
