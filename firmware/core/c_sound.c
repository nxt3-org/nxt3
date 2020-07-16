//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: c_sound.c                                     $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/c_soun $
//
// Platform        C
//

#include  <stdlib.h>
#include  <string.h>
#include <hal_sound.h>
#include <hal_general.h>
#include  "stdconst.h"
#include  "modules.h"
#include  "c_sound.iom.h"
#include  "c_loader.iom.h"
#include  "c_sound.h"
#include "c_sound_adpcm.h"
#include  <stdio.h>

static    IOMAPSOUND   IOMapSound;
static    VARSSOUND    VarsSound;
static    HEADER       **pHeaders;

const     HEADER       cSound =
{
  0x00080001L,
  "Sound",
  cSoundInit,
  cSoundCtrl,
  cSoundExit,
  (void *)&IOMapSound,
  (void *)&VarsSound,
  (UWORD)sizeof(IOMapSound),
  (UWORD)sizeof(VarsSound),
  0x0000                      //Code size - not used so far
};


UWORD     cSoundFile(UBYTE Cmd,UBYTE *pFile,UBYTE *pData,ULONG *pLng)
{
  return (pMapLoader->pFunc(Cmd,pFile,pData,pLng));
}


void      cSoundInit(void* pHeader)
{
  pHeaders                    = pHeader;
  IOMapSound.Flags           &= ~SOUND_UPDATE;
  IOMapSound.Flags           &= ~SOUND_RUNNING;
  IOMapSound.State            =  SOUND_IDLE;
  IOMapSound.Mode             =  SOUND_ONCE;
  IOMapSound.Volume           =  SOUNDVOLUMESTEPS;
  IOMapSound.SampleRate       =  0;
  IOMapSound.SoundFilename[0] =  0;
  VarsSound.BufferIn          =  0;
  VarsSound.BufferOut         =  0;
  if (!Hal_Sound_RefAdd())
      Hal_General_AbnormalExit("Cannot initialize sound output");
}

