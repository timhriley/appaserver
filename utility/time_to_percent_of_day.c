/* utility/time_to_percent_of_day.c */
/* -------------------------------- */
/* Tim Riley	      		    */
/* -------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	int time_offset;
	char time_string[ 128 ], percent_of_day_string[ 128 ];
	char delimiter;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s time_offset delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	time_offset = atoi( argv[ 1 ] );
	delimiter = *argv[ 2 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !piece(	time_string, 
				delimiter, 
				buffer, 
				time_offset ) )
		{
			fprintf(stderr,
		"%s ERROR: cannot extract time in (%s)\n",
				argv[ 0 ], buffer );
			exit( 1 );
		}

		sprintf( percent_of_day_string,
			 "%.4lf",
			 get_percent_of_day( time_string ) );

		piece_insert(	buffer, 
				delimiter, 
				percent_of_day_string,
				time_offset + 1 );
		printf( "%s\n", buffer );
	}
	return 0;

} /* main() */

