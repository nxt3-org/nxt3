//
// Date init       14.12.2004
//
// Revision date   $Date:: 24-06-09 12:15                                    $
//
// Filename        $Workfile:: d_loader.h                                    $
//
// Version         $Revision:: 18                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_load $
//
// Platform        C
//

#ifndef   D_LOADER
#define   D_LOADER

#define   HEADERFIXEDSIZE               (FILENAME_SIZE + 4 + 4 + 4 + 2 + 2)
#define   FILENAME_SIZE                 (FILENAME_LENGTH + 1)

#define   FULLNAME                      1
#define   NAME                          2
#define   EXTENTION                     3
#define   WILDCARD                      4

/* Enum related to HandleTable Status */
enum
{
  FREE,
  BUSY,
  DOWNLOADING,
  SEARCHING,
  DLERROR
};

/* Constants related to filetype */
enum
{
  SYSTEMFILE = 0x01,
  DATAFILE   = 0x02,
  LINEAR     = 0x04,
  NONLINEAR  = 0x08
};

/* Enum related to seek operation */
enum
{
  SEEK_FROMSTART,
  SEEK_FROMCURRENT,
  SEEK_FROMEND
};

void      dLoaderInit(void);
UWORD     dLoaderCreateFileHeader(ULONG FileSize, UBYTE *pName, UBYTE LinearState, UBYTE FileType);
UWORD     dLoaderWriteData(UWORD Handle, UBYTE *pBuf, UWORD *pLen);
UWORD     dLoaderCloseHandle(UWORD Handle);
UWORD     dLoaderOpenRead(UBYTE *pFileName, ULONG *pLength);
UWORD     dLoaderSeek(UBYTE Handle, SLONG offset, UBYTE from);
UWORD     dLoaderTell(UBYTE Handle, ULONG* filePos);
UWORD     dLoaderRead(UBYTE Handle, UBYTE *pBuf, ULONG *pLength);
UWORD     dLoaderDelete(UBYTE *pFile);
UWORD     dLoaderFind(UBYTE *pFind, UBYTE *pFound, ULONG *pFileLength, ULONG *pDataLength, UBYTE Session);
UWORD     dLoaderFindNext(UWORD Handle, UBYTE *pFound, ULONG *pFileLength, ULONG *pDataLength);
void      dLoaderDeleteAllFiles(void);
UWORD     dLoaderGetFilePtr(UBYTE *pFileName, UBYTE *pPtrToFile, ULONG *pFileLength);
void      dLoaderCopyFileName(UBYTE *pDst, UBYTE *pSrc);
UWORD     dLoaderOpenAppend(UBYTE *pFileName, ULONG *pAvailSize);
void      dLoaderCpyToLower(UBYTE *pDst, UBYTE *pSrc, UBYTE Length);
UWORD     dLoaderCheckName(UBYTE *pName, UBYTE *pSearchStr, UBYTE SearchType);
void      dLoaderInsertSearchStr(UBYTE *pDst, UBYTE *pSrc, UBYTE *pSearchType);
ULONG     dLoaderReturnFreeUserFlash(void);
UWORD     dLoaderRenameFile(UBYTE Handle, UBYTE *pNewName);
UWORD     dLoaderCheckFiles(UBYTE Handle);

UWORD     dLoaderCropDatafile(UBYTE Handle);



void      dLoaderExit(void);

#endif
