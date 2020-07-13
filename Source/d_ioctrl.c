//
// Date init       14.12.2004
//
// Revision date   $Date:: 5-12-07 15:23                                     $
//
// Filename        $Workfile:: d_ioctrl.c                                    $
//
// Version         $Revision:: 2                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_ioct $
//
// Platform        C
//


#include  <string.h>
#include  <stdlib.h>
#include  "stdconst.h"
#include  "m_sched.h"
#include  "d_ioctrl.h"
#include  "c_cmd.iom.h"


static    HEADER    **pHeaders;


void      dIOCtrlInit(void* pHeader) {
  pHeaders = pHeader;
}

void      dIOCtrlSetPower(UBYTE Power)
{
  if (Power != 0) {
    pMapCmd->Awake = FALSE;
  }
}

void      dIOCtrlSetPwm(UBYTE Pwm)
{}

void      dIOCtrlTransfer(void)
{}

void      dIOCtrlExit(void)
{}

