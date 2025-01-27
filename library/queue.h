/* queue.h						   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_MAX_QUEUE	65536
/* #define QUEUE_MAX_QUEUE	3 */

typedef struct {
	char *buffer[ QUEUE_MAX_QUEUE ];
	int start_ptr;
	int end_ptr;
} QUEUE;

QUEUE *queue_init();
int enqueue( QUEUE *, char * );
char *dequeue( QUEUE * );
char *queue_top( QUEUE * );
char *queue_bottom( QUEUE * );
void queue_display( QUEUE * );
int queue_is_empty( QUEUE * );
int queue_length( QUEUE * );
QUEUE *queue_new();
void queue_free( QUEUE *queue );
void queue_free_data( QUEUE *queue );
void queue_add_string( QUEUE *queue, char *s );

#endif
