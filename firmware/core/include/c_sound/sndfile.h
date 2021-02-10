#ifndef SOUND_SNDFILE
#define SOUND_SNDFILE

#include <hal_filesystem.h>
#include "adpcm.h"
#include "resampler.h"

/**
 * Minimum input file sample rate supported by sndfile.
 */
#define SNDFILE_SAMPLERATE_MIN  2000  // Min sample rate [sps]
/**
 * Maximum input file sample rate supported by sndfile.
 */
#define SNDFILE_SAMPLERATE_MAX 16000  // Max sample rate [sps]

/**
 * Result code for file-oriented functions.
 */
typedef enum {
    /**
     * Operation was performed (at least partially) successfully.
     */
    FILE_OK,
    /**
     * Operation could not complete because there are no remaining data in the file.
     */
    FILE_EOF,
    /**
     * Non-recoverable/non-trivial error occurred.
     */
    FILE_ERROR
} file_state_t;

/**
 * Represents a single file slot.
 */
typedef struct {
    /**
     * NXT handle of the sound file itself.
     */
    errhnd_t      handle;
    /**
     * Format code from file header (see FILEFORMAT_*).
     */
    uint16_t      fmt;
    /**
     * Functional input sampling rate (fusion from user request & file header).
     */
    uint16_t      rate;
    /**
     * Requested output sampling rate.
     */
    uint16_t      out_rate;
    /**
     * How many bytes to read from the file for each read request.
     */
    uint16_t      input_block;
    /**
     * Persistent ADPCM decoder state.
     */
    adpcm_state_t adpcm;
    /**
     * Persistent resampler state.
     */
    resampler_t   resampler;
    /**
     * Temporary buffer for ADPCM decoding.
     */
    buffer_t      file_temp_buffer;
    /**
     * Temporary buffer for resampling (contains input floats).
     */
    buffer_t      resampler_in_buffer;
    /**
     * Temporary buffer for resampling (contains output floats).
     */
    buffer_t      resampler_out_buffer;
} sndfile_t;

/**
 * Initialize a new sndfile slot.
 * @param file Slot to initialize.
 * @return True if the initialization was successful, false otherwise.
 */
bool sndfile_init(sndfile_t *file);

/**
 * Open a new file in the given slot.
 *
 * This function behaves sort-of atomically - it only closes the previous file if the
 * new file is certain to open successfully.
 *
 * @param file Slot where to open the file.
 * @param path NXT path to the file (not POSIX!).
 * @param in_rate_rq Input sampling rate override or 0 if not set.
 * @param out_rate Required output sampling rate.
 * @param bufsize Preferred output buffer size.
 * @return Whether the operation was successful (FILE_OK) or not (FILE_ERROR).
 */
file_state_t sndfile_open(sndfile_t *file, const char *path,
                          sample_rate_t in_rate_rq, sample_rate_t out_rate, uint32_t bufsize);

/**
 * Close the file in the given slot.
 * @param file Slot where to close the file.
 */
void sndfile_close(sndfile_t *file);

/**
 * Deinitialize the given sndfile slot.
 * @param file Slot to deinitialize.
 */
void sndfile_delete(sndfile_t *file);

/**
 * Rewind the file pointer so that the file can be played back from its start.
 * @param file Slot to rewind.
 * @return Whether the operation succeeded (FILE_OK) or failed (FILE_ERROR).
 */
file_state_t sndfile_rewind(sndfile_t *file);

/**
 * Read one block of melody data from this file.
 *
 * The file must have FILEFORMAT_MELODY in its header. If this is not
 * the case, FILE_ERROR is returned.
 *
 * @param file File slot to read the data from.
 * @param notes Managed output buffer for the RMD notes.
 * @return FILE_OK when some data was read, FILE_EOF when no more data can be read and FILE_ERROR on error.
 */
file_state_t sndfile_read_melody(sndfile_t *file, buffer_t *notes);

/**
 * Read one block of wave data from this file.
 *
 * The file must have FILEFORMAT_SOUND/FILEFORMAT_SOUND_COMPRESSED
 * in its header. If this is not the case, FILE_ERROR is returned.
 *
 * @param file File slot to read the data from.
 * @param samples Managed output buffer for the u8 pcm samples.
 * @return FILE_OK when some data was read, FILE_EOF when no more data can be read and FILE_ERROR on error.
 */
file_state_t sndfile_read_wave(sndfile_t *file, buffer_t *samples);

/**
 * Calculate optimal input block size in order to meet the optimal bufsize output block.
 * @param rate_in Input sampling rate.
 * @param rate_out Output sampling rate.
 * @param adpcm Whether the input stream is compressed using ADPCM.
 * @param bufsize Wanted output block size.
 * @return Optimal input block size.
 */
uint32_t sndfile_calculate_inblock(sample_rate_t rate_in, sample_rate_t rate_out, bool adpcm, uint32_t bufsize);

/**
 * Check whether this file is openable with sndfile.
 * @param path NXT path to the file (not POSIX).
 * @param pHnd Store the file handle. Must not be null!
 * @param pFmt Store the file format header here. Must not be null!
 * @param pRate Store the sampling rate from file header here. Must not be null!
 * @return FILE_OK if the filw was opened successfully, FILE_ERROR on error (the file will be closed automatically in that case).
 */
file_state_t sndfile_probe(const char *path, errhnd_t *pHnd, uint16_t *pFmt, uint16_t *pRate);

/**
 * Read a block of data from the given file.
 * @param file File slot to read the data from.
 * @param output Managed output buffer for the data.
 * @param length How mnay bytes to read.
 * @return FILE_OK when some data was read, FILE_EOF when no more data can be read and FILE_ERROR on error.
 */
file_state_t sndfile_read_block(sndfile_t *file, buffer_t *output, uint32_t length);

#endif //SOUND_SNDFILE
