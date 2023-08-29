#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "museumsim.h"

//
// In all of the definitions below, some code has been provided as an example
// to get you started, but you do not have to use it. You may change anything
// in this file except the function signatures.
//


struct shared_data {
	// Add any relevant synchronization constructs and shared state here.
	// For example:
	pthread_mutex_t ticket_mutex;
	pthread_cond_t guide_wait_to_leave;
	pthread_cond_t guide_wait_to_admit;
	pthread_cond_t guide_wait_to_enter;
	pthread_cond_t visitor_wait_to_enter;
	int tickets;
	int curr_tickets;
	int in_queue;
	int curr_guides;
	int curr_inside;
	int guides_finished;
};

static struct shared_data shared;


/**
 * Set up the shared variables for your implementation.
 * 
 * `museum_init` will be called before any threads of the simulation
 * are spawned.
 */
void museum_init(int num_guides, int num_visitors)
{
	// This initializes teh mutex
	pthread_mutex_init(&shared.ticket_mutex, NULL);

	// Chooses the smallest number of visitors per guide and number of guides, or number of visitors.
	shared.tickets = MIN(VISITORS_PER_GUIDE * num_guides, num_visitors);

	// Init pthread conditions
	pthread_cond_init(&shared.guide_wait_to_leave, NULL);
	pthread_cond_init(&shared.guide_wait_to_admit, NULL);
	pthread_cond_init(&shared.guide_wait_to_enter, NULL);
	pthread_cond_init(&shared.visitor_wait_to_enter, NULL);

	// Current ticket is 1 greater than total tickets
	shared.curr_tickets = shared.tickets + 1;

	// Init the rest to zero
	shared.in_queue = 0;
	shared.curr_guides = 0;
	shared.curr_inside = 0;
	shared.guides_finished = 0;
}


/**
 * Tear down the shared variables for your implementation.
 * 
 * `museum_destroy` will be called after all threads of the simulation
 * are done executing.
 */
void museum_destroy()
{
	//DESTROY! MWAHAHAHAHA!
	pthread_mutex_destroy(&shared.ticket_mutex);
	pthread_cond_destroy(&shared.guide_wait_to_leave);
	pthread_cond_destroy(&shared.guide_wait_to_admit);
	pthread_cond_destroy(&shared.guide_wait_to_enter);
	pthread_cond_destroy(&shared.visitor_wait_to_enter);
}


/**
 * Implements the visitor arrival, touring, and leaving sequence.
 */
void visitor(int id)
{
	// A visitor has arrived. Lock teh mutex.
	visitor_arrives(id);
	pthread_mutex_lock(&shared.ticket_mutex);
	{

		// If there are no tickets left, unlock and return.
		if(shared.tickets == 0)
		{
			pthread_mutex_unlock(&shared.ticket_mutex);
			return;
		}

		// Reduce tickets, add to queue
		int ticket_num = shared.tickets--;
		shared.in_queue++;

		// Put yourself out there. Broadcast to guides kekz.
		// Then wait for ticket to come up.
		pthread_cond_broadcast(&shared.guide_wait_to_admit);
		while (ticket_num < shared.curr_tickets)
		{
			pthread_cond_wait(&shared.visitor_wait_to_enter, &shared.ticket_mutex);
		}
	}

	// Unlock mutex for ze tours.
	pthread_mutex_unlock(&shared.ticket_mutex);
	visitor_tours(id);

	// Relock mutex after ze tours.
	pthread_mutex_lock(&shared.ticket_mutex);
	{
		// Decrement number of visitors inside then broadcast.
		shared.curr_inside--;
		pthread_cond_broadcast(&shared.guide_wait_to_leave);
		visitor_leaves(id);
	}

	// Unlock mutex and return.
	pthread_mutex_unlock(&shared.ticket_mutex);
	return;
}

/**
 * Implements the guide arrival, entering, admitting, and leaving sequence.
 */
void guide(int id)
{
	// A guide has arrived. Lock teh mutex!
	guide_arrives(id);
	pthread_mutex_lock(&shared.ticket_mutex);
	{
		// Check if there are tickets left and no visitors in queue.
		// Return if true.
		if (shared.tickets == 0 && shared.in_queue == 0)
		{
			pthread_mutex_unlock(&shared.ticket_mutex);
			return;
		}

		// If the max number of guides are in the museum or there are guides waiting to leave then wait.
//		while (shared.guides_finished != 0 || shared.curr_guides == GUIDES_ALLOWED_INSIDE)
		while (shared.curr_guides == GUIDES_ALLOWED_INSIDE)
		{
			pthread_cond_wait(&shared.guide_wait_to_enter, &shared.ticket_mutex);
		}

		// Guide enters, increase number of guides, begin tracking visitors admitted by that guide.
//		pthread_mutex_unlock(&shared.ticket_mutex);
		guide_enters(id);
		shared.curr_guides++;
		int admitted_visitors = 0;

		// Admit visitors until we hit the max or no visitors left
		while (admitted_visitors < VISITORS_PER_GUIDE)
		{
			// If no more tickets or visitors in queue break teh loop!
			if(shared.tickets == 0 && shared.in_queue == 0)
			{
//				guide_leaves(id);
//				pthread_mutex_unlock(&shared.ticket_mutex);
//				return;
				break;
			}
			int visitors_left = 1;

			// WAIT for visitors to come in OR for space in museum. WHILE waiting, if tickets run out set to zero and BREAK teh loop.
			while (shared.in_queue == 0)
			{
				pthread_cond_wait(&shared.guide_wait_to_admit, &shared.ticket_mutex);
				if (shared.tickets == 0 && shared.in_queue == 0)
				{
//					guide_leaves(id);
//					pthread_mutex_unlock(&shared.ticket_mutex);
//					return;
					visitors_left = 0;
					break;
				}
			}

			// Double check if there are any visitors left to admit
			if (visitors_left == 0)
				break;

			// Once space opens up AND there is still a queue, guide admits, decrement queue, ticket num, increment visitors inside and num admitted BY guide, then broadcast that visitors can ENTER.
			guide_admits(id);
			shared.in_queue--;
			shared.curr_tickets--;
			shared.curr_inside++;
			admitted_visitors++;
			pthread_cond_broadcast(&shared.visitor_wait_to_enter);
		}

		// Once teh loop is finished, increment guides finished and BROADCAST to other guides.
		shared.guides_finished++;
		pthread_cond_broadcast(&shared.guide_wait_to_leave);

		// Once finished, wait for ALL visitors to leave and other guide to finish.
		while (shared.curr_inside != 0 || shared.guides_finished < shared.curr_guides)
		{
			pthread_cond_wait(&shared.guide_wait_to_leave, &shared.ticket_mutex);
		}

		// Leave once ALL guides finish and decrement num of guides IN museum, BROADCAST, then LEAVE.
		shared.guides_finished--;
		shared.curr_guides--;
		pthread_cond_broadcast(&shared.guide_wait_to_enter);
		guide_leaves(id);
//		pthread_mutex_unlock(&shared.ticket_mutex);
//		return;
	}

	// Unlock teh mutex, RETURN
	pthread_mutex_unlock(&shared.ticket_mutex);
	return;
}
