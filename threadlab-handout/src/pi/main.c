#include <pi/pi.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char *argv[]) {

    // Setup data 
    pthread_mutex_init(&mutex, NULL);
    calculate_pi_t args[NUM_THREADS];
    for( int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].start = i*NUM_POINTS/NUM_THREADS;
        args[i].end = (i+1)*NUM_POINTS/NUM_THREADS ;
    }

    // Start threads
    pthread_t threads[NUM_THREADS] = {0};
    for( int i = 0; i < NUM_THREADS; i++) {

        PDEBUG("Creating thread %lu: %lu, %lu\n", args[i].thread_id, args[i].start, args[i].end);
        if (pthread_create(&threads[i], NULL, calculate_pi, &args[i]) != 0) {

            fprintf(stderr, "Couldn't create thread\n");
            return 1;
        }
    }

    // Wait for threads to finish
    for( int i = 0; i < NUM_THREADS; i++) {

        if (pthread_join(threads[i], NULL) == 0) {
            PDEBUG("Joined thread %lu: %lu\n", args[i].thread_id, count_hits);
        }
    }
    
    printf("PI = %f\n", (4.0*count_hits)/NUM_POINTS);

    return 0;

}

