//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author: Dkflebun $
//
// Revision date   $Date: 9-06-08 13:35 $
//
// Filename        $Workfile:: c_display.c                                   $
//
// Version         $Revision: 2 $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_disp $
//
// Platform        C
//

#include  <string.h>
#include  <stdint.h>
#include  "stdconst.h"
#include  "modules.h"
#include  "c_display.iom.h"
#include  "c_display.h"
#include  "hal_general.h"
#include  "hal_display.h"


static    IOMAPDISPLAY  IOMapDisplay;
static    VARSDISPLAY   VarsDisplay;

const     HEADER  cDisplay =
{
  0x000A0001L,
  "Display",
  cDisplayInit,
  cDisplayCtrl,
  cDisplayExit,
  (void *)&IOMapDisplay,
  (void *)&VarsDisplay,
  (UWORD)sizeof(IOMapDisplay),
  (UWORD)sizeof(VarsDisplay),
  0x0000                                    //Code size - not used so far
};


const     SCREEN_CORDINATE SCREEN_CORDINATES[SCREENS] =
{
  {  0, 0,DISPLAY_WIDTH,DISPLAY_HEIGHT },     // Background
  {  0, 8,DISPLAY_WIDTH,DISPLAY_HEIGHT - 8 }, // Large
  {  0, 8,DISPLAY_WIDTH,24 }                  // Small
};

const     SCREEN_CORDINATE SELECT_FRAME_CORDINATES =
{
  38,41,24,24
};


const     SCREEN_CORDINATE MENUICON_CORDINATES[MENUICONS] =
{
  { DISPLAY_MENUICONS_X_OFFS,DISPLAY_MENUICONS_Y,24,24 },                               // Left
  { DISPLAY_MENUICONS_X_OFFS + DISPLAY_MENUICONS_X_DIFF,DISPLAY_MENUICONS_Y,24,24 },    // Center
  { DISPLAY_MENUICONS_X_OFFS + DISPLAY_MENUICONS_X_DIFF * 2,DISPLAY_MENUICONS_Y,24,24 },// Right
};

const     SCREEN_CORDINATE STATUSICON_CORDINATES[STATUSICONS] =
{
  {  0, 0,12, 8 }, // Bluetooth
  { 12, 0,12, 8 }, // Usb
  { 76, 0,12, 8 }, // Vm
  { 88, 0,12, 8 }  // Battery
};


const     SCREEN_CORDINATE STEPICON_CORDINATES[STEPICONS] =
{
  { 11,16,11,16 }, // Step 1
  { 28,16,11,16 }, // Step 2
  { 45,16,11,16 }, // Step 3
  { 62,16,11,16 }, // Step 4
  { 79,16,11,16 }  // Step 5
};


void      cDisplaySetPixel(UBYTE X,UBYTE Y)
{
  if ((X < DISPLAY_WIDTH) && (Y < DISPLAY_HEIGHT))
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] |= (1 << (Y % 8));
  }
}


void      cDisplayClrPixel(UBYTE X,UBYTE Y)
{
  if ((X < DISPLAY_WIDTH) && (Y < DISPLAY_HEIGHT))
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] &= ~(1 << (Y % 8));
  }
}

void      cDisplayXorPixel(UBYTE X,UBYTE Y)
{
  if ((X < DISPLAY_WIDTH) && (Y < DISPLAY_HEIGHT))
  {
    IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] ^= (1 << (Y % 8));
  }
}


void      cDisplayChar(const FONT *pFont,UBYTE On,UBYTE X,UBYTE Y,UBYTE Char)
{
  UBYTE   *pSource;
  UBYTE   FontWidth;
  UBYTE   FontHeight;
  UBYTE   Items;
  UBYTE   Item;
  UBYTE   TmpY;


  Items          = pFont->ItemsX * pFont->ItemsY;
  Item           = Char - ' ';
  if (Item < Items)
  {
    FontWidth    = pFont->ItemPixelsX;
    pSource      = (UBYTE*)&pFont->Data[Item * FontWidth];
    while (FontWidth--)
    {
      TmpY       = 0;
      FontHeight = pFont->ItemPixelsY;
      while (FontHeight--)
      {
        if (On == TRUE)
        {
          if (((*pSource) & (1 << TmpY)))
          {
            cDisplaySetPixel(X,Y + TmpY);
          }
          else
          {
            cDisplayClrPixel(X,Y + TmpY);
          }
        }
        else
        {
          if (((*pSource) & (1 << TmpY)))
          {
            cDisplayClrPixel(X,Y + TmpY);
          }
          else
          {
            cDisplaySetPixel(X,Y + TmpY);
          }
        }
        TmpY++;
      }
      X++;
      pSource++;
    }
  }
}


