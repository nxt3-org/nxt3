#ifndef SOUND_BUFFER
#define SOUND_BUFFER

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Result enum for buffer processing functions.
 */
typedef enum {
    /**
     * Buffer was successfully sent/received/processed.
     */
    BUFFER_OK,
    /**
     * Buffer processing failed, but it is expected that this is only temporary (full FIFO etc).
     */
    BUFFER_BUSY,
    /**
     * Buffer processing failed with a nontrivial error.
     */
    BUFFER_ERROR
} buffer_state_t;

/**
 * Growable buffer for sound samples.
 */
typedef struct {
    /**
     * Buffer data. May be null if empty!
     */
    void     *data;
    /**
     * Number of active elements in this buffer (user-managed).
     */
    uint32_t length;
    /**
     * Capacity of the allocated array (internally managed).
     */
    uint32_t capacity;
} buffer_t;

/**
 * Initialize this growable buffer.
 * @param buffer Buffer to initialize.
 */
void buffer_init(buffer_t *buffer);

/**
 * Deinitialize this growable buffer.
 * @param buffer Buffer to deinitialize.
 */
void buffer_delete(buffer_t *buffer);

/**
 * Grow this buffer to the required capacity and return its data pointer.
 * @param buffer Buffer to access/grow.
 * @param minCapacity Required minimal capacity in (1 == one float, not one byte)
 * @return Buffer data pointer or NULL if allocation fails.
 */
float *buffer_grow_f32(buffer_t *buffer, uint32_t minCapacity);

/**
 * Grow this buffer to the required capacity and return its data pointer.
 * @param buffer Buffer to access/grow.
 * @param minCapacity Required minimal capacity in (1 == one byte)
 * @return Buffer data pointer or NULL if allocation fails.
 */
uint8_t *buffer_grow_u8(buffer_t *buffer, uint32_t minCapacity);

/**
 * Grow this buffer to the required capacity and return its data pointer.
 * @param buffer Buffer to access/grow.
 * @param minCapacity Required minimal capacity in (1 == one element)
 * @param elemSize Size of one element.
 * @return Buffer data pointer or NULL if allocation fails.
 */
void *buffer_grow(buffer_t *buffer, uint32_t minCapacity, uint32_t elemSize);

#endif //SOUND_BUFFER
