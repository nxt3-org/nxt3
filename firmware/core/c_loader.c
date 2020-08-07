//
// Date init       14.12.2004
//
// Revision date   $Date:: 12-03-08 15:28                                    $
//
// Filename        $Workfile:: c_loader.c                                    $
//
// Version         $Revision:: 5                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_load $
//
// Platform        C
//

#include  "stdconst.h"
#include  "modules.h"
#include  "c_loader.iom.h"
#include  "c_ioctrl.iom.h"
#include  "hal_general.h"
#include  "hal_filesystem.h"
#include  "c_loader.h"
#include <string.h>

static    IOMAPLOADER   IOMapLoader;
static    VARSLOADER    VarsLoader;
static    HEADER        **pHeaders;

const     HEADER        cLoader =
{
  0x00090001L,
  "Loader",
  cLoaderInit,
  cLoaderCtrl,
  cLoaderExit,
  (void *)&IOMapLoader,
  (void *)&VarsLoader,
  (UWORD)sizeof(IOMapLoader),
  (UWORD)sizeof(VarsLoader),
  0x0000                      //Code size - not used so far
};

UWORD     cLoaderFileRq(UBYTE Cmd, UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength);
UWORD     cLoaderGetIoMapInfo(ULONG ModuleId, UBYTE *pIoMap, UWORD *pIoMapSize);
UWORD     cLoaderFindModule(UBYTE *pBuffer);
void      cLoaderGetModuleName(UBYTE *pDst, UBYTE *pModule);
UWORD     cLoaderCreateFile(UBYTE *pFileName, ULONG *pLength);
UWORD     cLoaderRenameFile(UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength);
UWORD     cLoaderOpenRead(UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength, UBYTE bLinear);
UWORD     cLoaderOpenAppend(UBYTE *pFileName, ULONG *pLength);
UWORD     cLoaderDeleteFile(UBYTE *pFileName);
UWORD     cLoaderResizeFile(UBYTE *pFileName, ULONG pLength);

void      cLoaderInit(void* pHeader)
{

  IOMapLoader.pFunc       = &cLoaderFileRq;
  VarsLoader.IoMapHandle  = FALSE;
  VarsLoader.Resizing     = FALSE;
  pHeaders = pHeader;
  if (!Hal_Fs_RefAdd())
      Hal_General_AbnormalExit("Cannot initialize file system");

  Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
}

void      cLoaderCtrl(void)
{
}

UWORD cLoaderCreateFile(UBYTE *pFileName, ULONG *pLength)
{
  errhnd_t state = Hal_Fs_CreateWrite((const char *) pFileName, *pLength);
  if (FS_ISERR(state)) {
    Hal_Fs_Close(state);
  } else {
    Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
  }
  return state;
}

UWORD cLoaderRenameFile(UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength)
{
  if (!Hal_Fs_CheckForbiddenFilename((const char*) pFileName))
      return ILLEGALFILENAME;
  if (!Hal_Fs_CheckForbiddenFilename((const char*) pBuffer))
      return ILLEGALFILENAME;

  /* check if dst file exists */
  errhnd_t hnd = Hal_Fs_Locate((char*) pBuffer, NULL, NULL);
  Hal_Fs_Close(hnd);
  if (FILENOTFOUND != FS_ERR(hnd)) {
    return SUCCESS == LOADER_ERR(hnd) ? FILEEXISTS : FS_ERR(hnd);
  }

  hnd = Hal_Fs_Locate((char *)pFileName, NULL, NULL);
  if (FS_ISERR(hnd)) {
    Hal_Fs_Close(hnd);
    return hnd;
  }

  file_meta_t meta;
  error_t err = Hal_Fs_GetMeta(hnd, &meta);
  if (FS_ISERR(err)) {
    Hal_Fs_Close(hnd);
    return hnd | err;
  }

  *pLength = meta.allocated;

  err = Hal_Fs_CheckHandleIsExclusive(hnd, true);
  if (FS_ISERR(err)) {
    Hal_Fs_Close(hnd);
    return hnd | err;
  }

  err = Hal_Fs_RenameFile(hnd, (const char *) pBuffer);
  Hal_Fs_Close(hnd);
  return hnd | err;
}