void      cDisplayString(const FONT *pFont,UBYTE X,UBYTE Y,const UBYTE *pString)
{
  UBYTE   *pSource;
  UBYTE   *pDestination;
  UBYTE   FontWidth;
  UBYTE   Line;
  UBYTE   Items;
  UBYTE   Item;


  Line         = (Y & 0xF8) / 8;
  Items        = pFont->ItemsX * pFont->ItemsY;
  pDestination = (UBYTE*)&IOMapDisplay.Display[Line * DISPLAY_WIDTH + X];

  while (*pString)
  {
    Item           = *pString - ' ';
    if (Item < Items)
    {
      FontWidth    = pFont->ItemPixelsX;
      pSource      = (UBYTE*)&pFont->Data[Item * FontWidth];
      while (FontWidth--)
      {
        *pDestination = *pSource;
        pDestination++;
        pSource++;
      }
    }
    pString++;
  }
}


void      cDisplayUpdateScreen(const SCREEN_CORDINATE *pCord,const BMPMAP *pBitmap)
{
  const UBYTE   *pSource;
  UBYTE   *pDestination;
  UBYTE   Line;
  UBYTE   Lines;

  if (pBitmap)
  {
    if ((((pBitmap->StartY + pCord->StartY) & 0x07) == 0) && ((pBitmap->PixelsY & 0x07) == 0))
    {
      pSource = pBitmap->Data;
      Line    = (pBitmap->StartY + pCord->StartY) / 8;
      Lines   = Line + pBitmap->PixelsY / 8;
      while (Line < Lines)
      {
        pDestination = &IOMapDisplay.Display[Line * DISPLAY_WIDTH + pBitmap->StartX + pCord->StartX];
        memcpy(pDestination,pSource,(size_t)pBitmap->PixelsX);
        pSource += pBitmap->PixelsX;
        Line++;
      }
    }
  }
}


void      cDisplayCenterString(const FONT *pFont,const UBYTE *pString,UBYTE Line)
{
  UWORD   Chars;
  UBYTE   Column;

  if (pString)
  {
    Chars = 0;
    while (pString[Chars])
    {
      Chars++;
    }
    Column = (DISPLAY_WIDTH - Chars * pFont->ItemPixelsX) / 2;
    cDisplayString(pFont,Column,Line * 8,pString);
  }
}


void      cDisplayUpdateMenuIcon(const UBYTE *pIcon,const SCREEN_CORDINATE *pCord)
{
  UBYTE   *pDestination;
  UBYTE   Line;
  UBYTE   Column;
  UBYTE   Lines;
  UBYTE   Columns;

  if (((pCord->StartY & 0x07) == 0) && ((pCord->PixelsY & 0x07) == 0))
  {
    Line    = pCord->StartY / 8;
    Lines   = Line + pCord->PixelsY / 8;
    Columns = pCord->StartX + pCord->PixelsX;
    if (pIcon != NULL)
    {
      while (Line < Lines)
      {
        Column   = pCord->StartX;
        pDestination = &IOMapDisplay.Display[Line * DISPLAY_WIDTH + Column];

        while (Column < Columns)
        {
          *pDestination |= *pIcon;
          pIcon++;
          pDestination++;
          Column++;
        }
        Line++;
      }
    }
    else
    {
      while (Line < Lines)
      {
        pDestination = &IOMapDisplay.Display[Line * DISPLAY_WIDTH + pCord->StartX];
        memset(pDestination,0,(size_t)pCord->PixelsX);
        Line++;
      }
    }
  }
}


