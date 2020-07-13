//
// Date init       14.12.2004
//
// Revision date   $Date:: 19-03-10 12:36                                    $
//
// Filename        $Workfile:: c_loader.iom                                  $
//
// Version         $Revision:: 17                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_load $
//
// Platform        C
//

#ifndef   CLOADER_IOM
#define   CLOADER_IOM

#define   pMapLoader ((IOMAPLOADER*)(pHeaders[ENTRY_LOADER]->pIOMap))

//Version numbers are two bytes, MAJOR.MINOR (big-endian)
//For example, version 1.5 would be 0x0105
//If these switch to little-endian, be sure to update
//definition and usages of VM_OLDEST_COMPATIBLE_VERSION, too!
#define   SOFTWAREVERSION               0x0001 //0.01
#define   FIRMWAREVERSION               0x0120 //1.32
#define   PROTOCOLVERSION               0x017C //1.124

enum
{
  OPENREAD        = 0x80,
  OPENWRITE       = 0x81,
  READ            = 0x82,
  WRITE           = 0x83,
  CLOSE           = 0x84,
  DELETE          = 0x85,
  FINDFIRST       = 0x86,
  FINDNEXT        = 0x87,
  VERSIONS        = 0x88,
  OPENWRITELINEAR = 0x89,
  OPENREADLINEAR  = 0x8A,
  OPENWRITEDATA   = 0x8B,
  OPENAPPENDDATA  = 0x8C,
  CROPDATAFILE    = 0x8D,    /* New cmd for datalogging */
//  XXXXXXXXXXXXXX  = 0x8E,
//  XXXXXXXXXXXXXX  = 0x8F,
  FINDFIRSTMODULE = 0x90,
  FINDNEXTMODULE  = 0x91,
  CLOSEMODHANDLE  = 0x92,
//  XXXXXXXXXXXXXX  = 0x93,
  IOMAPREAD       = 0x94,
  IOMAPWRITE      = 0x95,
//  XXXXXXXXXXXXXX  = 0x96,
  BOOTCMD         = 0x97,     /* external command only */
  SETBRICKNAME    = 0x98,
//  XXXXXXXXXXXXXX  = 0x99,
  BTGETADR        = 0x9A,
  DEVICEINFO      = 0x9B,
//  XXXXXXXXXXXXXX  = 0x9C,
//  XXXXXXXXXXXXXX  = 0x9D,
//  XXXXXXXXXXXXXX  = 0x9E,
//  XXXXXXXXXXXXXX  = 0x9F,
  DELETEUSERFLASH = 0xA0,
  POLLCMDLEN      = 0xA1,
  POLLCMD         = 0xA2,
  RENAMEFILE      = 0xA3,
  BTFACTORYRESET  = 0xA4,

  // enhanced firmware additions
  RESIZEDATAFILE  = 0xD0,
  SEEKFROMSTART   = 0xD1,
  SEEKFROMCURRENT = 0xD2,
  SEEKFROMEND     = 0xD3,
  FILEPOSITION    = 0xD4
};

typedef UWORD LOADER_STATUS;

//Mask out handle byte of Loader status word for error code checks
#define LOADER_ERR(StatusWord) ((StatusWord & 0xFF00))

//Byte value of error half of Loader status word
#define LOADER_ERR_BYTE(StatusWord) ((UBYTE)((StatusWord & 0xFF00) >> 8))

//Value of handle inside Loader status word
#define LOADER_HANDLE(StatusWord) ((UBYTE)(StatusWord))

//Pointer to lower byte of Loader status word
#define LOADER_HANDLE_P(StatusWord) ((UBYTE*)(&StatusWord))

typedef   struct
{
  UWORD   (*pFunc)(UBYTE, UBYTE *, UBYTE *, ULONG *);
  ULONG   FreeUserFlash;
}IOMAPLOADER;


#endif



