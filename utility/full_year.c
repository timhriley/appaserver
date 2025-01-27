/* full_year.c 					*/
/* -------------------------------------------- */
/* Input: 	two digit year			*/
/*		number years for future events	*/
/* Output:	full year (with century added) 	*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "calculate_full_year.h"

int main( int argc, char **argv )
{
	int current_year;
	int difference;

	if ( argc < 3 )
	{
		fprintf(stderr,
		"Usage: %s two_digit_year years_future_events [current_year]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 4 )
		current_year = atoi( argv[ 3 ] );
	else
		current_year = 0;

	printf( "%d\n", calculate_full_year( &difference,
					     atoi( argv[ 1 ] ), 
					     atoi( argv[ 2 ] ),
					     current_year ) );

	return 0;

} /* main() */
