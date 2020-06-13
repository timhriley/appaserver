/* utility/count.c		*/
/* ---------------------- 	*/
/* 				*/
/* -- Tim Riley			*/
/* ----------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "date.h"

#define BUFFER_SIZE 4096

int main( int argc, char **argv )
{
	char buffer[ BUFFER_SIZE ];
	int count, i, total;

	if ( argc == 1 )
	{
		fprintf( stderr, "Usage: %s output_after [output_msg]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	count = atoi( argv[ 1 ] );
	if ( !count )
	{
		fprintf( stderr, "Usage: %s output_after [output_msg]\n",
			 argv[ 0 ] );
		exit( 1 );
	}
	

	i = 0;
	total = 0;
	while ( get_line( buffer, stdin ) )
	{
		if ( count == 1 )
		{
			printf( "%.3d) %s\n", ++i, buffer );
			continue;
		}

		printf( "%s\n", buffer );

		total++;
		if ( ++i == count )
		{
			if ( argc == 3 )
				fprintf( stderr, "%s (%s): %6d\n",
					 argv[ 2 ],
					 date_get_now_hhmmss(
						date_utc_offset() ),
					 total );
			else
				fprintf( stderr, "Count (%s): %6d\n",
					 date_get_now_hhmmss(
						date_utc_offset() ),
					 total );
			fflush( stderr );
			i = 0;
		}
	}

	if ( count != 1 )
	{
		if ( argc == 3 )
			fprintf( stderr,
				 "%s (%s): %6d\n",
				 argv[ 2 ],
				 date_get_now_hhmmss(
					date_utc_offset() ),
				 total );
		else
			fprintf( stderr,
				 "Count (%s): %6d\n",
				 date_get_now_hhmmss(
					date_utc_offset() ),
				 total );
	}

	return 0;

} /* main() */
