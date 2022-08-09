/*
 * Starter producer/consumer program using a shared buffer.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <exchange/exchange.h>
#include <stdlib.h>
#include <sched.h>

// Number of clients executing
#ifndef N_CLIENTS
#define N_CLIENTS 10
#endif

// Number of exchanges executing
#ifndef N_EXCHANGES
#define N_EXCHANGES 4
#endif

// Define debug if not defined previously
#ifndef DEBUG
// Set to zero to disable debug
// Set to one to enable debug
#define DEBUG 1
#endif

// This will remove the PDEBUG calls if debug is disabled
#if DEBUG
#define PDEBUG(fmt, ...) printf("DEBUG("__FILE__": %d): "fmt, __LINE__, ##__VA_ARGS__)
#else
#define PDEBUG(fmt, ...) 
#endif


int  main(int argc, char *argv[]);
void *producer(void * arg);
void *consumer(void * arg);
void *exchange(void * arg);
void thread_sleep(unsigned int ms);
void *create_client(void *id);


int main(int argc, char * argv[])
{
    exchange_config_t config = {
        .maximum_clients = N_CLIENTS,
        .buffer_slots = 10
    };

    /* Initialize the exchange, make sure all needed data structures are 
         ready to be accessed before we start the thread */
    init_exchange(config);


    /* Launch the exchange thread, exchange_thread is not safe, you need to make changes! */
    pthread_t t_exchange[N_EXCHANGES];
    for (int i = 0; i<N_EXCHANGES; i++) {
        if (pthread_create(&t_exchange[i], NULL, exchange_thread, NULL) != 0) {
            fprintf(stderr, "Couldn't create consumer thread\n");
            return 1;
        }
    }

    /*
     * Make sure output appears right away.
     */
    setlinebuf(stdout);

    /*
     * Create a thread for the producer.
     */
    pthread_t t_producer;
    if (pthread_create(&t_producer, NULL, producer, argv[1]) != 0) {
        fprintf(stderr, "Couldn't create consumer thread\n");
        return 1;
    }

    /*
     * Create a thread for each consumer.
     */
    pthread_t client[N_CLIENTS];
    for(size_t i = 0; i<N_CLIENTS; i++) {
        if (pthread_create(client+i, NULL, consumer, (void*)(i+1)) != 0) {
            fprintf(stderr, "Couldn't create consumer thread\n");
            return 1;
        }
    }


    /* Wait until the producer runs out of data */
    if (pthread_join(t_producer, NULL) != 0) {
        fprintf(stderr, "Couldn't join with producer thread\n");
        return 1;
    }
    
    /* Send a message to all clients and exchanges to let them know the program has finished! */
    message_t msg = {0};
    for(size_t i = 0; i<N_CLIENTS; i++) {
        msg.source = 0;
        msg.destination = i+1;
        msg.quit=1;
        send_message(&msg);
    }
    for(size_t i = 0; i<N_EXCHANGES; i++) {

        msg.source = 0;
        msg.destination = 0;
        msg.quit=1;
        send_message(&msg);
    }

    /*
     * The producer has terminated.  Clean up the consumers, which might
     * not have terminated yet.
     */
    for(size_t i = 0; i<N_CLIENTS; i++) {
        if (pthread_join(client[i], NULL) != 0) {
            fprintf(stderr, "Couldn't join with consumer thread\n");
            return 1;
        }
    }

    /* Wait until the echange terminates */
    for (int i = 0; i<4; i++) {
        if (pthread_join(t_exchange[i], NULL) != 0) {
            fprintf(stderr, "Couldn't join with exchange thread\n");
            return 1;
        }
    }
    delete_exchange();

    return 0;
}



void *producer(void * arg)
{
    FILE *i_file;
    const char *path = arg;
    const char *default_path = "tests/test0.txt";
    if(path == NULL) {
        path = default_path;
    }
    i_file = fopen(path, "r");

    message_t msg;

    while (1) {

        // read from file
        int n_args = fscanf(i_file, "%d %d ", &msg.source, &msg.destination);
        int success = fgets(msg.message, 1000, i_file) != NULL;
        
        // check validity
        if ( n_args<2 || !success ) {
            PDEBUG("Producer out!\n");
            return NULL;
        }

        PDEBUG("Read %d, %s\n", n_args, success?"success":"fail");
        PDEBUG("%d %d %s\n", msg.source, msg.destination, msg.message);

        // send to exchange
        send_message(&msg);
    }

    return NULL;
}

void *consumer(void * arg)
{
    size_t id = (size_t)arg;
    message_t msg = {0};
    do {
        // Get message from exchange
        if (get_message(id, &msg) == NULL) {
            fprintf(stderr, "Fail consumer %ld\n", id);
        } else if (!msg.quit) {
            // print the message, unless it's a quit message, then quit
            printf("%ld:%s\n", id, msg.message);
        }
    } while (msg.quit == 0);
    return NULL;
}

void thread_sleep(unsigned int ms)
{
    struct timespec sleep_time;

    if (ms == 0)
        return;

    sleep_time.tv_sec = ms/1000;
    sleep_time.tv_nsec = (ms-sleep_time.tv_sec*1000)*1000;
    nanosleep(&sleep_time, NULL);
}
