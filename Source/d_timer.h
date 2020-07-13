//
// Date init       14.12.2004
//
// Revision date   $Date: 23-04-08 11:15 $
//
// Filename        $Workfile:: d_timer.h                                     $
//
// Version         $Revision: 2 $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_time $
//
// Platform        C
//


#ifndef   D_TIMER
#define   D_TIMER

#include <time.h>

typedef struct timespec time_point;
typedef struct timespec time_delta;

void      dTimerInit(void);
void      dTimerExit(void);

time_point dTimerGetNow(void);

time_delta dTimerGetDelta(const time_point *now, const time_point *last);
UBYTE      dTimerHasElapsedMillis(const time_point *now, const time_point *last, ULONG millis);
time_delta dTimerNowGetDelta(const time_point *last);
UBYTE      dTimerNowHasElapsedMillis(const time_point *last, ULONG millis);

ULONG     dTimerRead(void);
ULONG     dTimerReadNoPoll(void);
ULONG     dTimerReadHiRes(void);


#endif



