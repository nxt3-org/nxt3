#include "c_sound/resampler.h"

#ifdef ENABLE_RESAMPLER

bool resampler_init(resampler_t *rs) {
    rs->src = src_new(SRC_LINEAR, 1, NULL);
    return rs->src != NULL;
}

void resampler_reset(resampler_t *rs) {
    src_reset(rs->src);
}

void resampler_delete(resampler_t *rs) {
    rs->src = src_delete(rs->src);
}

buffer_state_t
resampler_convert(resampler_t *rs, buffer_t *in_out,
                  buffer_t *tmp_in, buffer_t *tmp_out,
                  sample_rate_t in_rate, sample_rate_t out_rate) {

    // alloc intermediate float arrays
    uint32_t inLen    = in_out->length;
    uint32_t outLen = (inLen * out_rate + in_rate - 1) / in_rate;
    uint8_t  *inoutU8 = buffer_grow_u8(in_out, outLen);
    float    *inF32   = buffer_grow_f32(tmp_in, inLen);
    float    *outF32  = buffer_grow_f32(tmp_out, outLen);

    // fail on oom
    if (!inF32 || !outF32 || !inoutU8)
        return BUFFER_ERROR;

    // convert int -> float
    for (int i = 0; i < inLen; i++) {
        inF32[i] = (inoutU8[i] - 127) / 128.0f;
    }

    // prepare for submission
    SRC_DATA batch = {
        .data_in = inF32,
        .data_out = outF32,
        .input_frames = inLen,
        .output_frames = outLen,
        .input_frames_used = 0,
        .output_frames_gen = 0,
        .end_of_input = false,
        .src_ratio = ((float) out_rate) / in_rate
    };

    // submit
    int result = src_process(rs->src, &batch);
    if (result != 0)
        return BUFFER_ERROR;

    in_out->length = batch.output_frames_gen;

    // convert float -> int
    for (int i = 0; i < in_out->length; i++) {
        float normed = outF32[i] * 128.0f + 127;
        if (normed > 255.0f) normed = 255.0f;
        if (normed < 0.0f) normed   = 0.0f;
        inoutU8[i] = normed;
    }

    return BUFFER_OK;
}


#else

bool resampler_init(resampler_t *rs) {
    return true;
}

void resampler_reset(resampler_t *rs) {}

void resampler_delete(resampler_t *rs) {}

buffer_state_t
resampler_convert(resampler_t *rs, buffer_t *in_out,
                  buffer_t *tmp_in, buffer_t *tmp_out,
                  sample_rate_t in_rate, sample_rate_t out_rate) {
    if (in_rate == out_rate)
        return BUFFER_OK;
    else
        return BUFFER_ERROR;
}

#endif
