//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: m_sched.h                                     $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/m_sche $
//
// Platform        C
//


/* Defines related to loader */
#define   MAX_HANDLES                   16

enum
{
  NOS_OF_AVR_OUTPUTS  = 4,
  NOS_OF_AVR_BTNS     = 4,
  NOS_OF_AVR_INPUTS   = 4
};

typedef   struct {
  UWORD   Battery;
} IOFROMAVR;

extern    IOFROMAVR IoFromAvr;

#ifdef INCLUDE_OS

IOFROMAVR InFromAvr;

#endif
