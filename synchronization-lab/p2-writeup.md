# Project 2 Writeup
## By: Matt Semmel

### Deadlock
We can prevent deadlock in this program by protecting all shared data access with mutexes to make sure no one gets stuck waiting for each other. Additionally, the mutex will unlock on any exit or return to make sure other threads can lock the mutex and have their chance to run.

Had a problem with deadlocking on an earlier submission. Turns out I forgot to decrement the number of guides currently in the museum. Took me an entire day to figure out. 

### Starvation-Free
Wrote this in a way that guarantees all tickets will be admitted eventually provided the number of visitors does not exceed the max of visitors per guide. This means that any excess visitors will leave the queue. Also took steps to make sure ticket numbers are toured sequentially and in order like "first come first serve". All visitors ticketed prior to reaching capacity will be toured.
