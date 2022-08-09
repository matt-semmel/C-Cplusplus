/*
 * Starter producer/consumer program using a shared buffer.
 *
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <exchange/generic_circular_buffer.h>


#ifndef N_EXCHANGES
#define N_EXCHANGES 4
#endif

typedef struct _p_circular_buffer {
    circular_buffer_t public;
    size_t insert_idx;
    size_t remove_idx;
    size_t is_full;
    size_t size;
    size_t size_of_data;
    void *buffer;
    size_t armed_empty;
    size_t armed_full;

} p_circular_buffer_t;



static size_t next(p_circular_buffer_t *this, size_t idx);
static size_t is_full(circular_buffer_t *this);
static size_t is_empty(circular_buffer_t *this);
static void insert_item(circular_buffer_t *this, const void *d);
static void *remove_item(circular_buffer_t *this, void *d);
static size_t p_is_full(circular_buffer_t *this);
static size_t p_is_empty(circular_buffer_t *this);



/** PUBLIC INTERFACE **/
size_t cb_is_full(circular_buffer_t *buffer) {
    return buffer->is_full(buffer);
}
size_t cb_is_empty(circular_buffer_t *buffer){
    return buffer->is_empty(buffer);
}

void cb_insert_item(circular_buffer_t *buffer, const void *d){
    return buffer->insert_item(buffer, d);
}

void* cb_remove_item(circular_buffer_t *buffer, void *d){
    return buffer->remove_item(buffer, d);
}

circular_buffer_t *cb_init(const circular_buffer_config_t config) {

    p_circular_buffer_t *buffer = calloc(1, sizeof(p_circular_buffer_t));
    if(buffer == NULL) return NULL;
    buffer->size = config.max_elements;
    buffer->size_of_data = config.size_of_data;
    buffer->buffer = calloc(buffer->size, buffer->size_of_data);
    if(buffer->buffer == NULL) {
        free(buffer);
        return NULL;
    }
    buffer->public.is_full = is_full;
    buffer->public.is_empty = is_empty;
    buffer->public.insert_item = insert_item;
    buffer->public.remove_item = remove_item;

    return (void *)buffer;
}

void cb_delete(circular_buffer_t *public) {
    p_circular_buffer_t *this = (void*)public;
    if(this != NULL) free(this->buffer);
    free(this);
}

/** PRIVATE INTERFACE **/

static size_t next(p_circular_buffer_t *this, size_t idx) {
    return (idx+1)%this->size;
}

static size_t is_full(circular_buffer_t *public) {
    p_circular_buffer_t *this = (void*)public;
    if(this->armed_full>N_EXCHANGES) fprintf(stderr, "ERROR: Tried to check if full multiple times\n");
    this->armed_full++;
    return this->is_full;
}

static size_t is_empty(circular_buffer_t *public) {
    p_circular_buffer_t *this = (void *)public;
    if(this->armed_empty>N_EXCHANGES) {
        fprintf(stderr, "ERROR: Tried to check if empty multiple times\n");
    }
    this->armed_empty++;
    return !this->is_full && this->insert_idx==this->remove_idx;
}

static void insert_item(circular_buffer_t *public, const void *d) {
    p_circular_buffer_t *this = (void*)public;
    if(p_is_full(public)) {
        fprintf(stderr, "Tried to insert element in full buffer\n");
        return; // Drop data
    }

    // No pointer arithmetic :(
    void *address_to_insert = this->buffer + (this->insert_idx * this->size_of_data);
    memcpy(address_to_insert, d, this->size_of_data);
    
    size_t next_idx = next(this, this->insert_idx);
    if( next_idx == this->remove_idx ) this->is_full = 1;
    this->insert_idx = next_idx;
    this->armed_full = 0;
}
static void *remove_item(circular_buffer_t *public, void *d) {
    p_circular_buffer_t *this = (void*)public;

    if (p_is_empty(public)) {
        fprintf(stderr, "Tried to remove element from empty buffer\n");
        return NULL;
    }
    // No pointer arithmetic :(
    void *address_to_remove = this->buffer + (this->remove_idx * this->size_of_data);
    memcpy(d, address_to_remove, this->size_of_data);

    this->remove_idx = next(this, this->remove_idx);

    this->is_full = 0;
    this->armed_empty = 0;

    return d;
}


static size_t p_is_empty(circular_buffer_t *public) {
    p_circular_buffer_t *this = (void *)public;
    return !this->is_full && this->insert_idx==this->remove_idx;
}

static size_t p_is_full(circular_buffer_t *public) {
    p_circular_buffer_t *this = (void*)public;
    return this->is_full;
}
