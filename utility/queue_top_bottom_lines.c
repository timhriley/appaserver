/* utility/queue_top_bottom_lines.c				 */
/* ------------------------------------------------------------- */
/* This program allows you to see the top and bottom portions	 */
/* of the output to a process, while skipping the volumous	 */
/* middle portion.						 */
/*								 */
/* Freely available software: see Appaserver.org		 */
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "queue.h"

int main( int argc, char **argv )
{
	QUEUE *queue = queue_new();
	char input_buffer[ 8192 ];
	int item_number = 0;
	int number_to_queue;
	int number_skipped = 0;
	char *item_to_trash;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s number_to_queue\n", argv[ 0 ] );
		exit( 1 );
	}

	number_to_queue = atoi( argv[ 1 ] );

	if ( number_to_queue > QUEUE_MAX_QUEUE )
	{
		fprintf( stderr,
			 "Error in %s: max number_to_queue is %d.\n",
			 argv[ 0 ],
			 QUEUE_MAX_QUEUE );
		exit( 1 );
	}

	while( timlib_get_line( input_buffer, stdin, 8192 ) )
	{
		if ( ++item_number <= number_to_queue )
		{
			printf( "%s\n", input_buffer );
		}
		else
		{
			if ( queue_length( queue ) == number_to_queue )
			{
				item_to_trash = dequeue( queue );
				free( item_to_trash );
				number_skipped++;
			}

			if ( !enqueue( queue, strdup( input_buffer ) ) )
			{
				fprintf(stderr,
					"ERROR in %s/%s()/%d: queue is full.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}
		}
	}

	if ( number_skipped ) printf( "skipped %d\n", number_skipped );

	queue_display( queue );
/*
	queue_free_data( queue );
	queue_free( queue );
*/
	return 0;
} /* main() */

