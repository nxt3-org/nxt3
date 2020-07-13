//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_sound.c                                     $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_soun $
//
// Platform        C
//

#include  "stdconst.h"
#include  "m_sched.h"
#include  "d_sound.h"
#include  "d_sound_adpcm.h"
#include  <sys/types.h>
#include  <sys/mman.h>
#include  <sys/uio.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <memory.h>
#include  <time.h>

#define STATUS_READY 0
#define STATUS_BUSY  1
typedef struct {
  UBYTE status;
  UBYTE padding[3];
} sound_kmem;

#define CMD_PLAY 2
typedef struct {
  UBYTE cmd;
  UBYTE volume;
} sound_req_play;

#define CMD_BREAK 0
typedef struct {
  UBYTE cmd;
} sound_req_break;

#define CMD_TONE 1
typedef struct {
  UBYTE cmd;
  UBYTE volume;
  UWORD frequency;
  UWORD duration;
} sound_req_tone;

#define CMD_DATA 4
typedef struct {
  UBYTE cmd;
  UBYTE samples[];
} sound_req_data;

typedef struct {
  int                  soundFD;
  volatile sound_kmem *kernel;
  UWORD                melodyPointer;
  ADPCM_State          adpcm;
  UBYTE                adpcmDecompressed;
  UBYTE                volume;
  UBYTE                warningCount;
  UBYTE                decompressionBuffer[SOUNDBUFFERSIZE_ADPCM * 2];
  UBYTE                commandBuffer[SOUNDBUFFERSIZE + 1];
} EV3_SOUND_DATA;

static EV3_SOUND_DATA this;


void   dSoundInit(void) {
  this.melodyPointer = 0;
  this.adpcm = ADPCM_START;
  this.adpcmDecompressed = FALSE;
  this.volume = 50;
  this.warningCount = 0;
  memset(this.decompressionBuffer, 0, SOUNDBUFFERSIZE);

  this.soundFD = open("/dev/lms_sound", O_RDWR);
  if (this.soundFD < 0) {
    perror("EV3 SOUND: cannot open /dev/lms_sound");
    return;
  }

  this.kernel = mmap(0, sizeof(sound_kmem), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, this.soundFD, 0);
  if (this.kernel == MAP_FAILED) {
    this.kernel = NULL;
    perror("EV3 SOUND: cannot mmap /dev/lms_sound");
    return;
  }
}

static
SLONG  sendSoundRequest(void *buffer, size_t size) {
  if (this.soundFD < 0)
    return -1;

  SLONG bytes = pwrite(this.soundFD, buffer, size, 0);

  if (bytes < 0)
    perror("EV3 SOUND: cannot send request to kernel");

  return bytes;
}

static
SLONG  sendSoundData(UBYTE *samples, size_t size) {
  if (this.soundFD < 0)
    return -1;

  this.commandBuffer[0] = CMD_DATA;
  memcpy(this.commandBuffer + 1, samples, size);
  SLONG bytes = pwrite(this.soundFD, this.commandBuffer, size + 1, 0);

  if (bytes < 0)
    perror("EV3 SOUND: cannot send request to kernel");

  return bytes;
}

void   dSoundVolume(UBYTE Step) {
  if (Step > SOUNDVOLUMESTEPS)
    Step = SOUNDVOLUMESTEPS;

  this.volume = 2 * Step;
}


UBYTE  dSoundReady(void) {
  return this.kernel->status == STATUS_READY;
}


UBYTE  dSoundStart(UBYTE *Sound, UWORD Length, UWORD SampleRate, UBYTE Adpcm) {
  if (SampleRate != 8000 && this.warningCount == 0) {
    fputs("EV3 SOUND: warning: sampling rates other than 8000 Hz are not supported, sound will be distorted", stderr);
    this.warningCount++;
  }

  UBYTE *realBuffer;
  ULONG  realSize;

  if (Adpcm) {
    if (!this.adpcmDecompressed) {

      this.adpcm = ADPCM_START;
      for (int i = 0; i < Length; i++) {
        DecodeADPCM(Sound[i], &this.decompressionBuffer[2*i], &this.adpcm);
      }

      this.adpcmDecompressed = TRUE;
    }
    realBuffer = this.decompressionBuffer;
    realSize   = 2 * Length;
  } else {
    realBuffer = Sound;
    realSize   = Length;
  }

  if (this.kernel->status == STATUS_READY) {
    sound_req_play req = {
      .cmd    = CMD_PLAY,
      .volume = this.volume
    };

    if (sendSoundRequest(&req, sizeof(req)) < 0)
      return TRUE;

    this.kernel->status = STATUS_BUSY;
  }

  int bytes = sendSoundData(realBuffer, realSize);

  if (bytes == 0) {
    return FALSE;
  } else {
    this.adpcmDecompressed = FALSE;
    return TRUE;
  }
}

UBYTE  dSoundStop(void) {
  this.melodyPointer = 0;
  this.adpcmDecompressed = FALSE;

  sound_req_break req = { .cmd = CMD_BREAK };
  return sendSoundRequest(&req, sizeof(req)) >= 0;
}

UBYTE  dSoundTone(UBYTE *pMelody, UWORD Length, UBYTE Volume) {
  UBYTE Result = FALSE;

  if (this.kernel->status == STATUS_READY) {

    if (this.melodyPointer <= (Length - 4)) {
      UWORD Freq, mS;

      Freq    = (UWORD)pMelody[this.melodyPointer++] << 8;
      Freq   += (UWORD)pMelody[this.melodyPointer++];
      mS      = (UWORD)pMelody[this.melodyPointer++] << 8;
      mS     += (UWORD)pMelody[this.melodyPointer++];
      dSoundFreq(Freq, mS, Volume);

    } else {
      this.melodyPointer = 0;
      Result             = TRUE;
    }
  }

  return Result;
}


void  dSoundFreq(UWORD Freq, UWORD mS, UBYTE Step) {
  if (mS < DURATION_MIN)
    mS = DURATION_MIN;

  sound_req_tone req = {
    .cmd       = CMD_TONE,
    .volume    = 2 * Step,
    .frequency = Freq,
    .duration  = mS
  };

  sendSoundRequest(&req, sizeof(req));
}


void   dSoundExit(void) {
  dSoundStop();

  if (this.kernel) {
    if (munmap((void*) this.kernel, sizeof(sound_kmem)) < 0) {
      perror("EV3 SOUND: cannot munmap /dev/lms_sound");
    }
  }

  if (this.soundFD >= 0) {
    dSoundStop();
    if (close(this.soundFD) < 0) {
      perror("EV3 SOUND: cannot close /dev/lms_sound");
    }
  }

  this.soundFD = -1;
  this.kernel  = NULL;
}
