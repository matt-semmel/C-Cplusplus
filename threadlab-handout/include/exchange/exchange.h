#pragma once
#include <exchange/message_circular_buffer.h>

typedef struct _exchange {
    size_t clients;
    message_buffer_t **buffer;
    pthread_cond_t *full; // Maybe use these to wait for an event? Check the handout
    pthread_cond_t *empty; // Maybe use these to wait for an event? Check the handout
} exchange_t;

typedef struct _exchange_config {

    size_t maximum_clients;
    size_t buffer_slots;
} exchange_config_t;

void init_exchange(exchange_config_t config);
void delete_exchange();
void exchange_quit();
size_t send_message(message_t *message);
message_t *get_message(size_t id, message_t *message);
void *exchange_thread(void *data);
