//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dktochpe                                        $
//
// Revision date   $Date:: 10/21/08 12:08p                                   $
//
// Filename        $Workfile:: c_ui.h                                        $
//
// Version         $Revision:: 10                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_ui.h $
//
// Platform        C
//

#ifndef   C_UI
#define   C_UI

#ifndef STRIPPED
#define   DATALOGENABLED                1           // 1 == Datalog enable
#else
#define   DATALOGENABLED                0           // 0 == Datalog disabled
#endif

#define   NO_OF_FEEDBACK_CHARS          12          // Chars left when bitmap also showed
#define   SIZE_OF_CURSOR                16          // Bitmap size of cursor  (header + 8x8 pixels)
#define   SIZE_OF_PORTBITMAP            11          // Bitmap size of port no (header + 3x8 pixels)
#define   NO_OF_STATUSICONS             4           // Status icons

#define   NO_OF_INTROBITMAPS            16          // Intro bitmaps
#define   INTRO_START_TIME              1000        // Intro startup time                     [mS]
#define   INTRO_SHIFT_TIME              100         // Intro inter bitmap time                [mS]
#define   INTRO_STOP_TIME               1000        // Intro stop time                        [mS]
#define   INTRO_LOWBATT_TIME            2000        // Low battery show time at power up      [mS]

#define   MAX_VOLUME                    4           // Max volume in UI                       [cnt]

#define   CHECKBYTE                     0x78        // Used to validate NVData

#define   BUTTON_DELAY_TIME             800         // Delay before first repeat              [mS]
#define   BUTTON_REPEAT_TIME            200         // Repeat time                            [mS]

#define   RUN_BITMAP_CHANGE_TIME        125         // Running bimap update time              [mS]
#define   RUN_STATUS_CHANGE_TIME        167         // Running status update time             [mS]

#define   DISPLAY_SHOW_ERROR_TIME       2500        // Error string show time                 [mS]
#define   DISPLAY_SHOW_TIME             1500        // Min. response display time             [mS]
#define   DISPLAY_VIEW_UPDATE           200         // Display update time                    [mS]
#define   MIN_DISPLAY_UPDATE_TIME       50          // OBP min graphics update time           [mS]
#define   MIN_SENSOR_READ_TIME          100         // Time between sensor reads              [mS]

#define   ARM_WAIT_FOR_POWER_OFF        250         // Time for off command to execute        [mS]

#define   DISPLAY_SHOW_FILENAME_TIME    3000        // Datalog show saves as time             [mS]
#define   DATALOG_DEFAULT_SAMPLE_TIME   100L        // Default time between samples           [mS]

// Menu special flags

#include "MenuCommon.h"

#define   DATALOGPORTS                  (MENU_PORT_INVALID - MENU_PORT_EMPTY - 1)
#define   MAX_DATALOGS                  9999        // Highest datalog file number
#define   DATALOGBUFFERSIZE             25          // Largest number of characters buffered before flash write

#define   MENULEVELS                    10          // Max no of levels in one file (8 + 2 virtual)
#define   MENUFILELEVELS                3           // Max deept in menu file pool

typedef   struct                                    // VarsUi.MenuFiles[VarsUi.MenuFileLevel].MenuLevels[VarsUi.MenuLevel].
{
  ULONG   Id;                                       // Menu item id
  const UBYTE *IconText;                            // Menu item icon text  pointer
  ULONG   SpecialFlags;                             // Menu item special behaivor
  UBYTE   IconImageNo;                              // Menu item icon image no
  UBYTE   FunctionNo;                               // Menu item function call no   (0 = none)
  UBYTE   Parameter;                                // Menu item function call parameter
  UBYTE   NextFileNo;                               // Menu item next menu file no  (0 = none)
  UBYTE   NextMenuNo;                               // Menu item next menu no       (0 = none)

  UBYTE   ItemIndex;                                // Menu item index on level
  UBYTE   Items;                                    // Menu items on level
}
MENULEVEL;

typedef   struct
{
  MENULEVEL MenuLevels[MENULEVELS];                 // See above
  UBYTE   FileId;                                   // VarsUi.MenuFiles[VarsUi.MenuFileLevel].FileId
  UBYTE   MenuLevel;                                // VarsUi.MenuFiles[VarsUi.MenuFileLevel].MenuLevel
}
MENUFILE;

typedef   struct
{
  UBYTE   CheckByte;                                // Check byte (CHECKBYTE)
  UBYTE   DatalogEnabled;                           // Datalog enabled flag (0 = no)
  UBYTE   VolumeStep;                               // Volume step (0 - MAX_VOLUME)
  UBYTE   PowerdownCode;                            // Power down code
  UWORD   DatalogNumber;                            // Datalog file number (0 - MAX_DATALOGS)
}
NVDATA;

