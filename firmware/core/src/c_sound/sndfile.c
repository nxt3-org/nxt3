#include <string.h>
#include "stdconst.h"
#include "modules.h"
#include "c_sound/sndfile.h"

bool sndfile_init(sndfile_t *file) {
    file->handle      = NOMOREHANDLES;
    file->fmt         = 0;
    file->rate        = 0;
    file->out_rate    = 0;
    file->input_block = 0;
    file->adpcm       = ADPCM_START;
    if (!resampler_init(&file->resampler))
        return false;
    buffer_init(&file->file_temp_buffer);
    buffer_init(&file->resampler_in_buffer);
    buffer_init(&file->resampler_out_buffer);
    return true;
}

file_state_t
sndfile_open(sndfile_t *file, const char *path, sample_rate_t in_rate_rq, sample_rate_t out_rate, uint32_t bufsize) {
    errhnd_t hnd;
    uint16_t fmt, file_rate;

    file_state_t state = sndfile_probe(path, &hnd, &fmt, &file_rate);
    if (state != FILE_OK)
        return FILE_ERROR;

    sample_rate_t final_input_rate = (in_rate_rq != 0) ? in_rate_rq : file_rate;
    // fail on out-of-bounds rates (prevent targetted attacks - overflow/large malloc)
    if (final_input_rate < SNDFILE_SAMPLERATE_MIN || final_input_rate > SNDFILE_SAMPLERATE_MAX)
        return FILE_ERROR;

    sndfile_close(file);
    file->handle   = hnd;
    file->fmt      = fmt;
    file->rate     = final_input_rate;
    file->out_rate = out_rate;
    if (fmt == FILEFORMAT_SOUND || fmt == FILEFORMAT_SOUND_COMPRESSED) {
        file->input_block = sndfile_calculate_inblock(file->rate, file->out_rate,
                                                      fmt == FILEFORMAT_SOUND_COMPRESSED, bufsize);
        file->adpcm       = ADPCM_START;
        resampler_reset(&file->resampler);
    } else {
        file->input_block = bufsize;
    }
    return FILE_OK;
}

file_state_t sndfile_probe(const char *path, errhnd_t *pHnd, uint16_t *pFmt, uint16_t *pRate) {
    fserr_t  err;
    uint32_t length;
    NXTFILE  header;

    // find file
    *pHnd = Hal_Fs_Locate((const char *) path, NULL, NULL);
    if (FS_ISERR(*pHnd))
        goto on_error;

    // open it
    err = Hal_Fs_OpenRead(*pHnd);
    if (FS_ISERR(err))
        goto on_error_with_cleanup;

    // read header
    length = FILEHEADER_LENGTH;
    err    = Hal_Fs_Read(*pHnd, &header, &length);
    if (FS_ISERR(err) || length != FILEHEADER_LENGTH)
        goto on_error_with_cleanup;

    // verify format
    *pFmt = (header.Base.FormatMsb << 8) | header.Base.FormatLsb;
    if (*pFmt != FILEFORMAT_MELODY &&
        *pFmt != FILEFORMAT_SOUND &&
        *pFmt != FILEFORMAT_SOUND_COMPRESSED) {
        goto on_error_with_cleanup;
    }

    if (*pFmt == FILEFORMAT_SOUND || *pFmt == FILEFORMAT_SOUND_COMPRESSED) {
        *pRate = (header.Sound.SampleRateMsb << 8) | header.Sound.SampleRateLsb;
    } else {
        *pRate = 0;
    }
    return FILE_OK;
on_error_with_cleanup:
    Hal_Fs_Close(*pHnd);
on_error:
    *pHnd = NOMOREHANDLES;
    return FILE_ERROR;
}


void sndfile_close(sndfile_t *file) {
    if (!FS_ISERR(file->handle)) {
        Hal_Fs_Close(file->handle);
        file->handle = NOMOREHANDLES;
    }
}

void sndfile_delete(sndfile_t *file) {
    sndfile_close(file);
    resampler_delete(&file->resampler);
    buffer_delete(&file->file_temp_buffer);
    buffer_delete(&file->resampler_in_buffer);
    buffer_delete(&file->resampler_out_buffer);
}

file_state_t sndfile_rewind(sndfile_t *file) {
    fserr_t err = Hal_Fs_Seek(file->handle, FILEHEADER_LENGTH, SEEK_FROMSTART);
    if (FS_ISERR(err)) {
        return FILE_ERROR;
    } else {
        resampler_reset(&file->resampler);
        file->adpcm = ADPCM_START;
        return FILE_OK;
    }
}

file_state_t sndfile_read_melody(sndfile_t *file, buffer_t *notes) {
    if (file->fmt != FILEFORMAT_MELODY)
        return FILE_ERROR;
    return sndfile_read_block(file, notes, file->input_block);
}

file_state_t sndfile_read_wave(sndfile_t *file, buffer_t *samples) {
    if (file->fmt != FILEFORMAT_SOUND && file->fmt != FILEFORMAT_SOUND_COMPRESSED)
        return FILE_ERROR;

    bool needsAdpcm    = file->fmt == FILEFORMAT_SOUND_COMPRESSED;
    bool needsResample = file->out_rate != file->rate;

    buffer_t *file_buffer = needsAdpcm ? &file->file_temp_buffer : samples;

    file_state_t state = sndfile_read_block(file, file_buffer, file->input_block);
    if (state != FILE_OK)
        return state; // skip eof & error states

    // decompress if needed
    if (needsAdpcm) {
        if (adpcm_decode(file_buffer, samples, &file->adpcm) != BUFFER_OK)
            return FILE_ERROR;
    }

    // resample if needed
    if (needsResample) {
        if (resampler_convert(&file->resampler, samples,
                              &file->resampler_in_buffer,
                              &file->resampler_out_buffer,
                              file->rate, file->out_rate) != BUFFER_OK)
            return FILE_ERROR;
    }
    return FILE_OK;
}

file_state_t sndfile_read_block(sndfile_t *file, buffer_t *output, uint32_t length) {
    // prepare buffer
    if (!buffer_grow_u8(output, length))
        return FILE_ERROR;

    // do the read
    output->length = length;
    fserr_t err = Hal_Fs_Read(file->handle, output->data, &output->length);
    // how did it go?
    if (err == SUCCESS) {
        return FILE_OK; // ok, full payload
    } else if (err == ENDOFFILE && output->length != 0) {
        return FILE_OK; // ok, truncated payload
    } else if (err == ENDOFFILE && output->length == 0) {
        return FILE_EOF; // eof, no more data
    } else {
        return FILE_ERROR; // unknown state
    }
}

uint32_t sndfile_calculate_inblock(sample_rate_t rate_in, sample_rate_t rate_out, bool adpcm, uint32_t bufsize) {
    uint32_t blocksize = bufsize;
    // scale by samplerate ratio (16kHz to 8kHz -> need 2x as many samples)
    blocksize = (blocksize * rate_in) / rate_out;
    // divide by two if having ADPCM (audio expands to 2x its size)
    if (adpcm)
        blocksize /= 2;
    return blocksize;
}
