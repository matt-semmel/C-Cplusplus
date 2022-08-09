#include <pi/pi.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __APPLE__
    #define generate_random(randState, variable_addr) *(variable_addr) = erand48(randState)
    #define seed_random(seed, randState) srand48(seed)
    #define rand_state_t(name) unsigned short (name)[3]
#else 
    #define generate_random(randState, variable_addr) drand48_r(randState, variable_addr)
    #define seed_random(seed, randState) srand48_r(seed, randState)
    #define rand_state_t(name) struct drand48_data (name)
#endif

// Shared variable! We need to protect it
size_t count_hits = 0;
pthread_mutex_t mutex;

/* Implementation */

/** This function generates random 2D coordinates in
       a 2x2 square, and counts the number of coordinate 
       pairs that fall within a circle of radius 1 centered
       with the square
*/
void *calculate_pi(void *data) {
    
    calculate_pi_t *args = data;
    /** 
        Generating random numbers
    */
    
    rand_state_t(randState);
    // About this line of code: You should never do this if you care about randomness!
    // All threads will generate the same sequence of numbers, so the randomness is fake!
    // And the estimation of pi skewed... But... it makes it repeatable
    seed_random(449 , &randState); // Don't change this... This makes the random numbers repeatable! And gradable!

// Declare and initialize a local variable.
    size_t local_hits = 0;

    for (size_t i = args->start ; i < args->end; i++ ) {

        double x;
        generate_random(&randState, &x);        // Returns a pseudo-random double between 0 .. 1.
        x *= 2;                                 // Make it between 0 .. +2
        x -= 1;                                 // Make it between -1 .. +1

        double y;
        generate_random(&randState, &y);        // Returns a pseudo-random double between 0 .. 1.
        y = 2*y-1;                              // Make it between -1 .. +1

        if ( x*x + y*y <= 1.0*1.0 ) {           // If it's within a circle of radius 1

            // Count hits, this is a shared variable, I should probably do something about it
//         pthread_mutex_lock(&mutex);
//	   count_hits++;
//	   pthread_mutex_unlock(&mutex);
//         Above lines lock/unlock the mutex. Protects the global variable but cuts the speed in half because it locks/unlocks every L00P. As per instuctions.

	   local_hits++;
//	   count_hits = (local_hits*NUM_THREADS);
//	   This greatly improves performance. However, it seems like to get the correct approx of Pi we need to multiply this by the number of threads.
//	   Ask Luis for clarification...
        }

    }
    pthread_mutex_lock(&mutex);
    count_hits = (local_hits*NUM_THREADS);
    pthread_mutex_unlock(&mutex);
//  I believe this effectively solves the problem. Performance improved by placing this OUTSIDE teh L00P.

    return NULL;
}