UWORD cLoaderOpenRead(UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength, UBYTE bLinear)
{
  file_meta_t meta;
  error_t err;
  errhnd_t hnd = Hal_Fs_Locate((char*) pFileName, NULL, NULL);
  if (FS_ISERR(hnd))
      return hnd;

  if (!bLinear)
      err = Hal_Fs_OpenRead(hnd);
  else
      err = Hal_Fs_MapFile(hnd, (const uint8_t **) pBuffer, NULL);

  if (FS_ISERR(err)) {
    Hal_Fs_Close(hnd);
    return err;
  }

  err = Hal_Fs_GetMeta(hnd, &meta);
  if (FS_ISERR(err)) {
    Hal_Fs_Close(hnd);
    return err;
  }
  *pLength = meta.allocated;
  return hnd;
}

UWORD cLoaderOpenAppend(UBYTE *pFileName, ULONG *pLength)
{
    file_meta_t meta;
    error_t err;
    errhnd_t hnd = Hal_Fs_Locate((const char*) pFileName, NULL, (uint32_t*) pLength);
    if (FS_ISERR(hnd)) {
        return hnd;
    }

    err = Hal_Fs_OpenAppend(hnd);
    if (FS_ISERR(err)) {
        Hal_Fs_Close(hnd);
        return err;
    }

    err = Hal_Fs_GetMeta(hnd, &meta);
    if (FS_ISERR(err)) {
        Hal_Fs_Close(hnd);
        return err;
    }

    *pLength = meta.allocated;
    return hnd;
}

UWORD cLoaderDeleteFile(UBYTE *pFileName)
{
  errhnd_t hnd = Hal_Fs_Locate((const char*) pFileName, NULL, NULL);
  if (!FS_ISERR(hnd))
      hnd = Hal_Fs_DeleteFile(FS_HND(hnd));
  Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
  return hnd;
}

UWORD cLoaderResizeFile(UBYTE *pFileName, ULONG pLength)
{
  errhnd_t hnd = Hal_Fs_Locate((const char*) pFileName, NULL, NULL);
  if (!FS_ISERR(hnd))
    hnd = Hal_Fs_Resize(FS_HND(hnd), pLength);
  Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
  return hnd;
}

