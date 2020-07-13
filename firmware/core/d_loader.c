//
// Date init       14.12.2004
//
// Revision date   $Date:: 24-06-09 8:53                                     $
//
// Filename        $Workfile:: d_loader.c                                    $
//
// Version         $Revision:: 18                                            $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_load $
//
// Platform        C
//

#include "stdconst.h"
#include "modules.h"
#include "m_sched.h"
#include "d_loader.h"
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include "d_loader_ev3.h"

#define   MAX_FILES                     ((FILETABLE_SIZE) - 1)  /* Last file entry is used for file version*/
#define   IS_LOADER_ERR(LStatus)        (((LStatus) & 0xFF00) != SUCCESS)

typedef struct {
  EV3_FS_CB    block;
  DIR         *dirStream;
  UBYTE        status;
  char         searchString[FILENAME_SIZE];
  UBYTE        searchType;
} HANDLE;

static HANDLE   HandleTable[MAX_HANDLES];

UWORD     dLoaderGetFreeHandle(void);
UWORD     dLoaderCheckHandleForReadWrite(UWORD Handle);
UWORD     dLoaderCheckHandle(UWORD Handle, UBYTE Operation);
UWORD     dLoaderCheckDownload(UBYTE *pName);
UWORD     dLoaderCloseDirectory(UWORD Handle);


void      dLoaderInit(void)
{
  /* Clear handle table */
  for (int hnd = 0; hnd < MAX_HANDLES; hnd++)
  {
    HandleTable[hnd].status = FREE;
    HandleTable[hnd].block = EV3_FS_CB_INIT;
    HandleTable[hnd].dirStream = NULL;
  }

  ev3FsInit("/home/root/lms2012/prjs/NXT3/data", "/home/root/lms2012/prjs/NXT3/meta");
}


void      dLoaderDeleteAllFiles(void)
{
  for (UWORD hnd = 0; hnd < MAX_HANDLES; hnd++) {

    if (HandleTable[hnd].status == DOWNLOADING ||
        HandleTable[hnd].status == BUSY) {
      dLoaderCloseHandle(hnd);
    }
  }

  UWORD hnd = dLoaderFind((UBYTE*) "*.*", NULL, NULL, NULL, SEARCHING);

  if (hnd != NOMOREHANDLES) {
    while (hnd < 0x8000) {
      dLoaderDelete((UBYTE*) HandleTable[hnd].block.name);
      if (dLoaderFindNext(hnd, NULL, NULL, NULL) >= 0x8000)
        break;
    }

    dLoaderCloseHandle(hnd);
  }
}


UWORD     dLoaderCreateFileHeader(ULONG FileSize, UBYTE *pName, UBYTE LinearState, UBYTE FileType)
{
  UWORD Handle;

  Handle = dLoaderFind(pName, NULL, NULL, NULL, (UBYTE)BUSY);

  if (SUCCESS      == (Handle & 0xFF00))
    return Handle | FILEEXISTS;

  if (FILENOTFOUND != (Handle & 0xFF00))
    return Handle | UNDEFINEDERROR;

  Handle &= 0x00FF;

  /* Here check for the download buffers for a matching download */
  /* in progress                                                 */
  if (dLoaderCheckDownload(pName) > 0x8000)
    return Handle | FILEISBUSY;

  if (ev3FsCreateWrite(&HandleTable[Handle].block, (char*) pName, FileSize) >= 0) {
    HandleTable[Handle].status = DOWNLOADING;
    return Handle;
  } else {
    return Handle | UNDEFINEDERROR;
  }
}


UWORD     dLoaderWriteData(UWORD Handle, UBYTE *pBuf, UWORD *pLen)
{
  Handle = dLoaderCheckHandle(Handle, DOWNLOADING);
  if (Handle < 0x8000)
  {
    ULONG longLen = *pLen;

    int result = ev3FsWrite(&HandleTable[Handle].block, pBuf, &longLen);
    if (result == RETURN_OK) {
      *pLen = longLen;

    } else if (result == RETURN_EOF) {
      *pLen = longLen;
      Handle |= EOFEXSPECTED;

    } else {
      if (errno == ENOSPC) {
        Handle |= NOSPACE;
      } else {
        Handle |= UNDEFINEDERROR;
      }
    }
  }
  return Handle;
}


UWORD     dLoaderGetFreeHandle(void)
{
  for (UWORD hnd = 0; hnd < MAX_HANDLES; hnd++)
  {
    if (FREE == HandleTable[hnd].status)
    {
      HandleTable[hnd].status = BUSY;
      return hnd;
    }
  }
  return NOMOREHANDLES;
}

