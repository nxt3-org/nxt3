//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_display.h                                   $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_disp $
//
// Platform        C
//

#ifndef   C_DISPLAY
#define   C_DISPLAY

#ifndef   INCLUDE_OS

#include "hal_timer.h"

typedef   struct
{
  UBYTE   StartX;
  UBYTE   StartY;
  UBYTE   PixelsX;
  UBYTE   PixelsY;
}
SCREEN_CORDINATE;

typedef   struct
{
  UBYTE   *DisplaySave;
  const BMPMAP  *pOldBitmaps[BITMAPS];
  UBYTE   ErasePointer;
  UBYTE           UpdatePointer;
  UBYTE           RefreshStage;
  hal_timestamp_t LastRefresh;
  UBYTE   RefreshMillis;
}VARSDISPLAY;


#endif


void      cDisplayInit(void* pHeader);
void      cDisplayCtrl(void);
void      cDisplayExit(void);


extern    const HEADER cDisplay;


#endif