UWORD     cLoaderFileRq(UBYTE Cmd, UBYTE *pFileName, UBYTE *pBuffer, ULONG *pLength)
{
  UWORD   ReturnState;

  ReturnState = SUCCESS;

  switch(Cmd)
  {
    case OPENREAD:
    {
      ReturnState = cLoaderOpenRead(pFileName, pBuffer, pLength, false);
    }
    break;
    case OPENREADLINEAR:
    {
      ReturnState = cLoaderOpenRead(pFileName, pBuffer, pLength, true);
    }
    break;
    case OPENWRITE:
    case OPENWRITELINEAR:
    case OPENWRITEDATA:
    {
      /* This is to create a new file */
      ReturnState = cLoaderCreateFile(pFileName, pLength);
    }
    break;
    case OPENAPPENDDATA:
    {
      ReturnState = cLoaderOpenAppend(pFileName, pLength);
    }
    break;
    case CLOSE:
    {
      ReturnState = Hal_Fs_Close(*pFileName);
    }
    break;
    case CROPDATAFILE:
    {
      handle_t hnd = *pFileName;
      error_t err;
      err = Hal_Fs_Truncate(hnd);
      ReturnState = hnd | err;
      Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
    }
    break;
    case RESIZEDATAFILE:
    {
      ReturnState = cLoaderResizeFile(pFileName, *pLength);
      Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
    }
    break;
    case SEEKFROMSTART:
    case SEEKFROMCURRENT:
    case SEEKFROMEND:
    {
      handle_t hnd = *pFileName;
      error_t err;
      err = Hal_Fs_Seek(hnd, *(int32_t*)pLength, Cmd-SEEKFROMSTART);
      ReturnState = hnd | err;
    }
    break;
    case FILEPOSITION:
    {
        handle_t hnd = *pFileName;
        error_t err;
        err = Hal_Fs_Tell(hnd, (uint32_t*)pLength);
        ReturnState = hnd | err;
    }
    break;
    case READ:
    {
      handle_t hnd = *pFileName;
      error_t err;
      err = Hal_Fs_Read(hnd, pBuffer, (uint32_t*)pLength);
      ReturnState = hnd | err;
    }
    break;
    case WRITE:
    {
      handle_t hnd = *pFileName;
      error_t err;
      err = Hal_Fs_Write(hnd, pBuffer, (uint32_t*)pLength);
      ReturnState = hnd | err;
    }
    break;
    case FINDFIRST:
    {
      ReturnState = Hal_Fs_Locate((const char*) pFileName, (char*) pBuffer, (uint32_t*)pLength);
      if (FS_ISERR(ReturnState)) {
          Hal_Fs_Close(ReturnState);
      }
    }
    break;
    case FINDNEXT:
    {
      handle_t hnd = *pFileName;
      ReturnState = Hal_Fs_LocateNext(hnd, (char*) pBuffer, (uint32_t*) pLength);
    }
    break;
    case DELETE:
    {
      ReturnState = cLoaderDeleteFile(pFileName);
    }
    break;
    case DELETEUSERFLASH:
    {
      ReturnState = Hal_Fs_DeleteAll();
      Hal_Fs_GetFreeStorage(&IOMapLoader.FreeUserFlash);
    }
    break;

    case FINDFIRSTMODULE:
    {
      if (FALSE == VarsLoader.IoMapHandle)
      {
        VarsLoader.IoMapHandle    = TRUE;
        VarsLoader.ModSearchIndex = 0;
        Hal_Fs_ParseQuery((const char*) pFileName, &VarsLoader.ModSearch);
        ReturnState = cLoaderFindModule(pBuffer);
      }
      else
      {
        ReturnState = NOMOREHANDLES;
      }
    }
    break;

    case FINDNEXTMODULE:
    {
      ReturnState = cLoaderFindModule(pBuffer);
    }
    break;

    case CLOSEMODHANDLE:
    {
      VarsLoader.IoMapHandle  = FALSE;
      ReturnState             = SUCCESS;
    }
    break;

    case IOMAPREAD:
    {

      UBYTE *pIoMap;
      ULONG Ptr;
      UWORD IoMapSize;
      UBYTE Tmp;

      pIoMap = NULL;
      ReturnState = cLoaderGetIoMapInfo((*(ULONG*)(pFileName)),(UBYTE*)(&pIoMap), &IoMapSize);

      /* Did we have a valid module ID ?*/
      if (SUCCESS == LOADER_ERR(ReturnState))
      {

        /* This is the offset  */
        Ptr  =   pBuffer[0];
        Ptr |=  (UWORD)pBuffer[1] << 8;

        /* is the offset within the limits of the iomap size? */
        if ((Ptr + *pLength) <= IoMapSize)
        {

          /* Add the offset to the pointer */
          pIoMap += Ptr;

          for (Tmp = 0; Tmp < *pLength; Tmp++)
          {
            pBuffer[Tmp + 2] = *pIoMap;
            pIoMap++;
          }
        }
        else
        {

          /* Error - not within the bounderies */
          ReturnState = OUTOFBOUNDERY;
          *pLength    = 0;
        }
      }
      else
      {

        /* Error - not a valid module id */
        *pLength = 0;
      }
    }
    break;

    case IOMAPWRITE:
    {
      UBYTE *pIoMap;
      ULONG Ptr;
      UWORD IoMapSize;
      UWORD Tmp;


      pIoMap = NULL;
      ReturnState = cLoaderGetIoMapInfo(*((ULONG*)pFileName), (UBYTE*)&pIoMap, &IoMapSize);

      if (LOADER_ERR(ReturnState) == SUCCESS)
      {

        /* This is the offset  */
        Ptr  = *pBuffer;
        pBuffer++;
        Tmp = *pBuffer;
        Ptr |= Tmp << 8;
        pBuffer++;

        if ((Ptr + *pLength) <= IoMapSize)
        {

          pIoMap += Ptr;
          for (Tmp = 0; Tmp < *pLength; Tmp++)
          {
            *pIoMap = pBuffer[Tmp];
            pIoMap++;
          }
        }
        else
        {

          /* Error - not within the bounderies */
          ReturnState  = OUTOFBOUNDERY;
          *pLength = 0;
        }
      }
      else
      {

        /* Error - not a valid module id */
        *pLength = 0;
      }
    }
    break;

    case RENAMEFILE:
    {
      ReturnState = cLoaderRenameFile(pFileName, pBuffer, pLength);
    }
    break;

    default:
    {
    }
    break;
  }
  return (ReturnState);
}

