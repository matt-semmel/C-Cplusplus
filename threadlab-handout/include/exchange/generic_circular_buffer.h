#pragma once
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

typedef struct _circular_buffer {
    size_t (*is_full)(struct _circular_buffer *);
    size_t (*is_empty)(struct _circular_buffer *);
    void (*insert_item)(struct _circular_buffer *, const void*);
    void *(*remove_item)(struct _circular_buffer *, void*);
} circular_buffer_t;

typedef struct _circular_buffer_config {
    size_t max_elements;
    size_t size_of_data;
} circular_buffer_config_t;

circular_buffer_t *cb_init(const circular_buffer_config_t config);
void cb_delete(circular_buffer_t *);
size_t cb_is_full(circular_buffer_t *);
size_t cb_is_empty(circular_buffer_t *);
void cb_insert_item(circular_buffer_t *, const void*);
void* cb_remove_item(circular_buffer_t *, void*);

