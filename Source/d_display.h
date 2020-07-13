//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_display.h                                   $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_disp $
//
// Platform        C
//

#ifndef   D_DISPLAY
#define   D_DISPLAY

typedef struct {
  const char *Name;
  void  (*Init)    (void *pHeaders);
  void  (*Exit)    (void);
  void  (*SetPower)(UBYTE On, UBYTE Contrast);
  UBYTE (*Update)  (UWORD Height,UWORD Width,UBYTE *pImage, UBYTE Scaling);
} DISPLAY_OUTPUT;

extern DISPLAY_OUTPUT *pDisplay;

#endif