UWORD     dLoaderCloseHandle(UWORD Handle)
{
  UWORD       RtnStatus;

  RtnStatus = Handle;

  /* if it is a normal handle or handle closed due to an error then error must be different */
  /* from the no more handles available error (else you would delete a used handle)         */
  if (((Handle < 0x8000) || (NOMOREHANDLES != (Handle & 0xFF00))) && ((UBYTE)Handle < MAX_HANDLES))
  {
    Handle &= 0x00FF;

    if (FREE == HandleTable[Handle].status)
    {
      RtnStatus |= HANDLEALREADYCLOSED;
    }
    else
    {
      HandleTable[Handle].status = FREE;
      dLoaderCloseDirectory(Handle);
      if (ev3FsClose(&HandleTable[Handle].block) < 0) {
        RtnStatus |= UNDEFINEDERROR;
      }
    }
  }
  return RtnStatus;
}

UWORD     dLoaderCloseDirectory(UWORD Handle) {

  Handle &= 0x00FF;

  UWORD returnValue = Handle;

  if (HandleTable[Handle].dirStream != NULL)
  {
    if (closedir(HandleTable[Handle].dirStream) < 0)
    {
      perror("EV3 FS: cannot close directory");
      returnValue = Handle | UNDEFINEDERROR;
    }
    HandleTable[Handle].dirStream = NULL;
  }

  return returnValue;
}

UWORD     dLoaderOpenRead(UBYTE *pFileName, ULONG *pLength)
{
  UWORD Handle = dLoaderFind(pFileName, NULL, NULL, NULL, (UBYTE)BUSY);

  if (0x8000 > Handle)
  {
    if (ev3FsOpenRead(&HandleTable[Handle].block, NULL) < 0) {
      Handle |= UNDEFINEDERROR;
    }
  }
  return Handle;
}

UWORD     dLoaderSeek(UBYTE Handle, SLONG offset, UBYTE from)
{
  UWORD Status = dLoaderCheckHandleForReadWrite(Handle);
  if (Status < 0x8000)
  {
    Status = Handle;

    if (ev3FsSeekRead(&HandleTable[Handle].block, offset, from) < 0) {
      if (errno == EINVAL)
        Status |= INVALIDSEEK;
      else
        Status |= UNDEFINEDERROR;
    }
  }
  return (Status);
}

UWORD     dLoaderTell(UBYTE Handle, ULONG* filePos)
{
  UWORD Status = dLoaderCheckHandleForReadWrite(Handle);
  if (Status < 0x8000)
  {
    Status = Handle;

    if (ev3FsTellRead(&HandleTable[Handle].block, filePos) < 0) {
      Status |= UNDEFINEDERROR;
    }
  }
  return Status;
}

UWORD      dLoaderRead(UBYTE Handle, UBYTE *pBuffer, ULONG *pLength)
{
  UWORD Status = dLoaderCheckHandle(Handle, BUSY);
  if (Status < 0x8000)
  {
    Status  = Handle;

    int result = ev3FsRead(&HandleTable[Handle].block, pBuffer, pLength);
    if (result == RETURN_OK) {
      // ok
    } else if (result == RETURN_EOF) {
      Status |= ENDOFFILE;

    } else if (result == RETURN_ERR) {
      if (errno == ENOSPC) {
        Status |= NOSPACE;

      } else {
        Status |= UNDEFINEDERROR;
      }
    }
  }
  return Status;
}

UWORD      dLoaderDelete(UBYTE *pFile)
{
  UWORD LStatus = dLoaderFind(pFile, NULL, NULL, NULL, (UBYTE)BUSY);

  if (!IS_LOADER_ERR(LStatus))
  {
    if (ev3FsRemove(&HandleTable[LStatus].block) < 0) {
      if (errno == ENOENT) {
        LStatus |= FILENOTFOUND;
      } else {
        LStatus |= UNDEFINEDERROR;
      }
    }
  }

  dLoaderCloseHandle(LStatus);

  return LStatus;
}

UWORD     dLoaderFind(UBYTE *pFind, UBYTE *pFound, ULONG *pFileLength, ULONG *pDataLength, UBYTE Session)
{
  UWORD Handle = dLoaderGetFreeHandle();

  if (Handle >= 0x8000)
    return Handle;

  if (strlen((const char*)pFind) > FILENAME_LENGTH)
    return Handle | ILLEGALFILENAME;

  HandleTable[Handle].status = Session;
  dLoaderInsertSearchStr((UBYTE*) HandleTable[Handle].searchString, pFind, &HandleTable[Handle].searchType);

  Handle = dLoaderFindNext(Handle, pFound, pFileLength, pDataLength);

  return Handle;
}

