#ifndef SOUND_RESAMPLER
#define SOUND_RESAMPLER

#include "buffer.h"

#ifdef ENABLE_RESAMPLER

#include <samplerate.h>

/**
 * Internal resampler state.
 */
typedef struct {
    /**
     * libsamplerate state object.
     */
    SRC_STATE *src;
} resampler_t;

#else

/**
 * Internal resampler state.
 */
typedef struct {
    bool dummy;
} resampler_t;

#endif

typedef int sample_rate_t;

/**
 * Initialize this resampler instance.
 * @param rs Resampler state object.
 * @return True if initialization was successful, false otherwise.
 */
bool resampler_init(resampler_t *rs);

/**
 * Reset resampler so that it is able to process another independent stream.
 * @param rs Resampler state object.
 */
void resampler_reset(resampler_t *rs);

/**
 * Deinitialize this resampler instance.
 * @param rs Resampler state object.
 */
void resampler_delete(resampler_t *rs);

/**
 * Perform batch samplerate conversion.
 * @param rs Resampler state object.
 * @param in_out Input/output buffer with u8 pcm samples.
 * @param tmp_in Temporary buffer for storing pre-resampling f32 pcm samples.
 * @param tmp_out Temporary buffer for string post-resampling f32 pcm samples.
 * @param in_rate Input samplerate in Hz.
 * @param out_rate Output samplerate in Hz.
 * @return Whether the conversion was successful or not.
 */
buffer_state_t resampler_convert(resampler_t *rs, buffer_t *in_out,
                                 buffer_t *tmp_in, buffer_t *tmp_out,
                                 sample_rate_t in_rate, sample_rate_t out_rate);

#endif //SOUND_RESAMPLER
