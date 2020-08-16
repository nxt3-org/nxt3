//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_sound.h                                     $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_soun $
//
// Platform        C
//


#ifndef   C_SOUND
#define   C_SOUND

#include "hal_sound.h"
#include "c_sound_adpcm.h"

typedef   struct
{
  UWORD   Length[SOUNDBUFFERS];
  UWORD   File;
  UWORD   SampleRate;
  UBYTE   Buffer[SOUNDBUFFERS][SOUNDBUFFERSIZE];
  UBYTE   BufferIn;
  UBYTE   BufferOut;
  UBYTE   BufferTmp;
  ADPCM_State AdpcmState;
}VARSSOUND;

void      cSoundInit(void* pHeaders);
void      cSoundCtrl(void);
void      cSoundExit(void);

void cSoundDecodeAdpcm(UBYTE bufferNo);

#define BUFFER_SIZE(format) ((format) == FILEFORMAT_SOUND_COMPRESSED ? SOUNDBUFFERSIZE_ADPCM : SOUNDBUFFERSIZE)

extern    const HEADER cSound;

#endif