void      cDisplayUpdateIcon(const ICON *pIcons,UBYTE Index,const SCREEN_CORDINATE *pCord)
{
  const UBYTE   *pSource;
  UBYTE   *pDestination;
  UBYTE   Line;
  UBYTE   Lines;

  if (pIcons)
  {
    if ((Index > 0) && (Index <= (pIcons->ItemsX * pIcons->ItemsY)))
    {
      Index--;
      if (((pCord->StartY & 0x07) == 0) && ((pCord->PixelsY & 0x07) == 0))
      {
        Line    = pCord->StartY / 8;
        Lines   = Line + pCord->PixelsY / 8;
        pSource = &pIcons->Data[((Index / pIcons->ItemsX) * pIcons->ItemsX * pIcons->ItemPixelsX * pIcons->ItemPixelsY / 8) + ((Index % pIcons->ItemsX) * pIcons->ItemPixelsX)];
        while (Line < Lines)
        {
          pDestination = &IOMapDisplay.Display[Line * DISPLAY_WIDTH + pCord->StartX];
          memcpy(pDestination,pSource,(size_t)pCord->PixelsX);
          pSource += (pIcons->ItemPixelsX * pIcons->ItemsX);
          Line++;
        }
      }
    }
    else
    {
      if (((pCord->StartY & 0x07) == 0) && ((pCord->PixelsY & 0x07) == 0))
      {
        Line    = pCord->StartY / 8;
        Lines   = Line + pCord->PixelsY / 8;
        while (Line < Lines)
        {
          pDestination = &IOMapDisplay.Display[Line * DISPLAY_WIDTH + pCord->StartX];
          memset(pDestination,0,(size_t)pCord->PixelsX);
          Line++;
        }
      }
    }
  }
}


void cDisplayLineX(UBYTE X1, UBYTE X2, UBYTE Y, UBYTE PixelMode)
{
  UBYTE   X;
  UBYTE   M;
  UBYTE   t;

  if (Y > DISPLAY_HEIGHT) return;
  if (X1 > X2) {t = X1; X1 = X2; X2 = t;}
  if (X2 > (DISPLAY_WIDTH-1)) X2 = (DISPLAY_WIDTH-1);

  M   = 1 << (Y % 8);
  Y >>= 3;
  for (X = X1;X <= X2;X++)
  {
    switch (PixelMode)
    {
      case DRAW_PIXELS_INVERT:
        IOMapDisplay.Display[Y * DISPLAY_WIDTH + X] ^= M;
        break;
      case DRAW_PIXELS_CLEAR:
        IOMapDisplay.Display[Y * DISPLAY_WIDTH + X] &= ~M;
        break;
      case DRAW_PIXELS_SET:
      default:
        IOMapDisplay.Display[Y * DISPLAY_WIDTH + X] |= M;
        break;
    }
  }
}

static UBYTE Masks[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

void      cDisplayLineY(UBYTE X,UBYTE Y1,UBYTE Y2,UBYTE PixelMode)
{
  UBYTE   Y;
  UBYTE   M;
  UBYTE   t;

  if (X > DISPLAY_WIDTH) return;
  if (Y1 > Y2) {t = Y1; Y1 = Y2; Y2 = t;}
  if (Y2 > (DISPLAY_HEIGHT-1)) Y2 = (DISPLAY_HEIGHT-1);

  // starting point of Y is the byte containing Y1
  Y = (Y1 / 8) * 8;

  while (Y <= Y2)
  {
    M = 0xff;
    if (Y < Y1)
      M &= ~Masks[Y1 % 8];
    if ((Y2-Y) < 8)
      M &= Masks[(Y2 % 8) + 1];
    switch (PixelMode)
    {
      case DRAW_PIXELS_INVERT:
        IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] ^= M;
        break;
      case DRAW_PIXELS_CLEAR:
        IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] &= ~M;
        break;
      case DRAW_PIXELS_SET:
      default:
        IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] |= M;
        break;
    }
    Y += 8;
  }

}

void cDisplayFrame(SCREEN_CORDINATE *pCord, UBYTE PixelMode)
{
  cDisplayLineX(pCord->StartX, pCord->StartX + pCord->PixelsX-1, pCord->StartY, PixelMode);
  if (pCord->PixelsY > 1)
  {
    cDisplayLineY(pCord->StartX, pCord->StartY + 1, pCord->StartY + pCord->PixelsY - 1, PixelMode);
    cDisplayLineY(pCord->StartX + pCord->PixelsX - 1, pCord->StartY + 1, pCord->StartY + pCord->PixelsY - 1, PixelMode);
  }
}


void      cDisplayEraseLine(UBYTE Line)
{
  memset(&IOMapDisplay.Display[Line * DISPLAY_WIDTH], 0x00, DISPLAY_WIDTH);
}


