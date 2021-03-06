//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_ioctrl.iom                                  $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_ioct $
//
// Platform        C
//

#ifndef   CIOCTRL_IOM
#define   CIOCTRL_IOM

#define   pMapIoCtrl ((IOMAPIOCTRL*)(pHeaders[ENTRY_IOCTRL]->pIOMap))  

enum
{
  POWERDOWN = 0x5A00,
  BOOT      = 0xA55A
};

typedef   struct
{
  UWORD   PowerOn;
}IOMAPIOCTRL;


#endif



