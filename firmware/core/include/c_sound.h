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

#include <samplerate.h>
#include "hal_sound.h"
#include "c_sound_adpcm.h"

#define SAMPLERATE_MIN                2000      // Min sample rate [sps]
#define SAMPLERATE_DEFAULT            8000      // Default sample rate [sps]
#define SAMPLERATE_MAX                16000     // Max sample rate [sps]

#define SOUNDBUFFERS                  3         // [number]
#define SOUNDVOLUMESTEPS              4         // [steps]
#define SOUNDBUFFERSIZE_ADPCM         (SOUND_BUFFER_SIZE/2)
#define BUFFER_SIZE(format) ((format) == FILEFORMAT_SOUND_COMPRESSED ? SOUNDBUFFERSIZE_ADPCM : SOUND_BUFFER_SIZE)

typedef   struct
{
  UWORD   Length[SOUNDBUFFERS];
  UWORD   File;
  UWORD   SampleRate;
  UBYTE   Buffer[SOUNDBUFFERS][SOUND_BUFFER_SIZE];
  UBYTE   BufferIn;
  UBYTE   BufferOut;
  UBYTE   BufferTmp;
  ADPCM_State AdpcmState;
  SRC_STATE *ResamplerState;
}VARSSOUND;

void      cSoundInit(void* pHeaders);
void      cSoundCtrl(void);
void      cSoundExit(void);

void cSoundDecodeAdpcm(UBYTE bufferNo);


extern    const HEADER cSound;

#endif
