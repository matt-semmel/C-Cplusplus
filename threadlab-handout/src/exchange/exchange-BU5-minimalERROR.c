/*
 * Starter producer/consumer program using a shared buffer.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <exchange/message_circular_buffer.h>
#include <exchange/exchange.h>


// Maybe use this to manage the access to shared data?
static pthread_mutex_t  mutex;

// I belive this variable is accessed by many threads...
// Maybe I should think about protecting it?
static exchange_t *exchange = NULL;

/* This function initializes the exchange for you, 
    you shouldn't need to make any changes! */
// Not thread safe, because it's only called before any threads are created
void init_exchange(exchange_config_t config) {

    // Initialize the struct with the exchange state
    exchange = calloc(1, sizeof(exchange_t));
    if ( exchange == NULL ) return;

    // Store the maximum number of clients, we need this to access the arrays
    exchange->clients = config.maximum_clients;

    // Allocate an array of buffers, [0] for the producer, and [1].. to each client
    exchange->buffer = calloc(config.maximum_clients+1, sizeof(message_buffer_t));
    if ( exchange->buffer == NULL ) {
        free(exchange);
        return;
    }
    // Initialize each buffer
    for(int i = 0; i<config.maximum_clients+1; i++) {
        exchange->buffer[i] = mb_init(config.buffer_slots);
        // In case of error, cleanup
        if ( exchange->buffer[i] == NULL ) {
            for ( int j = 0; j<config.maximum_clients+1; j++ ) {
                free(exchange->buffer[i]);
            }
            free(exchange);
            return;
        }
    }

    // These conditional variables are very interesting...
    // I can unlock a mutex, and still block waiting for a condition to be triggered
    // Allocating an ARRAY of conditional variables. I'll need one per shared resource (aka buffer)
    exchange->full = calloc(config.maximum_clients+1, sizeof(pthread_cond_t));
    exchange->empty = calloc(config.maximum_clients+1, sizeof(pthread_cond_t));
    for(int i = 0; i<config.maximum_clients+1; i++) {
        pthread_cond_init(exchange->full+i, NULL);
        pthread_cond_init(exchange->empty+i, NULL);
    }

    // Not really using this, maybe I should?
    pthread_mutex_init(&mutex, NULL);

    return;
}

// Clean up variables, being called by main
// You shouldn't have to modify this
// Not thread safe, because it's only called after all threads quit
void delete_exchange() {
    // Just clean up everything...
    if (exchange == NULL) return;
    free(exchange->empty);
    free(exchange->full);
    for (size_t i = 0; i<exchange->clients+1; i++) {
        mb_delete(exchange->buffer[i]);
    }
    free(exchange->buffer);
    free(exchange);
}

// This is the thread being executed by each exchange thread! It's using shared resources, the buffers!
// Its job is to grab the next message from the producer's buffer (buffer[0])
//   and place it in the destination buffer (buffer[destination id])
void *exchange_thread(void *data) {
    message_t message;

    while (1) {

        // This is busy... If I keep looping this, I'm wasting CPU cycles.
        // Maybe I could check this once, and wait until something changes?

	pthread_mutex_lock(&mutex);
        while (mb_is_empty(exchange->buffer[0])) {
//correct syntax below. reading is important...
		pthread_cond_wait(exchange->empty, &mutex);
        }
//	commented out instead of deleted, when i signal here ttest throws errors.
//	pthread_cond_signal(exchange->empty);
	pthread_mutex_unlock(&mutex);

        // Hmmm... I wonder if another thread could have statched all messages before I got here!!!
        if (mb_remove_item(exchange->buffer[0], &message) == NULL) {
            fprintf(stderr, "Error reading from queue\n");
            // Careful if jumping out the window with the door locked!
	    // unlocks if error
	    pthread_mutex_unlock(&mutex);
            continue;
        }

        // When I get a message to quit, I'll quit!
        if(message.destination == 0 && message.quit == 1) {
            // Careful if jumping out the window with the door locked!
	    // unlocks on exit
	    pthread_mutex_unlock(&mutex);
            break;
        }

        // This is busy... If I keep looping this, I'm wasting CPU cycles.
        // Maybe I could check this once, and wait until something changes?

	pthread_mutex_lock(&mutex);
        while (mb_is_full(exchange->buffer[message.destination])) {
		pthread_cond_wait(exchange->full, &mutex);
        }
//	commented out instead of deleted. when i signal here, ttest throws errors.
//	pthread_cond_signal(exchange->full);
//	pthread_mutex_unlock(&mutex);
  //      pthread_cond_broadcast(exchange->full, &mutex);
        // Hmmm... I wonder if another thread could have filled the buffer before I got here!!!
//	pthread_mutex_lock(&mutex);
        mb_insert_item(exchange->buffer[message.destination], &message);
//	trying broadcast instead of signal... NOPE! gives more error than using signal...
	pthread_cond_signal(exchange->full);
//	pthread_cond_broadcast(exchange->full);
	pthread_mutex_unlock(&mutex);
    }
//    pthread_mutex_unlock(&mutex);
    return NULL;
}


// This function is called by any producer that wants to send a message!
// Its job is to place the message in the producer's buffer (buffer[0])
size_t send_message(message_t *message) {
    size_t ret=0;
    // This is busy... If I keep looping this, I'm wasting CPU cycles.
    // Maybe I could check this once, and wait until something changes?
    pthread_mutex_lock(&mutex);
    while ( mb_is_full(exchange->buffer[0]) ) {
	pthread_cond_wait(exchange->full, &mutex);
    }
    // Hmmm... I wonder if another thread could have filled the buffer before I got here!!!
    mb_insert_item(exchange->buffer[0], message);
//    pthread_cond_signal(exchange->full);
    pthread_cond_signal(exchange->empty);
    pthread_mutex_unlock(&mutex);

   return ret;
}

// This function is called by any client that wants to receive a message!
// Its job is to grab the next message in the client's buffer (buffer[id])
message_t *get_message(size_t id, message_t *message) {
    message_t *ret = NULL;
    // This is busy... If I keep looping this, I'm wasting CPU cycles.
    // Maybe I could check this once, and wait until something changes?
    pthread_mutex_lock(&mutex);
    while(mb_is_empty(exchange->buffer[id])) {
	pthread_cond_wait(exchange->empty, &mutex);
    }
    // Hmmm... I wonder if another thread could have statched all messages before I got here!!!
    ret = mb_remove_item(exchange->buffer[id], message);
    pthread_cond_signal(exchange->full);
    pthread_mutex_unlock(&mutex);

    return ret;
}
