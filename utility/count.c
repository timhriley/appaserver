/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/count.c		 			*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "date.h"

#define BUFFER_SIZE 4096

int main( int argc, char **argv )
{
	char buffer[ BUFFER_SIZE ];
	int count, i, total;
	DATE *start_date;
	DATE *now_date;
	boolean output_usage = 0;

	if ( argc == 1 )
	{
		output_usage = 1;
	}
	else
	{
		count = atoi( argv[ 1 ] );

		if ( !count ) output_usage = 1;
	}
	
	if ( output_usage )
	{
		fprintf( stderr, "Usage: %s output_after\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	i = 0;
	total = 0;

	start_date = date_now( date_utc_offset() );
	now_date = date_calloc();

	while ( string_input( buffer, stdin, BUFFER_SIZE ) )
	{
		printf( "%s\n", buffer );

		total++;

		if ( ++i == count )
		{
			now_date =
				date_set_now(
					now_date,
					date_utc_offset() );

			fprintf( stderr,
				 "Count (%02d): %10d\n",
				 date_subtract_minutes(
					now_date,
					start_date ),
				 total );

			fflush( stderr );
			i = 0;
			date_copy( start_date, now_date );
		}
	}

	if ( count != 1 )
	{
		now_date =
			date_set_now(
				now_date,
				date_utc_offset() );

		fprintf( stderr,
			 "Count (%02d): %10d\n",
			 date_subtract_minutes(
				now_date,
				start_date ),
			 total );
	}

	return 0;
}
