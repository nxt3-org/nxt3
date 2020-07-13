//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_hispeed.c                                   $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_hisp $
//
// Platform        C
//

#include  "stdconst.h"
#include  "m_sched.h"
#include  "d_hispeed.h"
#include  <string.h>


void dHiSpeedInit(void)
{
  // stub-only for now
}

void dHiSpeedSendData(UBYTE *OutputBuffer, UBYTE BytesToSend)
{}

void dHiSpeedSetupUart(UBYTE speed, UWORD mode, UBYTE umode)
{}

void dHiSpeedInitReceive(UBYTE *InputBuffer)
{}

void dHiSpeedReceivedData(UWORD *ByteCnt) {
  *ByteCnt = 0;
}

void dHiSpeedExit(void)
{}

void dHiSpeedBytesToSend(UWORD *bts) {
  *bts = 0;
}
