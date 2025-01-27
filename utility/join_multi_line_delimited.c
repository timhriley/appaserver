/* utility/join_multi_line_delimited.c				 */
/* ------------------------------------------------------------- */
/* > > > > the nature of the data is something like this:
 * > > > >
 * > > > > fixed number of columns, with '|' delimiter:
 * > > > >  col 1| col 2 | col 3 | ... col_n
 * > > > >  col 1| col 2 | col 3 | ... col_n
 * > > > >  col 1| col 2 | col 3 | ... col_n
 * > > > >  col 1| col 2 | col 3 | ... col_n
 * > > > >
 * > > > > some of the time one of these columns will contain multi-line text:
 * > > > >
 * > > > > col 1| col 2 |
 * > > > > blah blah blah ...
 * > > > > blah blah blah ...
 * > > > > blah blah blah ...
 * > > > > blah blah blah ...
 * > > > > blah blah blah ... | col_n
 * > > > >

   ------------------------------------------------------------- */
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
	char input_buffer[ 4096 ];
	int delimiter_count;
	int input_delimiter_count;
	char delimiter;
	char *string;

	if ( argc != 3 )
	{
		fprintf(	stderr,
				"Usage: %s delimiter delimiter_count\n",
				argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	delimiter_count = atoi( argv[ 2 ] );

	while( get_line( input_buffer, stdin ) )
	{
		input_delimiter_count =
			count_character(
				delimiter,
				input_buffer );

		/* ----------- */
		/* Normal line */
		/* ----------- */
		if ( input_delimiter_count == delimiter_count )
		{
			printf( "%s\n", input_buffer );
		}
		else
		/* ------------------ */
		/* Start a multi-line */
		/* ------------------ */
		if  ( input_delimiter_count
		&&    !queue_length( queue ) )
		{
			printf( "%s", input_buffer );
		}
		else
		/* --------------- */
		/* Is a multi-line */
		/* --------------- */
		if ( !input_delimiter_count )
		{
			enqueue( queue, strdup( input_buffer ) );
		}
		else
		/* ---------------- */
		/* End a multi-line */
		/* ---------------- */
		if ( input_delimiter_count
		&&   queue_length( queue ) )
		{
			int first_time = 1;
			while( queue_length( queue ) )
			{
				if ( first_time )
				{
					first_time = 0;
				}
				else
				{
					printf( " " );
				}
				string = dequeue( queue );
				printf( "%s", string );
				free( string );
			}
			printf( "%s\n", input_buffer );
		}
	}
	return 0;
} /* main() */

