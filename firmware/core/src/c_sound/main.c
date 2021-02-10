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

#include <hal_sound.h>
#include <hal_general.h>
#include <hal_filesystem.h>
#include "stdconst.h"
#include "modules.h"
#include "c_sound/main.h"
#include "c_sound/iomap.h"
#include "c_sound/buffer.h"

static IOMAPSOUND    IOMapSound;
static sound_state_t VarsSound;

const HEADER cSound = {
    0x00080001L,
    "Sound",
    sound_init,
    sound_tick,
    sound_exit,
    (void *) &IOMapSound,
    (void *) &VarsSound,
    sizeof(IOMapSound),
    sizeof(VarsSound),
    0x0000 // Code size - not used so far
};

void sound_init(__attribute__((unused)) void *pHeaders) {
    if (!Hal_Sound_RefAdd())
        Hal_General_AbnormalExit("Cannot initialize sound output");

    if (!Hal_Fs_RefAdd())
        Hal_General_AbnormalExit("Cannot initialize fs module");

    // iomap
    IOMapSound.Flags      = 0x00;
    IOMapSound.State      = SOUND_IDLE;
    IOMapSound.Mode       = SOUND_ONCE;
    IOMapSound.Volume     = SOUNDVOLUMESTEPS;
    IOMapSound.SampleRate = 0;
    IOMapSound.SoundFilename[0] = 0;

    // vars
    if (!sndfile_init(&VarsSound.file))
        Hal_General_AbnormalExit("Cannot initialize sndfile module");
    buffer_init(&VarsSound.decoded_buf);
}

void sound_tick(void) {
    sound_handle_iomap();
    sound_state_update();
}

void sound_exit(void) {
    buffer_delete(&VarsSound.decoded_buf);
    sndfile_delete(&VarsSound.file);
    Hal_Sound_Stop();
    Hal_Sound_RefDel();
    Hal_Fs_RefDel();
}

void sound_handle_iomap(void) {
    // skip if not update requested
    if ((IOMapSound.Flags & SOUND_UPDATE) != 0) {
        IOMapSound.Flags &= ~SOUND_UPDATE;

        // determine action
        if ((IOMapSound.Mode & SOUND_TONE) != 0) {
            sound_start_tone();
        } else {
            sound_start_file();
        }
    }
}

void sound_start_tone(void) {
    sndfile_close(&VarsSound.file);
    Hal_Sound_SendTone(IOMapSound.Freq, IOMapSound.Duration, sound_get_volume());
    IOMapSound.State = SOUND_FREQ;
}

void sound_start_file(void) {
    file_state_t state;
    // file open/replace is atomic here
    state = sndfile_open(&VarsSound.file,
                         (const char *) IOMapSound.SoundFilename,
                         (sample_rate_t) IOMapSound.SampleRate,
                         (sample_rate_t) Hal_Sound_SupportedSampleRate(),
                         SOUND_BUFFER_SIZE);

    if (state != FILE_OK)
        return;

    // clean samplerate request if necessary
    if (VarsSound.file.fmt == FILEFORMAT_SOUND || VarsSound.file.fmt == FILEFORMAT_SOUND_COMPRESSED)
        IOMapSound.SampleRate = 0;

    // proceed with loading
    IOMapSound.Flags &= ~SOUND_RUNNING;

    // start and schedule upload if ok
    if (sound_pipe_in(&VarsSound.file, &VarsSound.decoded_buf) == FILE_OK) {
        IOMapSound.State = SOUND_BUSY;
    } else {
        IOMapSound.State = SOUND_STOP;
    }
}

void sound_state_update(void) {
    switch (IOMapSound.State) {
    case SOUND_BUSY:
        sound_do_file_playback();
        break;

    case SOUND_FREQ:
        sound_do_tone_playback();
        break;

    case SOUND_STOP:
        sound_do_stop();
        break;
    }
    Hal_Sound_Tick();
}

void sound_do_file_playback(void) {
    IOMapSound.Flags |= SOUND_RUNNING;

    buffer_state_t bufstate  = BUFFER_OK;
    file_state_t   filestate = FILE_OK;
    // feed buffers until either end is busy
    while (true) {
        // try sending data to kernel
        if (VarsSound.decoded_buf.length > 0) {
            bufstate = sound_pipe_out(VarsSound.file.fmt, &VarsSound.decoded_buf);

            if (bufstate == BUFFER_BUSY || bufstate == BUFFER_ERROR)
                break;
        }

        // try loading new data from file
        filestate = sound_pipe_in(&VarsSound.file, &VarsSound.decoded_buf);

        if (filestate == FILE_EOF || filestate == FILE_ERROR)
            break;
    }

    // stopped due to error
    if (bufstate == BUFFER_ERROR || filestate == FILE_ERROR) {
        IOMapSound.State = SOUND_STOP;

    } else if (filestate == FILE_EOF) { // stopped due to end of file
        // looping -> reload
        if (IOMapSound.Mode == SOUND_LOOP) {
            // try to reload the file
            if (sndfile_rewind(&VarsSound.file) != FILE_OK
                || sound_pipe_in(&VarsSound.file, &VarsSound.decoded_buf) != FILE_OK) {
                IOMapSound.State = SOUND_STOP;
            }
        } else if (!Hal_Sound_IsFinished()) {
            // waiting for playback finish
        } else {
            // playback finished, signal readiness
            IOMapSound.State = SOUND_STOP;
        }
    }
}


file_state_t sound_pipe_in(sndfile_t *file, buffer_t *samples) {
    // try loading new data from file
    if (file->fmt == FILEFORMAT_MELODY) {
        return sndfile_read_melody(file, samples);
    } else {
        return sndfile_read_wave(file, samples);
    }
}


buffer_state_t sound_pipe_out(uint16_t fmt, buffer_t *samples) {
    sound_pipe_t send_pipe = fmt == FILEFORMAT_MELODY
                             ? &Hal_Sound_SendMelody
                             : &Hal_Sound_SendPcm;

    int result = send_pipe(samples->data,
                           samples->length,
                           sound_get_volume());
    if (result >= 0) {
        // ok, send data to fifo, mark tmp buffer as free
        samples->length = 0;
        return BUFFER_OK;
    } else if (result == SOUND_RESULT_BUSY) {
        // ok, fifo filled
        return BUFFER_BUSY;
    } else if (result == SOUND_RESULT_ERROR) {
        // ko, something bad happened
        return BUFFER_ERROR;
    } else {
        // unknown state
        return BUFFER_ERROR;
    }
}

void sound_do_tone_playback(void) {
    IOMapSound.Flags |= SOUND_RUNNING;
    if (Hal_Sound_IsFinished()) {
        if ((IOMapSound.Mode & SOUND_LOOP) != 0) {
            Hal_Sound_SendTone(IOMapSound.Freq, IOMapSound.Duration, sound_get_volume());
        } else {
            IOMapSound.Flags &= ~SOUND_RUNNING;
            IOMapSound.State = SOUND_IDLE;
        }
    }
}

void sound_do_stop(void) {
    Hal_Sound_Stop();
    sndfile_close(&VarsSound.file);
    IOMapSound.Flags &= ~SOUND_RUNNING;
    IOMapSound.State = SOUND_IDLE;
}

uint8_t sound_get_volume(void) {
    if (IOMapSound.Volume == 0) {
        return 0;
    } else if (IOMapSound.Volume == 1) {
        return 25;
    } else if (IOMapSound.Volume == 2) {
        return 50;
    } else if (IOMapSound.Volume == 3) {
        return 75;
    } else {
        return 100;
    }
}