typedef   struct
{
  UBYTE   StatusText[STATUSTEXT_SIZE + 1];          // RCX name
  UBYTE   Initialized;                              // Ui init done
  UWORD   SleepTimer;                               // Sleep timer

  // Menu system
  MENUFILE  MenuFiles[MENUFILELEVELS];              // Menu file array
  MENUFILE  *pMenuFile;                             // Actual menu file pointer
  MENULEVEL *pMenuLevel;                            // Actual menu item on level, pointer
  const MENUITEM *pMenuItem;                        // Actual menu item in menu flash file
  UBYTE     MenuFileLevel;                          // Actual menu file level
  UBYTE   Function;                                 // Running function (0 = none)
  UBYTE   Parameter;                                // Parameter for running function
  UBYTE   SecondTime;                               // Second time flag
  UBYTE   EnterOnlyCalls;                           // Enter button only calls
  UBYTE   ExitOnlyCalls;                            // Exit button only calls
  UWORD   ButtonTimer;                              // Button repeat timer
  UWORD   ButtonTime;                               // Button repeat time
  UBYTE   ButtonOld;                                // Button old state

  // Update status
  UWORD   UpdateCounter;                            // Update counter
  UBYTE   Running;                                  // Running pointer
  UBYTE   BatteryToggle;                            // Battery flash toggle flag
  UBYTE   NewStatusIcons[NO_OF_STATUSICONS];        // New status icons (used to detect changes)

  // Low battery voltage
  const BMPMAP   *LowBattSavedBitmap;               // Low battery overwritten bitmap placeholder
  UBYTE   LowBatt;                                  // Low battery volatge flag
  UBYTE   LowBattHasOccured;                        // Low battery voltage has occured
  UBYTE   LowBattSavedState;                        // Low battery current state placeholder

  // General used variables
  const UBYTE   *MenuIconTextSave;                  // Menu icon text save

  UBYTE   *pTmp;                                    // General UBYTE pointer
  ULONG   TmpLength;                                // General filelength  (used in filelist)
  SWORD   TmpHandle;                                // General filehandle  (used in filelist)

  SWORD   Timer;                                    // General tmp purpose timer
  SWORD   ReadoutTimer;                             // General read out timer
  UBYTE   Tmp;                                      // General UBYTE
  UBYTE   FileType;                                 // General file type
  UBYTE   State;                                    // General tmp purpose state
  UBYTE   Pointer;                                  // General tmp purpose pointer
  UBYTE   Counter;                                  // General tmp purpose counter
  UBYTE   Cursor;                                   // General cursor
  UBYTE   SelectedSensor;                           // General used for selected sensor
  UBYTE   SelectedPort;                             // General used for selected port
  UBYTE   SensorReset;
  UBYTE   SensorState;                              // Sensor state (reset, ask, read)
  SWORD   SensorTimer;                              // Timer used to time sensor states
  UBYTE   NextState;

  UBYTE   SelectedFilename[FILENAME_LENGTH + 1];    // Selected file name
  UBYTE   FilenameBuffer[FILENAME_LENGTH + 1];      // General filename buffer
  UBYTE   SearchFilenameBuffer[FILENAME_LENGTH + 1];// General filename buffer
  UBYTE   DisplayBuffer[DISPLAYLINE_LENGTH + 1];    // General purpose display buffer

  UBYTE   PortBitmapLeft[SIZE_OF_PORTBITMAP];       // Port no bitmap for left icon
  UBYTE   PortBitmapCenter[SIZE_OF_PORTBITMAP];     // Port no bitmap for center icon
  UBYTE   PortBitmapRight[SIZE_OF_PORTBITMAP];      // Port no bitmap for right icon

  // Find no of files and find name for file no
  ULONG   FNOFLength;                               // Length
  SWORD   FNOFHandle;                               // Handle
  UBYTE   FNOFState;                                // State
  UBYTE   FNOFSearchBuffer[FILENAME_LENGTH + 1];    // Search buffer
  UBYTE   FNOFNameBuffer[FILENAME_LENGTH + 1];      // File name buffer
  UBYTE   FNOFFileNo;                               // File no

  // File list
  UBYTE   FileCenter;                               // File center
  UBYTE   FileLeft;                                 // File left
  UBYTE   FileRight;                                // File right
  UBYTE   NoOfFiles;                                // No of files

#ifndef STRIPPED
  // On brick programming menu
  UBYTE   ProgramSteps[ON_BRICK_PROGRAMSTEPS];      // On brick programming steps
  UBYTE   ProgramStepPointer;                       // On brick programming step pointer
  UBYTE   CursorTmp[SIZE_OF_CURSOR];                // On brick programming cursor
  UBYTE   FileHeader[FILEHEADER_LENGTH];            // File header for programs
  UBYTE   *FeedBackText;                            // Program end text
  UWORD   OBPTimer;                                 // Graphic update timer
#endif

  // BT search menu
  UBYTE   NoOfDevices;                              // BT search no of devices found
  UBYTE   NoOfNames;                                // BT search no of names found
  UBYTE   SelectedDevice;                           // BT selected device
  UBYTE   SelectedSlot;                             // BT selected slot

  // BT device list menu
  UBYTE   DevicesKnown;                             // BT device known flag
  UBYTE   Devices;                                  // BT devices
  UBYTE   DeviceLeft;                               // BT device left
  UBYTE   DeviceCenter;                             // BT device center
  UBYTE   DeviceRight;                              // BT device right
  UBYTE   DeviceType;                               // BT device type

  // BT connect Menu
  UBYTE   Slots;                                    // BT connect no of slots
  UBYTE   SlotLeft;                                 // BT connect
  UBYTE   SlotCenter;                               // BT connect
  UBYTE   SlotRight;                                // BT connect

  // Get user string
  UBYTE   GUSTmp;                                   // Seperat tmp for "Get user string"
  UBYTE   GUSState;                                 // Seperat state for "Get user string"
  UBYTE   GUSNoname;                                // No user entry
  UBYTE   UserString[DISPLAYLINE_LENGTH + 1];       // User string
  UBYTE   DisplayText[DISPLAYLINE_LENGTH + 1];      // Display buffer
  SBYTE   FigurePointer;                            // Figure cursor
  UBYTE   GUSCursor;                                // User string cursor

  // Connect request
  ULONG   CRPasskey;                                // Passkey to fake wrong pin code
  UBYTE   CRState;                                  // Seperate state for "Connect request"
  UBYTE   CRTmp;                                    // Seperate tmp for "Connect request"

  // Run files
  const UBYTE  *RunIconSave;                              // Menu center icon save
  UWORD   RunTimer;                                 // Bitmap change timer
  UBYTE   RunBitmapPointer;                         // Bitmap pointer

  // Delete files
  UBYTE   SelectedType;                             // Type of selected files for delete

  // View
  SLONG   ViewSampleValue;                          // Latch for sensor values
  UBYTE   ViewSampleValid;                          // Latch for sensor valid

#ifndef STRIPPED
  // Datalog
  ULONG   DatalogOldTick;
  ULONG   DatalogRTC;                               // Real time in mS
  ULONG   DatalogTimer;                             // Logging main timer
  ULONG   DatalogSampleTime;                        // Logging sample time
  ULONG   DatalogSampleTimer;                       // Logging sample timer
  SLONG   DatalogSampleValue[DATALOGPORTS];         // Latch for sensor values
  UBYTE   DatalogSampleValid[DATALOGPORTS];         // Latch for sensor valid
  UWORD   DatalogError;                             // Error code
  UBYTE   DatalogPort[DATALOGPORTS];                // Logging sensor
  UBYTE   Update;                                   // Update icons flag
#endif

  // NV storage
  ULONG   NVTmpLength;                              // Non volatile filelength
  SWORD   NVTmpHandle;                              // Non volatile filehandle
  UBYTE   NVFilename[FILENAME_LENGTH + 1];          // Non volatile file name
  NVDATA  NVData;                                   // Non volatile data

  // Feedback
  UBYTE   *FBText;                                  // Seperate text pointer for feedback
  UWORD   FBTimer;                                  // Seperate timer for feedback
  UBYTE   FBState;                                  // Seperate state for feedback
  UBYTE   FBPointer;                                // Seperate pointer for feedback

  // BT command
  UBYTE   BTIndex;                                  // List index
  UBYTE   BTTmpIndex;                               // Tmp list index
  UBYTE   BTCommand;                                // Last lached BT command
  UBYTE   BTPar1;                                   // Last lached BT command parameter 1
  UBYTE   BTPar2;                                   // Last lached BT command parameter 2
  UWORD   BTResult;                                 // Last lached BT command result

  // Error display
  UBYTE   ErrorTimer;                               // Error show timer
  UBYTE   ErrorFunction;                            // Error latched function
  UBYTE   ErrorParameter;                           // Error latched parameter
  UBYTE   ErrorState;                               // Error latched state
  UBYTE   ErrorString[DISPLAYLINE_LENGTH + 1];      // Error string
}VARSUI;


void      cUiInit(void* pHeader);                   // Init controller
void      cUiCtrl(void);                            // Run  controller
void      cUiExit(void);                            // Exit controller

extern    const HEADER cUi;

#endif