UWORD     dLoaderFindNext(UWORD Handle, UBYTE *pFound, ULONG *pFileLength, ULONG *pDataLength)
{
  UWORD   ReturnVal;

  if (pFileLength) *pFileLength = 0;

  ReturnVal     = Handle | FILENOTFOUND;

  if (HandleTable[Handle].dirStream == NULL)
  {
    if (ev3FsStartBrowse(&HandleTable[Handle].dirStream) < 0)
      return Handle | UNDEFINEDERROR;
  }

  errno = 0;
  struct dirent *entry;
  while ((entry = readdir(HandleTable[Handle].dirStream)) != NULL) {

    if (SUCCESS == dLoaderCheckName((UBYTE*) entry->d_name, (UBYTE*) HandleTable[Handle].searchString, HandleTable[Handle].searchType))
    {
      ReturnVal = Handle;
      errno = 0;
      break;
    }

    errno = 0;
  }
  if (errno != 0) {
    perror("EV3 FS: cannot read directory");
    return Handle | UNDEFINEDERROR;
  }

  if (IS_LOADER_ERR(ReturnVal))
    return ReturnVal;

  if (ev3FsLoadMeta(&HandleTable[ReturnVal].block, entry->d_name) >= 0) {
    if (pFileLength) *pFileLength = HandleTable[ReturnVal].block.fullLength;
    if (pDataLength) *pDataLength = HandleTable[ReturnVal].block.writePointer;
    if (pFound) {
      dLoaderCopyFileName(pFound, (UBYTE *)HandleTable[ReturnVal].block.name);
    }
  } else {
    Handle |= UNDEFINEDERROR;
  }

  return ReturnVal;
}


UWORD     dLoaderGetFilePtr(UBYTE *pFileName, UBYTE *pPtrToFile, ULONG *pFileLength)
{
  UWORD RtnVal = dLoaderFind(pFileName, NULL, NULL, NULL, (UBYTE)BUSY);

  if (RtnVal < 0x8000)
  {
    if (ev3FsOpenRead(&HandleTable[RtnVal].block, NULL) < 0) {
      return RtnVal | UNDEFINEDERROR;
    }

    UBYTE *memory = NULL;
    if (ev3FsMmap(&HandleTable[RtnVal].block, &memory) < 0) {
      return RtnVal | UNDEFINEDERROR;
    }

    if (pFileLength) {
      *pFileLength = HandleTable[RtnVal].block.fullLength;
    }
    if (pPtrToFile) {
      *((UBYTE**)pPtrToFile) = memory;
    }
  }
  return RtnVal;
}


UWORD     dLoaderCheckFiles(UBYTE Handle)
{
  char TopName[FILENAME_SIZE];
  dLoaderCpyToLower((UBYTE*) TopName, (UBYTE*) HandleTable[Handle].block.name, FILENAME_LENGTH);

  for (int hnd = 0; hnd < MAX_HANDLES; hnd++) {

    if (    (    (BUSY        == HandleTable[hnd].status)
              || (DOWNLOADING == HandleTable[hnd].status))
         && (SUCCESS == dLoaderCheckName((UBYTE*) HandleTable[hnd].block.name, (UBYTE*) TopName, FULLNAME))
         && (hnd != Handle)) {
      return Handle | FILEISBUSY;
    }
  }
  return Handle;
}


void     dLoaderCopyFileName(UBYTE *pDst, UBYTE *pSrc)
{
  for (int i = 0; i < FILENAME_SIZE; i++, pDst++)
  {
    if ('\0' != *pSrc)
    {
      *pDst = *pSrc;
      pSrc++;
    }
    else
    {
      *pDst = '\0';
    }
  }
}


UWORD     dLoaderOpenAppend(UBYTE *pFileName, ULONG *pAvailSize)
{
  *pAvailSize = 0;
  ULONG fileSize, dataSize;

  UWORD Handle = dLoaderFind(pFileName, NULL, &fileSize, &dataSize, (UBYTE)BUSY);

  if (Handle >= 0x8000)
    return Handle;

  /* Check for an append in progress for this file */
  if (dLoaderCheckDownload(pFileName) >= 0x8000)
    return Handle | FILEISBUSY;

  if (fileSize <= dataSize)
    return Handle | FILEISFULL;

  if (ev3FsOpenAppend(&HandleTable[Handle].block, NULL) < 0)
    return Handle | UNDEFINEDERROR;

  *pAvailSize = fileSize - dataSize;
  return Handle;
}


