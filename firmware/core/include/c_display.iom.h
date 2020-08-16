//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_display.iom                                 $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_disp $
//
// Platform        C
//

#ifndef   CDISPLAY_IOM
#define   CDISPLAY_IOM

#define   pMapDisplay ((IOMAPDISPLAY*)(pHeaders[ENTRY_DISPLAY]->pIOMap))

// Constants related to simple draw entry (x = dont care)
enum
{
  DISPLAY_ERASE_ALL         = 0x00,     // W - erase entire screen     (CMD,x,x,x,x,x)
  DISPLAY_PIXEL             = 0x01,     // W - set pixel (on/off)      (CMD,TRUE/FALSE,X,Y,x,x)
  DISPLAY_HORIZONTAL_LINE   = 0x02,     // W - draw horisontal line (on/off) (CMD,TRUE/FALSE,X1,Y1,X2,x)
  DISPLAY_VERTICAL_LINE     = 0x03,     // W - draw vertical line (on/off) (CMD,TRUE/FALSE,X1,Y1,x,Y2)
  DISPLAY_CHAR              = 0x04,     // W - draw char (actual font) (CMD,TRUE/FALSE,X1,Y1,Char,x)
  DISPLAY_ERASE_LINE        = 0x05,     // W - erase a single line     (CMD,x,LINE,x,x,x)
  DISPLAY_FILL_REGION       = 0x06,     // W - fill screen region    (CMD,TRUE/FALSE,X1,Y1,X2,Y2)
  DISPLAY_FRAME             = 0x07      // W - draw a frame (on/off) (CMD,TRUE/FALSE,X1,Y1,X2,Y2)
};

//JJR
// Constants related to drawing operations.

enum
{
  DRAW_PIXELS_SET       = 0x00,     //Basic options for pixel, line and shape drawing.
  DRAW_PIXELS_CLEAR     = 0x01,
  DRAW_PIXELS_INVERT    = 0x02
};

enum
{
  DRAW_SHAPE_HOLLOW     = 0x00,     //Extra options for shape drawing.
  DRAW_SHAPE_FILLED     = 0x01
};

enum
{
  DRAW_BITMAP_PLAIN      = 0x00,
  DRAW_BITMAP_INVERT     = 0x01
};

enum
{
  DRAW_LOGICAL_COPY     = 0x00,
  DRAW_LOGICAL_AND      = 0x01,
  DRAW_LOGICAL_OR       = 0x02,
  DRAW_LOGICAL_XOR      = 0x03
};
//JJR

enum
{
  DRAW_FONT_WRAP_OFF = 0x00,
  DRAW_FONT_WRAP_ON  = 0x01
};

enum
{
  DRAW_FONT_DIR_L2RB = 0x00,
  DRAW_FONT_DIR_L2RT = 0x01,
  DRAW_FONT_DIR_R2LB = 0x02,
  DRAW_FONT_DIR_R2LT = 0x03,
  DRAW_FONT_DIR_B2TL = 0x04,
  DRAW_FONT_DIR_B2TR = 0x05,
  DRAW_FONT_DIR_T2BL = 0x06,
  DRAW_FONT_DIR_T2BR = 0x07
};

// Constants related to Flags
enum
{
  DISPLAY_ON                = 0x01,     // W  - Display on
  DISPLAY_REFRESH           = 0x02,     // W  - Enable refresh
  DISPLAY_POPUP             = 0x08,     // W  - Use popup display memory
  DISPLAY_REFRESH_DISABLED  = 0x40,     // R  - Refresh disabled
  DISPLAY_BUSY              = 0x80      // R  - Refresh in progress
};

#define   DISPLAY_HEIGHT      64        // Y pixels
#define   DISPLAY_WIDTH       100       // X pixels
#define   DISPLAY_BUFF_WIDTH  100       // width of buffer

#define   DISPLAY_MENUICONS_Y       40
#define   DISPLAY_MENUICONS_X_OFFS  7
#define   DISPLAY_MENUICONS_X_DIFF  31

#define   DISPLAY_IDLE                  ((pMapDisplay->EraseMask == 0) && (pMapDisplay->UpdateMask == 0))

#define   DISPLAY_CONTRAST_DEFAULT 0x5A
#define   DISPLAY_CONTRAST_MAX     0x7F

enum      TEXTLINE_NO                   // Used in macro "TEXTLINE_BIT"
{
  TEXTLINE_1,                           // Upper most line
  TEXTLINE_2,                           //
  TEXTLINE_3,                           //
  TEXTLINE_4,                           //
  TEXTLINE_5,                           //
  TEXTLINE_6,                           //
  TEXTLINE_7,                           //
  TEXTLINE_8,                           // Buttom line
  TEXTLINES
};

enum      MENUICON_NO                   // Used in macro "MENUICON_BIT"
{
  MENUICON_LEFT,                        // Left icon
  MENUICON_CENTER,                      // Center icon
  MENUICON_RIGHT,                       // Right icon
  MENUICONS
};