void      cSoundCtrl(void)
{
  static  UWORD FileFormat;
  static  UBYTE SoundFilename[FILENAME_LENGTH + 1];
  UWORD   Handle;
  ULONG   Length;
  UBYTE   Header[FILEHEADER_LENGTH];
  UBYTE   In,Out,Tmp;

  Hal_Sound_Tick();

  In  = VarsSound.BufferIn;
  Out = VarsSound.BufferOut;

  if ((IOMapSound.Flags & SOUND_UPDATE))
  {
// Check if valid update
    if (!(SOUND_TONE & IOMapSound.Mode))
    {
      Handle = pMapLoader->pFunc(FINDFIRST,IOMapSound.SoundFilename,SoundFilename,&Length);
      if (!(Handle & 0x8000))
      {
        pMapLoader->pFunc(CLOSE,(UBYTE*)&Handle,NULL,NULL);
      }
      else
      {
        IOMapSound.Flags   &= ~SOUND_UPDATE;
      }
    }
    if ((IOMapSound.Flags & SOUND_UPDATE))
    {
// Check for open file
      if (!(VarsSound.File & 0x8000))
      {
        cSoundFile(CLOSE,(UBYTE*)&VarsSound.File,NULL,NULL);
        VarsSound.File = 0x8000;
      }

      IOMapSound.Flags   &= ~SOUND_UPDATE;

      if ((SOUND_TONE & IOMapSound.Mode))
      {
        Hal_Sound_StartTone(IOMapSound.Freq, IOMapSound.Duration, IOMapSound.Volume);
        IOMapSound.State  = SOUND_FREQ;
      }
      else
      {
        if (IOMapSound.Flags & SOUND_RUNNING)
        {
          Hal_Sound_Stop();
          IOMapSound.Flags  &= ~SOUND_RUNNING;
        }
        VarsSound.File = pMapLoader->pFunc(OPENREAD,SoundFilename,NULL,&Length);
        if (!(VarsSound.File & 0x8000))
        {
          Length = FILEHEADER_LENGTH;
          pMapLoader->pFunc(READ,(UBYTE*)&VarsSound.File,Header,&Length);
          if (Length == FILEHEADER_LENGTH)
          {
            FileFormat = ((UWORD)Header[0] << 8) + (UWORD)Header[1];

            if (FILEFORMAT_SOUND == (FileFormat & 0xFF00))
            {
              if (IOMapSound.SampleRate)
              {
                VarsSound.SampleRate  = IOMapSound.SampleRate;
                IOMapSound.SampleRate = 0;
              }
              else
              {
                VarsSound.SampleRate  = ((UWORD)Header[4] << 8) + (UWORD)Header[5];
              }
              Length = BUFFER_SIZE(FileFormat);
              pMapLoader->pFunc(READ,(UBYTE*)&VarsSound.File,VarsSound.Buffer[In],&Length);
              VarsSound.Length[In] = (UWORD)Length;
              if (FileFormat == FILEFORMAT_SOUND_COMPRESSED)
                  cSoundDecodeAdpcm(In);
              In++;
              if (In >= SOUNDBUFFERS)
              {
                In = 0;
              }
              IOMapSound.State  = SOUND_BUSY;
              VarsSound.AdpcmState = ADPCM_START;
            }
            else
            {
              if (FILEFORMAT_MELODY == FileFormat)
              {
                Length = SOUNDBUFFERSIZE;
                pMapLoader->pFunc(READ,(UBYTE*)&VarsSound.File,VarsSound.Buffer[In],&Length);
                VarsSound.Length[In] = (UWORD)Length;
                In++;
                if (In >= SOUNDBUFFERS)
                {
                  In = 0;
                }
                IOMapSound.State  = SOUND_BUSY;
              }
              else
              {
                pMapLoader->pFunc(CLOSE,(UBYTE*)&VarsSound.File,NULL,NULL);
              }
            }
          }
        }
      }
    }
  }

  switch (IOMapSound.State)
  {
    case SOUND_BUSY :
    {
      IOMapSound.Flags |= SOUND_RUNNING;
      if (In != Out)
      {
        if ((FILEFORMAT_SOUND == FileFormat) || (FILEFORMAT_SOUND_COMPRESSED == FileFormat))
        {
          int result = Hal_Sound_StartPcm(VarsSound.Buffer[Out],
                                          VarsSound.Length[Out],
                                          VarsSound.SampleRate,
                                          IOMapSound.Volume);
          if (result == SOUND_RESULT_SENT)
          {
            Out++;
            if (Out >= SOUNDBUFFERS)
            {
              Out = 0;
            }
          } else if (result == SOUND_RESULT_ERROR) {
              fputs("ERR: cannot send pcm\n", stderr);
          }
        }
        else
        {
          int result = Hal_Sound_StartMelody(VarsSound.Buffer[Out],
                                             VarsSound.Length[Out],
                                             IOMapSound.Volume);
          if (result == SOUND_RESULT_SENT)
          {
            Out++;
            if (Out >= SOUNDBUFFERS)
            {
              Out = 0;
            }
          } else if (result == SOUND_RESULT_ERROR) {
              fputs("ERR: cannot send melody\n", stderr);
          }
        }
      }

      Tmp = In;
      Tmp++;
      if (Tmp >= SOUNDBUFFERS)
      {
        Tmp = 0;
      }

      if (Tmp != Out)
      {
        Tmp++;
        if (Tmp >= SOUNDBUFFERS)
        {
          Tmp = 0;
        }
        if (Tmp != Out)
        {
          Length = BUFFER_SIZE(FileFormat);
          Handle = cSoundFile(READ,(UBYTE*)&VarsSound.File,VarsSound.Buffer[In],&Length);
          if ((Handle & 0x8000))
          {
            Length = 0L;
          }
          VarsSound.Length[In] = (UWORD)Length;
          if (VarsSound.Length[In] == 0)
          {
            if (SOUND_LOOP == IOMapSound.Mode)
            {
              if (!(IOMapSound.Flags & SOUND_UPDATE))
              {
                cSoundFile(CLOSE,(UBYTE*)&VarsSound.File,NULL,NULL);
                VarsSound.File = cSoundFile(OPENREAD,SoundFilename,NULL,&Length);
                Length = FILEHEADER_LENGTH;
                cSoundFile(READ,(UBYTE*)&VarsSound.File,Header,&Length);
                Length = BUFFER_SIZE(FileFormat);
                cSoundFile(READ,(UBYTE*)&VarsSound.File,VarsSound.Buffer[In],&Length);
                VarsSound.Length[In] = (UWORD)Length;
                VarsSound.AdpcmState = ADPCM_START;
              }
            }
          }
          if (VarsSound.Length[In] != 0)
          {
            if (FileFormat == FILEFORMAT_SOUND_COMPRESSED)
              cSoundDecodeAdpcm(In);
            In++;
            if (In >= SOUNDBUFFERS)
            {
              In = 0;
            }
          }
          if (VarsSound.Length[Out] == 0)
          {
            if (!(VarsSound.File & 0x8000))
            {
              pMapLoader->pFunc(CLOSE,(UBYTE*)&VarsSound.File,NULL,NULL);
              VarsSound.File = 0x8000;
            }
            IOMapSound.Flags &= ~SOUND_RUNNING;
            IOMapSound.State  = SOUND_IDLE;
          }
        }
      }
    }
    break;

    case SOUND_FREQ :
    {
      IOMapSound.Flags |= SOUND_RUNNING;
      if (Hal_Sound_ToneFinished())
      {
        if (SOUND_LOOP & IOMapSound.Mode)
        {
          Hal_Sound_StartTone(IOMapSound.Freq, IOMapSound.Duration, IOMapSound.Volume);
        }
        else
        {
          IOMapSound.Flags &= ~SOUND_RUNNING;
          IOMapSound.State  = SOUND_IDLE;
        }
      }
    }
    break;

    case SOUND_STOP :
    {
      Hal_Sound_Stop();
      if (!(VarsSound.File & 0x8000))
      {
        pMapLoader->pFunc(CLOSE,(UBYTE*)&VarsSound.File,NULL,NULL);
        VarsSound.File = 0x8000;
      }
      IOMapSound.Flags &= ~SOUND_RUNNING;
      IOMapSound.State  = SOUND_IDLE;
      Out = In;
    }
    break;

  }

  VarsSound.BufferIn  = In;
  VarsSound.BufferOut = Out;
}

void cSoundDecodeAdpcm(UBYTE bufferNo) {
    UWORD inLength = VarsSound.Length[bufferNo];
    if (inLength > SOUNDBUFFERSIZE/2)
        inLength = SOUNDBUFFERSIZE/2;
    UWORD outLength = inLength * 2;

    UBYTE tmpBuffer[SOUNDBUFFERSIZE];

    UBYTE *inPtr = &VarsSound.Buffer[bufferNo][0];
    UBYTE *outPtr = &tmpBuffer[0];
    while (inLength) {
        DecodeADPCM(*inPtr, outPtr, &VarsSound.AdpcmState);
        inPtr += 1;
        outPtr += 2;
        inLength--;
    }

    memcpy(&VarsSound.Buffer[bufferNo][0], tmpBuffer, outLength);
    VarsSound.Length[bufferNo] = outLength;
}

void      cSoundExit(void)
{
  Hal_Sound_RefDel();
}