void      cDisplayErase(void)
{
  memset(&IOMapDisplay.Display[0], 0x00, DISPLAY_WIDTH*DISPLAY_HEIGHT/8);
}

void      cDisplayFillScreen(SCREEN_CORDINATE *pCord, UBYTE PixelMode)
{
  UBYTE   X1, Y1;
  UBYTE   X2, Y2;
  UBYTE   X, Y;
  UBYTE   M;

  X1 = pCord->StartX;
  Y1 = pCord->StartY;
  X2 = pCord->StartX + pCord->PixelsX - 1;
  Y2 = pCord->StartY + pCord->PixelsY - 1;

  if (X2 > (DISPLAY_WIDTH-1)) X2 = (DISPLAY_WIDTH-1);
  if (Y2 > (DISPLAY_HEIGHT-1)) Y2 = (DISPLAY_HEIGHT-1);

  Y = (Y1 / 8) * 8;

  while (Y <= Y2)
  {
    M = 0xff;
    if (Y < Y1)
      M &= ~Masks[Y1 % 8];
    if ((Y2-Y) < 8)
      M &= Masks[(Y2 % 8) + 1];
    switch (PixelMode)
    {
      case DRAW_PIXELS_INVERT:
        for (X=X1; X<=X2; X++)
          IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] ^= M;
        break;
      case DRAW_PIXELS_CLEAR:
        for (X=X1; X<=X2; X++)
          IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] &= ~M;
        break;
      case DRAW_PIXELS_SET:
      default:
        for (X=X1; X<=X2; X++)
          IOMapDisplay.Display[(Y / 8) * DISPLAY_WIDTH + X] |= M;
        break;
    }
    Y += 8;
  }
}


void cDisplayDraw(UBYTE Cmd,UBYTE PixelMode,UBYTE X1,UBYTE Y1,UBYTE X2,UBYTE Y2)
{
  SCREEN_CORDINATE Coord;

  switch (Cmd)
  {
    case DISPLAY_ERASE_ALL :
    {
      cDisplayErase();
    }
    break;

    case DISPLAY_PIXEL :
    {
      switch (PixelMode)
      {
        case DRAW_PIXELS_INVERT:
          cDisplayXorPixel(X1,Y1);
          break;
        case DRAW_PIXELS_CLEAR:
          cDisplayClrPixel(X1,Y1);
          break;
        case DRAW_PIXELS_SET:
        default:
          cDisplaySetPixel(X1,Y1);
          break;
      }
    }
    break;

    case DISPLAY_HORIZONTAL_LINE :
    {
        cDisplayLineX(X1,X2,Y1,PixelMode);
    }
    break;

    case DISPLAY_VERTICAL_LINE :
    {
        cDisplayLineY(X1,Y1,Y2,PixelMode);
    }
    break;

    case DISPLAY_CHAR :
    {
      cDisplayChar(IOMapDisplay.pFont,PixelMode,X1,Y1,X2);
    }
    break;

    case DISPLAY_ERASE_LINE :
    {
      cDisplayEraseLine(X1);
    }
    break;

    case DISPLAY_FILL_REGION :
    {
      Coord.StartX = X1;
      Coord.StartY = Y1;
      Coord.PixelsX = X2;
      Coord.PixelsY = Y2;
      cDisplayFillScreen(&Coord, PixelMode);
    }
    break;

    case DISPLAY_FRAME :
    {
      Coord.StartX = X1;
      Coord.StartY = Y1;
      Coord.PixelsX = X2;
      Coord.PixelsY = Y2;
      cDisplayFrame(&Coord, PixelMode);
    }
    break;

  }
}


void      cDisplayInit(void* pHeader)
{
  if (!Hal_Display_RefAdd())
    return Hal_General_AbnormalExit("Cannot initialize display output");
  IOMapDisplay.Display              =  (UBYTE*)IOMapDisplay.Normal;
  IOMapDisplay.pFunc                =  &cDisplayDraw;
  IOMapDisplay.EraseMask            =  0;
  IOMapDisplay.UpdateMask           =  0;
  IOMapDisplay.TextLinesCenterFlags =  0;
  IOMapDisplay.Flags                =  DISPLAY_REFRESH | DISPLAY_ON;
  IOMapDisplay.Contrast             =  0x5A; // 90
  IOMapDisplay.Scaling              =  DISPLAY_SCALE_CENTER;
  VarsDisplay.ErasePointer          =  0;
  VarsDisplay.UpdatePointer =   0;
  VarsDisplay.RefreshStage  =   0;
  VarsDisplay.RefreshMillis = 100;
}