enum      SPECIAL_NO                    // Used in macro "SPECIAL_BIT"
{
  FRAME_SELECT,                         // Center icon select frame
  STATUSTEXT,                           // Status text (BT name)
  MENUTEXT,                             // Center icon text
  STEPLINE,                             // Step collection lines
  TOPLINE,                              // Top status underline
  SPECIALS
};

enum      STATUSICON_NO                 // Used in macro "STATUSICON_BIT"
{
  STATUSICON_BLUETOOTH,                 // BlueTooth status icon collection
  STATUSICON_USB,                       // USB status icon collection
  STATUSICON_VM,                        // VM status icon collection
  STATUSICON_BATTERY,                   // Battery status icon collection
  STATUSICONS
};

enum      SCREEN_NO                     // Used in macro "SCREEN_BIT"
{
  SCREEN_BACKGROUND,                    // Entire screen
  SCREEN_LARGE,                         // Entire screen except status line
  SCREEN_SMALL,                         // Screen between menu icons and status line
  SCREENS
};

enum      BITMAP_NO                     // Used in macro "BITMAP_BIT"
{
  BITMAP_1,                             // Bitmap 1
  BITMAP_2,                             // Bitmap 2
  BITMAP_3,                             // Bitmap 3
  BITMAP_4,                             // Bitmap 4
  BITMAPS
};

enum      STEP_NO                       // Used in macro "STEPICON_BIT"
{
  STEPICON_1,                           // Left most step icon
  STEPICON_2,                           //
  STEPICON_3,                           //
  STEPICON_4,                           //
  STEPICON_5,                           // Right most step icon
  STEPICONS
};

#define   SCREEN_BITS                   ((ULONG)0xE0000000)  // Executed as 1.
#define   STEPICON_BITS                 ((ULONG)0x1F000000)  // Executed as 2.
#define   BITMAP_BITS                   ((ULONG)0x00F00000)  // Executed as 3.
#define   MENUICON_BITS                 ((ULONG)0x000E0000)  // Executed as 4.
#define   STATUSICON_BITS               ((ULONG)0x0001E000)  // Executed as 5.
#define   SPECIAL_BITS                  ((ULONG)0x00001F00)  // Executed as 6.
#define   TEXTLINE_BITS                 ((ULONG)0x000000FF)  // Executed as 7.

#define   SCREEN_BIT(No)                ((ULONG)0x20000000 << (No))
#define   STEPICON_BIT(No)              ((ULONG)0x01000000 << (No))
#define   BITMAP_BIT(No)                ((ULONG)0x00100000 << (No))
#define   MENUICON_BIT(No)              ((ULONG)0x00020000 << (No))
#define   STATUSICON_BIT(No)            ((ULONG)0x00002000 << (No))
#define   SPECIAL_BIT(No)               ((ULONG)0x00000100 << (No))
#define   TEXTLINE_BIT(No)              ((ULONG)0x00000001 << (No))


typedef   struct
{
  void    (*pFunc)(UBYTE,UBYTE,UBYTE,UBYTE,UBYTE,UBYTE);    // Simple draw entry

  ULONG   EraseMask;                                        // Section erase mask   (executed first)
  ULONG   UpdateMask;                                       // Section update mask  (executed next)

  const FONT    *pFont;                                     // Pointer to font file
  const UBYTE   *pTextLines[TEXTLINES];                     // Pointer to text strings

  const UBYTE   *pStatusText;                               // Pointer to status text string
  const ICON    *pStatusIcons;                              // Pointer to status icon collection file

  const BMPMAP  *pScreens[SCREENS];                         // Pointer to screen bitmap file
  const BMPMAP  *pBitmaps[BITMAPS];                         // Pointer to free bitmap files

  const UBYTE   *pMenuText;                                 // Pointer to menu icon text                (NULL == none)
  const UBYTE   *pMenuIcons[MENUICONS];                     // Pointer to menu icon images              (NULL == none)

  const ICON    *pStepIcons;                                // Pointer to step icon collection file

  UBYTE   *Display;                                         // Display content copied to physical display every 17 mS

  UBYTE   StatusIcons[STATUSICONS];                         // Index in status icon collection file     (index = 0 -> none)

  UBYTE   StepIcons[STEPICONS];                             // Index in step icon collection file       (index = 0 -> none)

  UBYTE   Flags;                                            // Update flags enumerated above

  UBYTE   TextLinesCenterFlags;                             // Mask to center TextLines

  UBYTE   Normal[DISPLAY_HEIGHT / 8][DISPLAY_BUFF_WIDTH];   // Raw display memory for normal screen
  UBYTE   Popup[DISPLAY_HEIGHT / 8][DISPLAY_BUFF_WIDTH];    // Raw display memory for popup screen

  UBYTE   Contrast;                                         // Display contrast
  UBYTE   Scaling;                                          // Scaling to EV3 display size
}
IOMAPDISPLAY;

#endif



