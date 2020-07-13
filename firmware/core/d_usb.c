//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_usb.c                                       $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_usb. $
//
// Platform        C
//

#include  "stdconst.h"
#include  "m_sched.h"
#include  "d_usb.h"

static    UBYTE UsbHandleList[MAX_HANDLES];
static    UBYTE UsbHandleCnt;
static    UBYTE BrickNameKnown;

void dUsbStoreBtAddress(UBYTE *pBtAddress) {
  BrickNameKnown = TRUE;
}

ULONG  dUsbRead(UBYTE *pData, ULONG Length) {
  return 0;
}

ULONG  dUsbWrite( const UBYTE *pData, ULONG Length) {
  return 0;
}

UBYTE  dUsbIsConfigured(void) {
  return FALSE;
}

void      dUsbInsertHandle(UBYTE Handle)
{
  UBYTE   Tmp;

  Tmp = 0;
  while((UsbHandleList[Tmp] != MAX_HANDLES) && (Tmp < MAX_HANDLES))
  {
    Tmp++;
  }
  UsbHandleList[Tmp] = Handle;
}

void      dUsbRemoveHandle(UBYTE Handle)
{
  UBYTE   Tmp;

  Tmp = 0;
  while (Tmp < MAX_HANDLES)
  {
    if (Handle == UsbHandleList[Tmp])
    {
      UsbHandleList[Tmp] = MAX_HANDLES;
    }
    Tmp++;
  }
}

UWORD  dUsbGetFirstHandle(void) {
  UsbHandleCnt = 0;
  return dUsbGetNextHandle();
}

UWORD  dUsbGetNextHandle(void) {
  UBYTE   Tmp;
  UWORD   RtnVal;

  RtnVal = 0;
  Tmp = UsbHandleCnt;
  while ((Tmp < MAX_HANDLES) && (MAX_HANDLES == UsbHandleList[Tmp])) {
    Tmp++;
  }
  UsbHandleCnt = Tmp + 1;

  if (Tmp < MAX_HANDLES) {
    RtnVal |= UsbHandleList[Tmp];
  } else {
    RtnVal = 0x8100;
  }

  return RtnVal;
}

UWORD  dUsbCheckConnection(void) {
  return FALSE;
}

void   dUsbResetConfig(void) {
}

void   dUsbInit(void) {
  BrickNameKnown = FALSE;
  for (int hnd = 0; hnd < MAX_HANDLES; hnd++) {
    UsbHandleList[hnd] = MAX_HANDLES;
  }
}

void   dUsbExit(void) {
}
