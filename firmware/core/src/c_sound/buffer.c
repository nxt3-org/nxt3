#include <malloc.h>
#include "c_sound/buffer.h"

void buffer_init(buffer_t *buffer) {
    buffer->data     = NULL;
    buffer->length   = 0;
    buffer->capacity = 0;
}

void buffer_delete(buffer_t *buffer) {
    free(buffer->data);
    buffer->data     = NULL;
    buffer->length   = 0;
    buffer->capacity = 0;
}

float *buffer_grow_f32(buffer_t *buffer, uint32_t minCapacity) {
    return buffer_grow(buffer, minCapacity, sizeof(float));
}

uint8_t *buffer_grow_u8(buffer_t *buffer, uint32_t minCapacity) {
    return buffer_grow(buffer, minCapacity, sizeof(uint8_t));
}

void *buffer_grow(buffer_t *buffer, uint32_t minCapacity, uint32_t elemSize) {
    uint32_t total = minCapacity * elemSize;
    if (buffer->capacity < total) {
        uint8_t *new = realloc(buffer->data, total);
        if (new == NULL) {
            return NULL;
        } else {
            buffer->data     = new;
            buffer->capacity = total;
        }
    }
    return buffer->data;
}
