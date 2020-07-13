//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_button.h                                    $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_butt $
//
// Platform        C
//

#ifndef   D_BUTTON
#define   D_BUTTON


typedef struct {
  const char *Name;
  void  (*Init)(void *pHeaders, UBYTE Prescaler);
  void  (*Exit)(void);
  void  (*Read)(UBYTE *pOutMask);
} BUTTON_INPUT;

extern BUTTON_INPUT *pButton;

#endif
