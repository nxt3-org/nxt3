#include <stdconst.h>
#include <string.h>
#include <stdio.h>
#include "modules.h"
#include "c_cmd.h"
#include "c_comm.iom.h"
#include "c_loader.iom.h"

#if WRITE_IOMAP_OFFSETS
void cCmdWriteIOMapOffsetsFile(HEADER **pHeaders)
{
  LOADER_STATUS LStatus;
  UBYTE Handle;
  ULONG BenchFileSize;
  ULONG Length;
  UBYTE Buffer[256];

  //Remove old benchmark file, create a new one
  strcpy((char *)Buffer, "offsets.txt");
  pMapLoader->pFunc(DELETE, Buffer, NULL, NULL);
  BenchFileSize = 2048;
  LStatus = pMapLoader->pFunc(OPENWRITEDATA, Buffer, NULL, &BenchFileSize);

  if (!LOADER_ERR(LStatus))
  {
    //Write Benchmark file
    Handle = LOADER_HANDLE(LStatus);

    //Header
    sprintf((char *)Buffer, "%s Offsets\r\n", "Comm Module");
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "pFunc: %ld\r\n", (ULONG)pMapComm->pFunc - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "pFunc2: %ld\r\n", (ULONG)pMapComm->pFunc2 - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtDeviceTable: %ld\r\n", (ULONG)pMapComm->BtDeviceTable - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtConnectTable: %ld\r\n", (ULONG)pMapComm->BtConnectTable - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BrickData: %ld\r\n", (ULONG)pMapComm->BrickData.Name - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtInBuf: %ld\r\n", (ULONG)pMapComm->BtInBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtOutBuf: %ld\r\n", (ULONG)pMapComm->BtOutBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsInBuf: %ld\r\n", (ULONG)pMapComm->HsInBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsOutBuf: %ld\r\n", (ULONG)pMapComm->HsOutBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "UsbInBuf: %ld\r\n", (ULONG)pMapComm->UsbInBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "UsbOutBuf: %ld\r\n", (ULONG)pMapComm->UsbOutBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "UsbPollBuf: %ld\r\n", (ULONG)pMapComm->UsbPollBuf.Buf - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtDeviceCnt: %ld\r\n", (ULONG)&(pMapComm->BtDeviceCnt) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtDeviceNameCnt: %ld\r\n", (ULONG)&(pMapComm->BtDeviceNameCnt) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsFlags: %ld\r\n", (ULONG)&(pMapComm->HsFlags) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsSpeed: %ld\r\n", (ULONG)&(pMapComm->HsSpeed) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsState: %ld\r\n", (ULONG)&(pMapComm->HsState) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "UsbState: %ld\r\n", (ULONG)&(pMapComm->UsbState) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsMode: %ld\r\n", (ULONG)&(pMapComm->HsMode) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtDataMode: %ld\r\n", (ULONG)&(pMapComm->BtDataMode) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsDataMode: %ld\r\n", (ULONG)&(pMapComm->HsDataMode) - (ULONG)pMapComm);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "HsDataMode = %d\r\n", pMapComm->HsDataMode);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);

    sprintf((char *)Buffer, "BtDataMode = %d\r\n", pMapComm->BtDataMode);
    Length = strlen((char *)Buffer);
    LStatus = pMapLoader->pFunc(WRITE, &Handle, Buffer, &Length);
/*

  UBYTE          Spare1;
*/
    //close file
    LStatus = pMapLoader->pFunc(CLOSE, &Handle, NULL, NULL);
  }
}
#endif
