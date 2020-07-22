//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: m_sched.c                                     $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/m_sche $
//
// Platform        C
//


#define   INCLUDE_OS

#include  "stdconst.h"
#include  "modules.h"
#include  "m_sched.h"
#include  "hal_general.h"
#include  <stdlib.h>

#include  "c_comm.h"
#include  "c_input.h"
#include  "c_button.h"
#include  "c_loader.h"
#include  "c_sound.h"
#include  "c_display.h"
#include  "c_lowspeed.h"
#include  "c_output.h"
#include  "c_cmd.h"
#include  "c_cmd.iom.h"
#include  "c_ioctrl.h"
#include  "c_ui.h"
#include  "c_pnp.h"


static    const HEADER*  pModuleHeaders[] =
{
  &cComm,
  &cInput,
  &cButton,
  &cDisplay,
  &cLoader,
  &cLowSpeed,
  &cOutput,
  &cSound,
  &cIOCtrl,
  &cCmd,
  &cUi,
  &cPnp,
  0
};

IOFROMAVR IoFromAvr;

void      mSchedInit(void)
{
  UWORD   Tmp;

  Tmp = 0;
  while(pModuleHeaders[Tmp])
  {
    (*pModuleHeaders[Tmp]).cInit((void*) pModuleHeaders);
    Tmp++;
  }
}


UBYTE     mSchedCtrl(void)
{
  UWORD   Tmp;

  Tmp = 0;
  while(pModuleHeaders[Tmp])
  {
    (*pModuleHeaders[Tmp]).cCtrl();
    Tmp++;
  }

  return(((IOMAPCMD*)(pModuleHeaders[ENTRY_CMD]->pIOMap))->Awake);
}


void      mSchedExit(void)
{
  UWORD   Tmp;

  Tmp = 0;
  while(pModuleHeaders[Tmp])
  {
    (*pModuleHeaders[Tmp]).cExit();
    Tmp++;
  }
}

int main(void)
{
#ifdef STOP_LMS2012
  system("killall -STOP lms2012");
#endif
  Hal_General_SetupSignals(&((IOMAPCMD*)(pModuleHeaders[ENTRY_CMD]->pIOMap))->Awake);
  mSchedInit();
  while (TRUE == mSchedCtrl()) {}
  mSchedExit();
#ifdef STOP_LMS2012
  system("killall -CONT lms2012");
#endif
  return 0;
}
