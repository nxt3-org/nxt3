#ifndef   D_SOUND_ADPCM
#define   D_SOUND_ADPCM

#include  "stdconst.h"
#include  "modules.h"

typedef   struct {
  SWORD Valprev;
  SWORD Index;
} ADPCM_State;

#define INIT_PREV_VAL_ADPCM           0x7F
#define INIT_INDEX_ADPCM              20
#define ADPCM_START ((ADPCM_State){ .Valprev = INIT_PREV_VAL_ADPCM, .Index = INIT_INDEX_ADPCM })

extern void DecodeADPCM(UBYTE Indata, UBYTE *Outdata, ADPCM_State *pState);

#endif
