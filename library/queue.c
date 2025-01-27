/* queue.c */
/* ------- */
/* ======================================================= */
/* This supports the QUEUE ADT                             */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "queue.h"

QUEUE *queue_new()
{
	return queue_init();
}

QUEUE *queue_init()
{
        QUEUE *s = (QUEUE *)calloc( 1, sizeof( QUEUE ) );

        if ( !s )
        {
                fprintf( stderr, "allocation failed in create_queue()\n" );
                exit( 1 );
        }

        s->start_ptr = 0;
        s->end_ptr = 0;

        return s;

} /* queue_init() */

int enqueue( QUEUE *q, char *item )
{
	int results;

	q->buffer[ q->end_ptr ] = item;

	results = q->end_ptr + 1;
	q->end_ptr = results % QUEUE_MAX_QUEUE;
/*
	q->end_ptr = ++q->end_ptr % QUEUE_MAX_QUEUE;
*/

	return 1;

} /* enqueue() */

char *dequeue( QUEUE *s )
{
	char *return_ptr;
	int results;

	if ( s->start_ptr == s->end_ptr ) return (char *)0;

	return_ptr = s->buffer[ s->start_ptr ];

	results = s->start_ptr + 1;
	s->start_ptr = results %  QUEUE_MAX_QUEUE;

	return return_ptr;

} /* dequeue() */


char *queue_top( QUEUE *s )
{
	int here;

	if ( s->start_ptr == s->end_ptr ) return (char *)0;

	here = ( s->start_ptr - 1 ) % QUEUE_MAX_QUEUE;
	return s->buffer[ here ];

} /* queue_top() */

char *queue_bottom( QUEUE *s )
{
	if ( s->start_ptr == s->end_ptr ) return (char *)0;

	return s->buffer[ s->start_ptr ];

} /* queue_bottom() */

int queue_length( QUEUE *q )
{
	if ( q->start_ptr == q->end_ptr )
		return 0;
	else
	if ( q->end_ptr > q->start_ptr)
		return q->end_ptr - q->start_ptr;
	else
		return q->end_ptr + ( QUEUE_MAX_QUEUE - q->start_ptr );
} /* queue_length() */

int queue_is_empty( QUEUE *s )
{
	return ( s->start_ptr == s->end_ptr );
}

void queue_add_string( QUEUE *queue, char *s )
{
	enqueue( queue, s );
}

void queue_display( QUEUE *s )
{
	int start_ptr;

	if ( s->start_ptr == s->end_ptr )
		return;
	else
		start_ptr = s->start_ptr;

	while ( start_ptr != s->end_ptr )
	{
		printf( "%s\n", s->buffer[ start_ptr ] );
		start_ptr++;
		start_ptr = start_ptr % QUEUE_MAX_QUEUE;
	}
} /* queue_display() */

void queue_free( QUEUE *queue )
{
	free( queue );
}

void queue_free_data( QUEUE *queue )
{
	int start_ptr;

	if ( !queue->start_ptr )
		return;
	else
		start_ptr = queue->start_ptr;

	while ( start_ptr != queue->end_ptr )
	{
		free( queue->buffer[ start_ptr ] );
		start_ptr++;
		start_ptr = start_ptr % QUEUE_MAX_QUEUE;
	}
	queue->start_ptr = 0;
	queue->end_ptr = 0;
} /* queue_free_data() */