void      dLoaderCpyToLower(UBYTE *pDst, UBYTE *pSrc, UBYTE Length)
{
  UBYTE   Tmp;

  for(Tmp = 0; Tmp < Length; Tmp++)
  {
    pDst[Tmp] =(UBYTE)toupper((UWORD)pSrc[Tmp]);
  }

  /* The requried length has been copied - now fill with zeros */
  for(Tmp = Length; Tmp < FILENAME_SIZE; Tmp++)
  {
    pDst[Tmp] = '\0';
  }
}

UWORD     dLoaderCheckName(UBYTE *pName, UBYTE *pSearchStr, UBYTE SearchType)
{
  UBYTE TmpName[FILENAME_SIZE];
  dLoaderCpyToLower(TmpName, pName, (UBYTE)FILENAME_SIZE);

  UBYTE found;
  switch (SearchType)
  {
    case FULLNAME:
      found = (0 == strcmp((const char*)TmpName, (const char *)pSearchStr));
      break;
    case NAME:
      found = (0 == memcmp(TmpName, pSearchStr, strlen((const char *)pSearchStr)));
      break;
    case EXTENTION:
      found = (0 != strstr((const char *)TmpName, (const char*)pSearchStr));
      break;
    default:
    case WILDCARD:
      found = TRUE;
      break;
  }

  if (found)
    return SUCCESS;
  else
    return UNDEFINEDERROR;
}

void      dLoaderInsertSearchStr(UBYTE *pDst, UBYTE *pSrc, UBYTE *pSearchType)
{
  if (0 != strstr((char const *)pSrc, "*.*")) {
    /* find all */
    pDst[0] = '\0';
    *pSearchType = WILDCARD;
    return;
  }

  /* Using other wild cards? */
  int len = strlen((const char *) pSrc);
  if (0 != strstr((const char*) pSrc, ".*")) {

    /* Extention wildcard */
    dLoaderCpyToLower(pDst, pSrc, len-1);
    *pSearchType = NAME;
    return;
  }

  if (0 != strstr((PSZ)(pSrc), "*.")) {

    /* Filename wildcard */
    dLoaderCpyToLower(pDst, &pSrc[1], (UBYTE)4);
    *pSearchType = EXTENTION;
    return;
  }

  /* no wildcards used */
  dLoaderCpyToLower(pDst, pSrc, len);
  *pSearchType = FULLNAME;
}

UWORD     dLoaderCheckHandleForReadWrite(UWORD Handle)
{
  if (Handle >= MAX_HANDLES)
    return Handle | ILLEGALHANDLE;

  if (    (DOWNLOADING != HandleTable[(UBYTE)Handle].status)
       && (BUSY        != HandleTable[(UBYTE)Handle].status)) {
    return Handle | ILLEGALHANDLE;
  } else {
    return Handle;
  }
}

UWORD     dLoaderCheckHandle(UWORD Handle, UBYTE Operation)
{
  if (Handle >= MAX_HANDLES)
    return Handle | ILLEGALHANDLE;

  if (Operation != HandleTable[Handle].status)
    return Handle | ILLEGALHANDLE;

  return Handle;
}

ULONG     dLoaderReturnFreeUserFlash(void)
{
  ULONG bytes = 0;

  if (ev3FsGetFreeBytes(&bytes) >= 0) {
    return bytes;
  } else {
    return 0;
  }
}

UWORD     dLoaderRenameFile(UBYTE Handle, UBYTE *pNewName)
{
  if (Handle >= MAX_HANDLES)
    return Handle | ILLEGALHANDLE;

  if (strlen(HandleTable[Handle].block.name) == 0)
    return Handle | ILLEGALHANDLE;

  if (ev3FsMove(&HandleTable[Handle].block, (char*) pNewName) < 0)
    return Handle | UNDEFINEDERROR;

  return Handle;
}


UWORD     dLoaderCheckDownload(UBYTE *pName)
{
  for (int hnd = 0; hnd < MAX_HANDLES; hnd++) {

    if (DOWNLOADING == HandleTable[hnd].status) {
      if (SUCCESS == dLoaderCheckName(pName, (UBYTE*) HandleTable[hnd].searchString, FULLNAME)) {
        return FILEEXISTS;
      }
    }
  }
  return SUCCESS;
}




UWORD     dLoaderCropDatafile(UBYTE Handle)
{
  if (dLoaderCheckHandleForReadWrite(Handle) <= 0x8000)
    return Handle | ILLEGALHANDLE;

  HandleTable[Handle].status = FREE;

  if (ev3FsShrink(&HandleTable[Handle].block) < 0)
    return Handle | UNDEFINEDERROR;

  return Handle;
}

void      dLoaderExit(void)
{
  ev3FsDeinit();
}
