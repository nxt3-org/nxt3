//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-01-09 10:34                                    $
//
// Filename        $Workfile:: d_input.c                                     $
//
// Version         $Revision:: 12                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_inpu $
//
// Platform        C
//

#include  "stdconst.h"
#include  "m_sched.h"
#include  "c_input.h"
#include  "d_input.h"
#include  <unistd.h>
#include  <time.h>


void      dInputInit(void) {
}

void      dInputSetColorClkInput(void) {
}

void      dInputGetAllColors(COLORSTRUCT *pRaw, UBYTE Status) {
}

void      dInputGetRawAd(UWORD *pValues, UBYTE No) {
}

void      dInputSetDirOutDigi0(UBYTE Port) {
}

void      dInputSetDirOutDigi1(UBYTE Port) {
}

void      dInputSetDirInDigi0(UBYTE Port) {
}

void      dInputSetDirInDigi1(UBYTE Port) {
}

void      dInputClearDigi0(UBYTE Port) {
}

void      dInputClearDigi1(UBYTE Port) {
}

void      dInputSetDigi0(UBYTE Port) {
}

void      dInputSetDigi1(UBYTE Port) {
}

void      dInputRead0(UBYTE Port, UBYTE *pData) {
}

void      dInputRead1(UBYTE Port, UBYTE * pData) {
}

void      dInputSetActive(UBYTE Port) {
}

void      dInputSet9v(UBYTE Port) {
}

void      dInputSetInactive(UBYTE Port) {
}

UBYTE     dInputGetColor(UBYTE No, UWORD *pCol) {
  return FALSE;
}

void      dInputColorTx(UBYTE Port, UBYTE Data) {
}

void      dInputReadCal(UBYTE Port, UBYTE *pData) {
}

UBYTE     dInputCheckColorStatus(UBYTE Port) {
  return TRUE;
}

void      dInputClearColor100msTimer(UBYTE No) {
}

UBYTE     dInputChkColor100msTimer(UBYTE No) {
  return TRUE;
}

void      dInputExit(void) {
}

void      dInputClockHigh(UBYTE Port) {
}

void      dInputClockLow(UBYTE Port) {
}

void      dInputWaitUS(UBYTE usec) {
  struct timespec wait = {
    .tv_sec  = 0,
    .tv_nsec = 1000 * usec
  };
  while (nanosleep(&wait, &wait) < 0);
}
