//
// Date init       14.12.2004
//
// Revision date   $Date: 23-04-08 11:15 $
//
// Filename        $Workfile:: d_timer.c                                     $
//
// Version         $Revision: 2 $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_time $
//
// Platform        C
//


#include "stdconst.h"
#include "m_sched.h"
#include "d_timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


static time_point TimeZero;
static ULONG      TimerMS;

void      dTimerInit(void)
{
  TimeZero = dTimerGetNow();
  TimerMS = 0;
}


time_point dTimerGetNow(void) {
  struct timespec now = {0, 0};

  int result = clock_gettime(CLOCK_MONOTONIC, &now);
  if (__builtin_expect(result < 0, 0)) {
    perror("clock_gettime(CLOCK_MONOTONIC) failed");
    abort();
  }

  return now;
}

time_delta dTimerGetDelta(const time_point *pNow, const time_point *pLast) {
  struct timespec delta;

  if (pNow->tv_nsec > pLast->tv_nsec) {
    delta.tv_sec  = pNow->tv_sec  - pLast->tv_sec;
    delta.tv_nsec = pNow->tv_nsec - pLast->tv_nsec;
  } else {
    delta.tv_sec  = pNow->tv_sec  - pLast->tv_sec - 1;
    delta.tv_nsec = 1000000000 - pLast->tv_nsec + pNow->tv_nsec;
  }

  return delta;
}

UBYTE      dTimerHasElapsedMillis(const time_point *pNow, const time_point *pLast, ULONG millis) {
  time_delta delta = dTimerGetDelta(pNow, pLast);

  SLONG  secs = millis / 1000;
  SLONG nsecs = (millis % 1000000) * 1000;

  if (delta.tv_sec < secs)
    return FALSE;
  if (delta.tv_nsec < nsecs)
    return FALSE;

  return TRUE;
}

time_delta dTimerNowGetDelta(const time_point *pLast) {
  time_point now = dTimerGetNow();
  time_delta dT  = dTimerGetDelta(&now, pLast);
  return dT;
}

UBYTE      dTimerNowHasElapsedMillis(const time_point *pLast, ULONG millis) {
  time_point now = dTimerGetNow();

  return dTimerHasElapsedMillis(&now, pLast, millis);
}

ULONG     dTimerRead(void)
{
  time_delta elapsed = dTimerNowGetDelta(&TimeZero);
  TimerMS = elapsed.tv_sec * 1000 + elapsed.tv_nsec / 1000000;
  return TimerMS;
}

ULONG     dTimerReadNoPoll(void)
{
  return TimerMS;
}

ULONG     dTimerReadHiRes(void)
{
  time_delta elapsed = dTimerNowGetDelta(&TimeZero);
  return elapsed.tv_sec * 1000000 + elapsed.tv_nsec / 1000;
}

void      dTimerExit(void)
{}

