#ifndef SOUND_ADPCM
#define SOUND_ADPCM

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "buffer.h"

/**
 * ADPCM decoder state.
 */
typedef struct {
    /**
     * Last emitted value.
     */
    int16_t Valprev;
    /**
     * ???
     */
    int16_t Index;
} adpcm_state_t;

/**
 * Initial ADPCM ValPrev value.
 */
#define INIT_PREV_VAL_ADPCM           0x7F

/**
 * Initial ADPCM Index vlaue.
 */
#define INIT_INDEX_ADPCM              20

/**
 * Initial state for adpcm_state_t.
 */
#define ADPCM_START ((adpcm_state_t){ .Valprev = INIT_PREV_VAL_ADPCM, .Index = INIT_INDEX_ADPCM })

/**
 * Decode ADPCM samples from a managed buffer.
 * @param input Input buffer with ADPCM data.
 * @param output Output buffer with u8 pcm samples (2x the length of the compressed stream).
 * @param state Persistent ADPCM state.
 * @returns True if the conversion was successful, false otherwise.
 */
extern buffer_state_t adpcm_decode(buffer_t *input, buffer_t *output, adpcm_state_t *state);

/**
 * Decode ADPCM samples from a raw buffer.
 * @param inBytes Number of bytes on input (output will be 2x this long).
 * @param input Input buffer with ADPCM data.
 * @param output Output buffer with u8 pcm samples.
 * @param pState Persistent ADPCM state.
 */
extern void adpcm_decode_raw(uint32_t inBytes, const uint8_t *input, uint8_t *output, adpcm_state_t *pState);

#endif // SOUND_ADPCM
