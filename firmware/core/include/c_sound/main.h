#ifndef SOUND_MAIN
#define SOUND_MAIN

#include "adpcm.h"
#include "buffer.h"
#include "resampler.h"
#include "sndfile.h"

/**
 * NXT volume range (0-4).
 */
#define SOUNDVOLUMESTEPS 4 // [steps]

/**
 * Sound module state.
 */
typedef struct {
    /**
     * Buffer with decoded data from previous state update.
     */
    buffer_t  decoded_buf;
    /**
     * Sndfile slot for melody/waveform playback.
     */
    sndfile_t file;
} sound_state_t;

/**
 * Initialize the sound module.
 * @param pHeaders List of all modules.
 */
static void sound_init(void *pHeaders);

/**
 * Update sound module state.
 *
 * Call this often (at least once in 10 ms should be good).
 */
static void sound_tick(void);

/**
 * Deinitialize the sound module.
 */
static void sound_exit(void);


/**
 * Convert current NXT volume to percent.
 * @return Integer in range 0-100.
 */
static uint8_t sound_get_volume(void);

/**
 * Handle changes in sound module's IOMap.
 */
static void sound_handle_iomap(void);

/**
 * Start tone playback as specified in IOMap.
 */
static void sound_start_tone(void);

/**
 * Start file playback as specified in IOMap.
 */
static void sound_start_file(void);

/**
 * Do internal bookkeeping (mostly shifting data around).
 */
static void sound_state_update(void);

/**
 * Perform file playback (mostly shifting data around).
 */
static void sound_do_file_playback(void);

/**
 * Perform tone playback (mostly waiting for finish).
 */
static void sound_do_tone_playback(void);

/**
 * Stop currently playing medium and return to idle state.
 */
static void sound_do_stop(void);

/**
 * Read data from currently opened file to an buffer.
 * @param file File to read the data from.
 * @param samples Buffer to read the data to.
 * @return Whether the operation succeeded or not (see file_state_t/sndfile for more information).
 */
static file_state_t sound_pipe_in(sndfile_t *file, buffer_t *samples);

/**
 * Write data to the appropriate kernel data pipe.
 * @param fmt Format of the data (FILEFORMAT_MELODY/FILEFORMAT_SOUND/FILEFORMAT_SOUND_COMPRESSED).
 * @param samples Buffer with data to write.
 * @return Whether the operation succeeded or not (see buffer_state_t for more information).
 */
static buffer_state_t sound_pipe_out(uint16_t fmt, buffer_t *samples);

/**
 * Prototype of a generic NXT3 HAL sound feed function.
 */
typedef int (*sound_pipe_t)(uint8_t *notes, uint32_t length, uint8_t volume);

#endif // SOUND_MAIN
