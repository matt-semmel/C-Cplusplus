/*
 *
 * MATT SEMMEL - MWS73
 *
 * Developed by R. E. Bryant, 2017
 * Extended to store strings, 2018
 */

/*
 * This program implements a queue supporting both FIFO and LIFO
 * operations.
 *
 * It uses a singly-linked list to represent the set of queue elements
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
  Create empty queue.
  Return NULL if could not allocate space.
*/
queue_t *q_new()
{
    queue_t *q =  malloc(sizeof(queue_t));
    /* What if malloc returned NULL? */
    /* What IF indeed... */
    if (q == NULL)
	return NULL;

    /* If space is allocatable then this function will set a new head for the list and
     * initialize the size of the queue to zero. */

    q->head = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
/*
 * Per instructions, MALLOC and FREE, ONLY. No CALLOC, etc. Makes life easier.
 */

{
    /*
     * First ya gotta check and make sure the queue isn't empty!
     */

    if ( q == NULL )
	return;

    /* Assign a temporary pointer to the head of the queue. */
    /* By making a copy of the element and keeping it one spot ahead we can traverse the
     * list with "prev" to free the data that was just deleted by "copy". */
    /* Use a loop to traverse list and free up the elements! */

    list_ele_t *prev = q->head;
    list_ele_t *copy = q->head;
    while ( copy != NULL )
    {
	free( copy->value );
	prev = copy;
	copy = copy->next;
	free ( prev );
    }
    /* How about freeing the list elements and the strings? */
    /* Free queue structure */
    free(q);
}

/*
  Attempt to insert element at head of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    /* We have a var for queue size in header but we need to make one locally for string
     * size and we need to make a copy of "*s" as well. */

    list_ele_t *newh;
    int s_size;
    char *s_copy;

    /* What should you do if the q is NULL? */
    /* OMG we should return false if it's NULL since this is a bool function! */
    if ( q == NULL )
	return false;

    /* Per lab instructions, called functions strcpy and strlen don't account for null char.
     * We have to add one more space to length to account for that
     * Must also allocate memory for the copy of s. */

    /* By calling "strlen" on our input "s", we get the size of the string,
     * which is just a char array, we can pass that size to MALLOC, and tell it to
     * allocate the size of a char TIMES the size of the string */

    s_size = strlen(s) + 1;
    s_copy = malloc ( s_size * sizeof(char) );

    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */

    /* If s_copy is null, just return false. If newh is null, must free space allocated
     * for copy, THEN return false. */

    if ( s_copy == NULL )
	return false;

    newh = malloc( sizeof (list_ele_t) );
    if ( newh == NULL )
    {
	free(s_copy);
	return false;
    }

    /* Call "strcopy" function to copy s. If there is a NULL head, then have the head/tail
     * pointers point to a new head (newh), and assign "next" pointer to NULL. Just like
     * Java. */

    strcpy(s_copy, s);
    newh->value = s_copy;

    if ( q->head == NULL )
    {
	q->tail = newh;
	newh->next = NULL;
    }

    newh->next = q->head;
    q->head = newh;
    q->size++;
    return true;
}


/*
  Attempt to insert element at tail of queue.
  Return true if successful.
  Return false if q is NULL or could not allocate space.
  Argument s points to the string to be stored.
  The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    /* You need to write the complete code for this function */
    /* Remember: It should operate in O(1) time */

    /* This is just the same as q_insert_head but backwards! This hasn't been much
     * different from the LL labs in 445 with Java. */

    /* Like inserting at the head of the list, we need to copy the string "s" and keep
     * track of the size of the string (char array). */

    list_ele_t *newt;
    char *s_copy;
    int s_size;

    if ( q == NULL )
	return false;

    s_size = strlen(s) + 1;
    newt = malloc( sizeof(list_ele_t) );
    if ( newt == NULL )
	return false;

    s_copy = malloc ( s_size * sizeof(char) );
    if ( s_copy == NULL )
    {
	free ( newt );
	return false;
    }

    strcpy ( s_copy, s );
    newt->value = s_copy;
    newt->next = NULL;

    if ( q->head == NULL )
    {
	q->head = newt;
	q->tail = newt;
	q->size++;
	return true;
    }

    /* We essentially just swap the head and tail pointers for this function. */

    q->tail->next = newt;
    q->tail = newt;
    q->size++;
    return true;
}

/*
  Attempt to remove element from head of queue.
  Return true if successful.
  Return false if queue is NULL or empty.
  If sp is non-NULL and an element is removed, copy the removed string to *sp
  (up to a maximum of bufsize-1 characters, plus a null terminator.)
  The space used by the list element and the string should be freed.
*/
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    /* You need to fix up this code. */
    /* Okay. */
//    q->head = q->head->next;


/* Handling of the cases where queue is empty or the head of the queue is NULL
 * If the queue itself or the head of the queue is NULL then return false. */

    if ( q == NULL )
	return false;
    if ( q->head == NULL )
	return false;

/* As per instructions, copied to "sp" and accounted for null char (\0) */

    if ( sp != NULL )
    {
	strncpy (sp, q->head->value, bufsize - 1);
	*(sp + bufsize - 1) = '\0';
    }

/* Free spaces then return true!! */
    list_ele_t *copy = q->head;
    q->head = q->head->next;
    free (copy->value);
    free (copy);
    q->size--;

    return true;
}

/*
  Return number of elements in queue.
  Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    /* You need to write the code for this function */
    /* Remember: It should operate in O(1) time */
    /* Should be as simple as a one-line return statement just like LLs in Java.
     * Correction: That was the "isEmpty" method from the LL Java labs, NOT size!
     * BE SURE TO INCREMENT SIZE COUNTER (the one in the header!!!) IN OTHER FUNCTS!
     * Otherwise this probably won't work correctly... */

    /* So I kept wondering why this function was returning undefined behavior,
     * then realized I hadn't initialized size in the new queue funct. lol */

    if ( q == NULL )
	return 0;

    return q->size;
}

/*
  Reverse elements in queue
  No effect if q is NULL or empty
  This function should not allocate or free any list elements
  (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
  It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    /* You need to write the code for this function */
    /* omg no way lol */

    list_ele_t *copy, *next, *prev;

    /* Account for cases where queue is empty or head is null per instructions */

    if ( q == NULL )
	return;
    if ( q->head == NULL )
	return;

    /* Rearranging elements (mixing up the pointers) but NOT freeing or reallocating! */
    /* prev=head - copy=next elem from prev - next=copy - next head=NULL - tail=head */
    /* Just like 445 labs */

    prev = q->head;
    copy = prev->next;
    next = copy;
    q->head->next = NULL;
    q->tail = q->head;

    /* This will "mix it up" until our copy pointer hits NULL, effectively reversing
     * the elements in the queue. */

    while ( copy != NULL )
    {
	next = copy->next;
	copy->next = prev;
	prev = copy;
	copy = next;
    }

    q->head = prev;
    return;

}
