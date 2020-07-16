//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_button.c                                    $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_butt $
//
// Platform        C
//

#include <hal_general.h>
#include  "stdconst.h"
#include  "modules.h"
#include  "c_button.h"
#include  "c_button.iom.h"
#include  "c_button.h"
#include  "hal_button.h"
#include  "hal_led.h"

#define   BTN_PRESCALER      2

enum
{
  LONG_TIME     = (2000/BTN_PRESCALER)
};

static    IOMAPBUTTON   IOMapButton;
static    VARSBUTTON    VarsButton;
static    UBYTE         BtnCnt;

const     HEADER        cButton =
{
  0x00040001L,
  "Button",
  cButtonInit,
  cButtonCtrl,
  cButtonExit,
  (void *)&IOMapButton,
  (void *)&VarsButton,
  (UWORD)sizeof(IOMapButton),
  (UWORD)sizeof(VarsButton),
  0x0000                      //Code size - not used so far
};


void      cButtonInit(void* pHeader)
{
  UBYTE   Tmp;

  for (Tmp = 0; Tmp < NO_OF_BTNS; Tmp++)
  {
    IOMapButton.State[Tmp]                = 0;
    IOMapButton.BtnCnt[Tmp].PressedCnt    = 0;
    IOMapButton.BtnCnt[Tmp].LongPressCnt  = 0;
    IOMapButton.BtnCnt[Tmp].ShortRelCnt   = 0;
    IOMapButton.BtnCnt[Tmp].LongRelCnt    = 0;
    VarsButton.Cnt[Tmp]                   = 0;
  }
  VarsButton.OldState = 0;
  BtnCnt              = 0;
  if (!Hal_Button_RefAdd())
      Hal_General_AbnormalExit("Cannot initialize buttons");
  if (!Hal_Led_RefAdd())
      Hal_General_AbnormalExit("Cannot initialize LEDs");
}

void      cButtonCtrl(void)
{
  uint32_t ButtonState;
  UBYTE Tmp, ButtonNo;

  for (Tmp = 0; Tmp < NO_OF_BTNS; Tmp++)
  {
    IOMapButton.State[Tmp] &= ~PRESSED_EV;
  }
  if (++BtnCnt >= BTN_PRESCALER)
  {
    BtnCnt = 0;
    Hal_Button_Read(&ButtonState);

    ButtonNo = 0x01;
    for (Tmp = 0; Tmp < NO_OF_BTNS; Tmp++)
    {
      if (ButtonState & ButtonNo)
      {
        if (LONG_TIME >= (VarsButton.Cnt[Tmp]))
        {
          (VarsButton.Cnt[Tmp])++;
        }
        IOMapButton.State[Tmp] = PRESSED_STATE;
        if (!((VarsButton.OldState) & ButtonNo))
        {

          /* Button just pressed */
          (IOMapButton.State[Tmp]) |= PRESSED_EV;
          (IOMapButton.BtnCnt[Tmp].PressedCnt)++;
          VarsButton.Cnt[Tmp]     = 0;
        }
        else
        {
          if (LONG_TIME == VarsButton.Cnt[Tmp])
          {
            IOMapButton.State[Tmp] |= LONG_PRESSED_EV;
            (IOMapButton.BtnCnt[Tmp].LongPressCnt)++;
          }
        }
      }
      else
      {
        IOMapButton.State[Tmp] = 0x00;
        if ((VarsButton.OldState) & ButtonNo)
        {
          if (VarsButton.Cnt[Tmp] > LONG_TIME)
          {
            IOMapButton.State[Tmp] = LONG_RELEASED_EV;
            (IOMapButton.BtnCnt[Tmp].LongRelCnt)++;

          }
          else
          {
            IOMapButton.State[Tmp] = SHORT_RELEASED_EV;
            (IOMapButton.BtnCnt[Tmp].ShortRelCnt)++;
          }
        }
      }
      ButtonNo <<= 1;
      IOMapButton.BtnCnt[Tmp].RelCnt = ((IOMapButton.BtnCnt[Tmp].ShortRelCnt) + (IOMapButton.BtnCnt[Tmp].LongRelCnt));
    }
    VarsButton.OldState = ButtonState;
  }
}

void      cButtonExit(void)
{
  Hal_Led_RefDel();
  Hal_Button_RefDel();
}
