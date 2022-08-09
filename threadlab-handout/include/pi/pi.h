#pragma once
#include <stdlib.h>
#include <pthread.h>


// Define debug if not defined previously
#ifndef DEBUG
// Set to zero to disable debug
// Set to one to enable debug
#define DEBUG 1
#endif

// Define the number of threads we are using
#ifndef NUM_THREADS
#define NUM_THREADS 4
#endif

// Define the number of points we are generating
#ifndef NUM_POINTS
#define NUM_POINTS 1000000000LU
#endif

// This will remove the PDEBUG calls if debug is disabled
#if DEBUG
#define PDEBUG(fmt, ...) printf("DEBUG("__FILE__": %d): "fmt, __LINE__, ##__VA_ARGS__)
#else
#define PDEBUG(fmt, ...) 
#endif

// Shared variable! We need to protect it
extern size_t count_hits;
extern pthread_mutex_t  mutex;

// Struct that will hold the arguments for the thread
typedef struct {
    size_t thread_id;
    size_t start;
    size_t end;
} calculate_pi_t;

/* Function declarations */
void *calculate_pi(void *data);
