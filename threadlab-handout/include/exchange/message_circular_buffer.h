#pragma once
#include <exchange/generic_circular_buffer.h>


typedef circular_buffer_t message_buffer_t;

typedef struct _message {
    int source;         /* Source of the message */
    int destination;    /* Destination of the message */
    int line;           /* Line number in input file */
    int quit;           /* Nonzero if consumer should exit ("value" ignored) */
    char message[1000]; /* Message */
} message_t;


// Initialize buffer, returns a pointer to a shinny new buffer with `size` capacity
message_buffer_t *mb_init(size_t size);
// Destroy the buffer (free memory)
void mb_delete(message_buffer_t *buffer);
// Returns true if the buffer is full
size_t mb_is_full(message_buffer_t *);
// Returns true if the buffer is empty
size_t mb_is_empty(message_buffer_t *);
// Insert message into buffer, makes a copy of the struct
void mb_insert_item(message_buffer_t *, const message_t*);
// Removes and returns a message into the supplied buffer, makes a copy of the struct
message_t* mb_remove_item(message_buffer_t *, message_t*);
// Prints a message, in case we need to display it
void mb_print_util(message_t *message);
