//Playback of compressed sound files. This additional feature is being brought to you under the following license.
//Please adhere to its terms.
//The original code includes minor changes to function correctly within the LEGO MINDSTORMS NXT embedded system,
//but the main architecture are implemented as within the original code.

//***********************************************************
//Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam, The
//Netherlands.
//
//                        All Rights Reserved
//
//Permission to use, copy, modify, and distribute this software and its
//documentation for any purpose and without fee is hereby granted,
//provided that the above copyright notice appear in all copies and that
//both that copyright notice and this permission notice appear in
//supporting documentation, and that the names of Stichting Mathematisch
//Centrum or CWI not be used in advertising or publicity pertaining to
//distribution of the software without specific, written prior permission.
//
//STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
//THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
//FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
//FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
//WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
//ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
//OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//******************************************************************/

//
// Programmer
//
// Date init       14.12.2004
//
// Reviser         $Author:: Dkandlun                                        $
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_sound_adpcm.r                               $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_sound_adpc $
//
// Platform        C
//

#include  "c_sound/adpcm.h"

static int16_t IndexTable[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static int16_t StepsizeTable[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void adpcm_decode_raw(uint32_t inBytes, const uint8_t *input, uint8_t *output, adpcm_state_t *pState) {
    int16_t step;              // Stepsize
    int16_t valprev;           // Virtual previous output value
    int16_t index;             // Current step change index

    valprev = pState->Valprev;
    index   = pState->Index;
    step    = StepsizeTable[index];

    for (uint8_t nibble = 0; nibble < inBytes * 2; nibble++) {

        // Step 1 - get the delta value
        uint8_t delta = input[nibble >> 1];
        if ((nibble & 1) == 0)
            delta >>= 4;
        delta &= 0x0F;

        index += IndexTable[delta]; // Step 2 - Find new index value (for later)
        if (index < 0) {
            index = 0;
        } else if (index > 88) {
            index = 88;
        }

        // Step 3 - Separate sign and magnitude
        bool negative = (delta & 0x8) != 0;
        delta = delta & 0x7;

        int16_t Vpdiff = step >> 3; // Step 4 - Compute difference and new predicted value

        if (delta & 4) Vpdiff += step;
        if (delta & 2) Vpdiff += step >> 1;
        if (delta & 1) Vpdiff += step >> 2;

        valprev += negative ? -Vpdiff : +Vpdiff;

        if (valprev > 255) // Step 5 - clamp output value
            valprev = 255;
        else if (valprev < 0)
            valprev = 0;

        step = StepsizeTable[index];        // Step 6 - Update step value
        output[nibble] = (uint8_t) valprev; // Step 7 - Output value
    }
    pState->Valprev = valprev;
    pState->Index   = index;
}

buffer_state_t adpcm_decode(buffer_t *input, buffer_t *output, adpcm_state_t *state) {
    uint8_t *in  = input->data;
    uint8_t *out = buffer_grow_u8(output, 2 * input->length);
    if (!out)
        return BUFFER_ERROR;
    adpcm_decode_raw(input->length, in, out, state);
    return BUFFER_OK;
}