void      cDisplayCtrl(void)
{
  ULONG   TmpMask;
  UBYTE   Tmp;
  SCREEN_CORDINATE Cordinate;

  if (!(IOMapDisplay.Flags & DISPLAY_POPUP))
  {
    if (IOMapDisplay.Display == (UBYTE*)IOMapDisplay.Popup)
    {
      IOMapDisplay.Display    = VarsDisplay.DisplaySave;
    }
  }
  else
  {
    if (IOMapDisplay.Display != (UBYTE*)IOMapDisplay.Popup)
    {
      VarsDisplay.DisplaySave = IOMapDisplay.Display;
      IOMapDisplay.Display    = (UBYTE*)IOMapDisplay.Popup;
    }
  }

  if (IOMapDisplay.EraseMask)
  {

    VarsDisplay.ErasePointer = 31;
    while ((VarsDisplay.ErasePointer) && (!(IOMapDisplay.EraseMask & (0x00000001 << VarsDisplay.ErasePointer))))
    {
      VarsDisplay.ErasePointer--;
    }

    TmpMask = IOMapDisplay.EraseMask & (1 << VarsDisplay.ErasePointer);
    if ((TmpMask & TEXTLINE_BITS))
    {
      Tmp = 0;
      while (!(TmpMask & TEXTLINE_BIT(Tmp)))
      {
        Tmp++;
      }
      if (Tmp < TEXTLINES)
      {
        cDisplayEraseLine(Tmp);
      }
    }
    else
    {
      if ((TmpMask & MENUICON_BITS))
      {
        Tmp = 0;
        while (!(TmpMask & MENUICON_BIT(Tmp)))
        {
          Tmp++;
        }
        if (Tmp < MENUICONS)
        {
          cDisplayFillScreen((SCREEN_CORDINATE*)&MENUICON_CORDINATES[Tmp], DRAW_PIXELS_CLEAR);
        }
      }
      else
      {
        if ((TmpMask & STATUSICON_BITS))
        {
          Tmp = 0;
          while (!(TmpMask & STATUSICON_BIT(Tmp)))
          {
            Tmp++;
          }
          if (Tmp < STATUSICONS)
          {
            cDisplayFillScreen((SCREEN_CORDINATE*)&STATUSICON_CORDINATES[Tmp], DRAW_PIXELS_CLEAR);
          }
        }
        else
        {
          if ((TmpMask & SCREEN_BITS))
          {
            Tmp = 0;
            while (!(TmpMask & SCREEN_BIT(Tmp)))
            {
              Tmp++;
            }
            if (Tmp < SCREENS)
            {
              cDisplayFillScreen((SCREEN_CORDINATE*)&SCREEN_CORDINATES[Tmp], DRAW_PIXELS_CLEAR);
            }
            if ((TmpMask & SCREEN_BIT(SCREEN_LARGE)))
            {
              if ((IOMapDisplay.UpdateMask & SPECIAL_BIT(TOPLINE)))
              {
                cDisplayLineX(0,DISPLAY_WIDTH - 1, 9, DRAW_PIXELS_SET);
                IOMapDisplay.UpdateMask &= ~SPECIAL_BIT(TOPLINE);
              }
            }
          }
          else
          {
            if ((TmpMask & BITMAP_BITS))
            {
              Tmp = 0;
              while (!(TmpMask & BITMAP_BIT(Tmp)))
              {
                Tmp++;
              }
              if (Tmp < BITMAPS)
              {
                Cordinate.StartX  = VarsDisplay.pOldBitmaps[Tmp]->StartX;
                Cordinate.StartY  = VarsDisplay.pOldBitmaps[Tmp]->StartY;
                Cordinate.PixelsX = VarsDisplay.pOldBitmaps[Tmp]->PixelsX;
                Cordinate.PixelsY = VarsDisplay.pOldBitmaps[Tmp]->PixelsY;
                cDisplayFillScreen(&Cordinate, DRAW_PIXELS_CLEAR);
              }
            }
            else
            {
              if ((TmpMask & SPECIAL_BITS))
              {
                Tmp = 0;
                while (!(TmpMask & SPECIAL_BIT(Tmp)))
                {
                  Tmp++;
                }
                switch (Tmp)
                {
                  case FRAME_SELECT :
                  {
                  }
                  break;

                  case MENUTEXT :
                  {
                    cDisplayEraseLine(TEXTLINE_5);
                  }
                  break;

                  case STATUSTEXT :
                  {
                    cDisplayEraseLine(TEXTLINE_1);
                  }
                  break;

                  case STEPLINE :
                  {
                  }
                  break;

                  case TOPLINE :
                  {
                  }
                  break;

                }
              }
              else
              {
                if ((TmpMask & STEPICON_BITS))
                {
                  Tmp = 0;
                  while (!(TmpMask & STEPICON_BIT(Tmp)))
                  {
                    Tmp++;
                  }
                  if (Tmp < STEPICONS)
                  {
                    cDisplayFillScreen((SCREEN_CORDINATE*)&STEPICON_CORDINATES[Tmp], DRAW_PIXELS_CLEAR);
                  }
                }
              }
            }
          }
        }
      }
    }
    IOMapDisplay.EraseMask &= ~TmpMask;

    if (++VarsDisplay.ErasePointer >= 32)
    {
      VarsDisplay.ErasePointer = 0;
    }
    VarsDisplay.UpdatePointer = 0;
  }
  else
  {
    if (IOMapDisplay.UpdateMask)
    {

      VarsDisplay.UpdatePointer = 31;
      while ((VarsDisplay.UpdatePointer) && (!(IOMapDisplay.UpdateMask & (0x00000001 << VarsDisplay.UpdatePointer))))
      {
        VarsDisplay.UpdatePointer--;
      }
      TmpMask = IOMapDisplay.UpdateMask & (0x00000001 << VarsDisplay.UpdatePointer);

      if ((TmpMask & TEXTLINE_BITS))
      {
        Tmp = 0;
        while (!(TmpMask & TEXTLINE_BIT(Tmp)))
        {
          Tmp++;
        }
        if (Tmp < TEXTLINES)
        {
          if ((IOMapDisplay.TextLinesCenterFlags & (UBYTE)TmpMask))
          {
            cDisplayCenterString(IOMapDisplay.pFont,IOMapDisplay.pTextLines[Tmp],TEXTLINE_1 + Tmp);
          }
          else
          {
            cDisplayString(IOMapDisplay.pFont,0,Tmp * 8,IOMapDisplay.pTextLines[Tmp]);
          }
        }
      }
      else
      {
        if ((TmpMask & MENUICON_BITS))
        {
          Tmp = 0;
          while (!(TmpMask & MENUICON_BIT(Tmp)))
          {
            Tmp++;
          }
          if (Tmp < MENUICONS)
          {
            cDisplayUpdateMenuIcon(IOMapDisplay.pMenuIcons[Tmp],(SCREEN_CORDINATE*)&MENUICON_CORDINATES[Tmp]);
          }
        }
        else
        {
          if ((TmpMask & STATUSICON_BITS))
          {
            Tmp = 0;
            while (!(TmpMask & STATUSICON_BIT(Tmp)))
            {
              Tmp++;
            }
            if (Tmp < STATUSICONS)
            {
              cDisplayUpdateIcon(IOMapDisplay.pStatusIcons,IOMapDisplay.StatusIcons[Tmp],(SCREEN_CORDINATE*)&STATUSICON_CORDINATES[Tmp]);
            }
          }
          else
          {
            if ((TmpMask & SCREEN_BITS))
            {
              Tmp = 0;
              while (!(TmpMask & SCREEN_BIT(Tmp)))
              {
                Tmp++;
              }
              if (Tmp < SCREENS)
              {
                cDisplayUpdateScreen((SCREEN_CORDINATE*)&SCREEN_CORDINATES[Tmp],IOMapDisplay.pScreens[Tmp]);
              }
            }
            else
            {
              if ((TmpMask & BITMAP_BITS))
              {
                Tmp = 0;
                while (!(TmpMask & BITMAP_BIT(Tmp)))
                {
                  Tmp++;
                }
                if (Tmp < BITMAPS)
                {
                  VarsDisplay.pOldBitmaps[Tmp] = IOMapDisplay.pBitmaps[Tmp];
                  cDisplayUpdateScreen((SCREEN_CORDINATE*)&SCREEN_CORDINATES[SCREEN_BACKGROUND],IOMapDisplay.pBitmaps[Tmp]);
                }
              }
              else
              {
                if ((TmpMask & SPECIAL_BITS))
                {
                  Tmp = 0;
                  while (!(TmpMask & SPECIAL_BIT(Tmp)))
                  {
                    Tmp++;
                  }
                  switch (Tmp)
                  {
                    case FRAME_SELECT :
                    {
                      cDisplayFrame((SCREEN_CORDINATE*)&SELECT_FRAME_CORDINATES,DRAW_PIXELS_SET);
                    }
                    break;

                    case MENUTEXT :
                    {
                      cDisplayCenterString(IOMapDisplay.pFont,IOMapDisplay.pMenuText,TEXTLINE_5);
                    }
                    break;

                    case STATUSTEXT :
                    {
                      cDisplayCenterString(IOMapDisplay.pFont,IOMapDisplay.pStatusText,TEXTLINE_1);
                    }
                    break;

                    case STEPLINE :
                    {
                      cDisplayLineX(22,28,20,DRAW_PIXELS_SET);
                      cDisplayLineX(39,45,20,DRAW_PIXELS_SET);
                      cDisplayLineX(56,62,20,DRAW_PIXELS_SET);
                      cDisplayLineX(73,79,20,DRAW_PIXELS_SET);
                    }
                    break;

                    case TOPLINE :
                    {
                      cDisplayLineX(0,DISPLAY_WIDTH - 1,9,DRAW_PIXELS_SET);
                    }
                    break;

                  }
                }
                else
                {
                  if ((TmpMask & STEPICON_BITS))
                  {
                    Tmp = 0;
                    while (!(TmpMask & STEPICON_BIT(Tmp)))
                    {
                      Tmp++;
                    }
                    if (Tmp < STEPICONS)
                    {
                      cDisplayUpdateIcon(IOMapDisplay.pStepIcons,IOMapDisplay.StepIcons[Tmp],(SCREEN_CORDINATE*)&STEPICON_CORDINATES[Tmp]);
                    }
                  }
                }
              }
            }
          }
        }
      }
      IOMapDisplay.TextLinesCenterFlags &= (UBYTE)(~TmpMask);
      IOMapDisplay.UpdateMask &= ~TmpMask;
      if (++VarsDisplay.UpdatePointer >= 32)
      {
        VarsDisplay.UpdatePointer = 0;
      }
    }
    VarsDisplay.ErasePointer = 0;
  }

  if (!(IOMapDisplay.Flags & DISPLAY_POPUP))
  {
    if (!(IOMapDisplay.Flags & DISPLAY_REFRESH_DISABLED))
    {
      Hal_Display_SetPower(IOMapDisplay.Flags & DISPLAY_ON);
      Hal_Display_SetContrast(IOMapDisplay.Contrast);
      Hal_Display_SetScaling(IOMapDisplay.Scaling);

      if (VarsDisplay.RefreshStage == 0) {
          VarsDisplay.LastRefresh = Hal_Timer_Now();
          if (!Hal_Display_Write((const uint8_t *) IOMapDisplay.Normal)) {
              Hal_General_AbnormalExit("Cannot refresh display");
          }
          IOMapDisplay.Flags |=  DISPLAY_BUSY;
          VarsDisplay.RefreshStage++;

      } else {
          if (Hal_Timer_MillisSince(VarsDisplay.LastRefresh) < VarsDisplay.RefreshMillis) {
              IOMapDisplay.Flags |=  DISPLAY_BUSY;

          } else {
              IOMapDisplay.Flags &= ~DISPLAY_BUSY;
              if (!(IOMapDisplay.Flags & DISPLAY_REFRESH))
                  IOMapDisplay.Flags |= DISPLAY_REFRESH_DISABLED;
              VarsDisplay.RefreshStage = 0;
          }
      }
    }
    else
    {
      if ((IOMapDisplay.Flags & DISPLAY_REFRESH))
      {
        IOMapDisplay.Flags &= ~DISPLAY_REFRESH_DISABLED;
      }
     }
    }
    else
    {
      Hal_Display_Write((const uint8_t *) IOMapDisplay.Popup);
    }
}


void      cDisplayExit(void)
{
  Hal_Display_RefDel();
}