UWORD     cLoaderGetIoMapInfo(ULONG ModuleId, UBYTE *pIoMap, UWORD *pIoMapSize)
{
  UBYTE   Tmp;
  UBYTE   Exit;
  UWORD   RtnVal;

  RtnVal = SUCCESS;
  Tmp = 0;
  Exit = FALSE;
  while((Tmp < 32) && (Exit == FALSE))
  {
    if ((*(pHeaders[Tmp])).ModuleID == ModuleId)
    {
      Exit = TRUE;
    }
    else
    {
      Tmp++;
    }
  }

  /* Did we have a valid module ID ?*/
  if (TRUE == Exit)
  {
    /* Get the pointer of the module io map */
   *((ULONG *)pIoMap) = (ULONG)((*(pHeaders[Tmp])).pIOMap);
    *pIoMapSize = (*(pHeaders[Tmp])).IOMapSize;
  }
  else
  {
    RtnVal = MODULENOTFOUND;
  }

  /* To avoid a warning - this is optimized away */
  *pIoMap = *pIoMap;
  return(RtnVal);
}

UWORD     cLoaderFindModule(UBYTE *pBuffer)
{
  UBYTE   Tmp;
  UWORD   RtnVal;
  UBYTE   ModuleName[FILENAME_SIZE];

  RtnVal  = MODULENOTFOUND;

  for (Tmp = VarsLoader.ModSearchIndex; Tmp < 32; Tmp++)
  {
    if (pHeaders[Tmp] != 0)
    {
      cLoaderGetModuleName(ModuleName, ((*(pHeaders[Tmp])).ModuleName));
      if (SUCCESS == Hal_Fs_CheckQuery((const char*) ModuleName, &VarsLoader.ModSearch))
      {
        strncpy((char*) pBuffer, (const char*) ModuleName, FS_NAME_MAX_CHARS);

        pBuffer[FILENAME_SIZE] = (UBYTE) ((*(pHeaders[Tmp])).ModuleID);
        pBuffer[FILENAME_SIZE + 1] = (UBYTE)(((*(pHeaders[Tmp])).ModuleID) >> 8);
        pBuffer[FILENAME_SIZE + 2] = (UBYTE)(((*(pHeaders[Tmp])).ModuleID) >> 16);
        pBuffer[FILENAME_SIZE + 3] = (UBYTE)(((*(pHeaders[Tmp])).ModuleID) >> 24);

        pBuffer[FILENAME_SIZE + 4] = (UBYTE)(((*(pHeaders[Tmp])).ModuleSize));
        pBuffer[FILENAME_SIZE + 5] = (UBYTE)(((*(pHeaders[Tmp])).ModuleSize) >> 8);
        pBuffer[FILENAME_SIZE + 6] = (UBYTE)(((*(pHeaders[Tmp])).ModuleSize) >> 16);
        pBuffer[FILENAME_SIZE + 7] = (UBYTE)(((*(pHeaders[Tmp])).ModuleSize) >> 24);

        pBuffer[FILENAME_SIZE + 8] = (UBYTE) ((*(pHeaders[Tmp])).IOMapSize);
        pBuffer[FILENAME_SIZE + 9] = (UBYTE)(((*(pHeaders[Tmp])).IOMapSize) >> 8);

        RtnVal     = SUCCESS;
        (VarsLoader.ModSearchIndex)    = Tmp + 1;
        Tmp        = 32;
      }
    }
  }
  return(RtnVal);
}

void      cLoaderGetModuleName(UBYTE *pDst, UBYTE *pModule)
{
  const char *modName = (const char *) pModule;
  char *nameEnd = stpncpy((char*) pDst, modName, FS_NAME_MAX_CHARS);
  stpncpy(nameEnd, ".mod", FS_NAME_MAX_CHARS - (nameEnd - modName));
}

void      cLoaderExit(void)
{
    Hal_Fs_RefDel();
}
