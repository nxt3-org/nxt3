//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_ioctrl.c                                    $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_ioct $
//
// Platform        C
//


#include  "stdconst.h"
#include  "modules.h"
#include  "c_ioctrl.iom.h"
#include  "c_cmd.iom.h"
#include  "c_ioctrl.h"

static IOMAPIOCTRL IOMapIOCtrl;
static VARSIOCTRL  VarsIOCtrl;
static HEADER      **pHeaders;

const HEADER cIOCtrl = {
    0x00060001L,
    "IOCtrl",
    cIOCtrlInit,
    cIOCtrlCtrl,
    cIOCtrlExit,
    (void *) &IOMapIOCtrl,
    (void *) &VarsIOCtrl,
    (UWORD) sizeof(IOMapIOCtrl),
    (UWORD) sizeof(VarsIOCtrl),
    0x0000                      //Code size - not used so far
};


void cIOCtrlInit(void *pHeader) {
    pHeaders = pHeader;
}


void cIOCtrlCtrl(void) {
    switch (IOMapIOCtrl.PowerOn) {
    case POWERDOWN:
    case BOOT:
        pMapCmd->Awake = FALSE;
        break;
    default:
        break;
    }
}


void cIOCtrlExit(void) {}

