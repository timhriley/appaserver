/* utility/piece_inverse.c */
/* ----------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "timlib.h"

#define MAX_FIELDS_TO_TRIM	50

/* Global variables */
/* ---------------- */
int trim_array[ MAX_FIELDS_TO_TRIM ];

/* Prototypes */
/* ---------- */
void piece_inverse_utility( char *piece_offset_comma_list, char delimiter );
void populate_trim_array( int *trim_array, char *piece_offset_comma_list );
int initialize_trim_array( int trim_array[ MAX_FIELDS_TO_TRIM ] );

int main( int argc, char **argv )
{
	char *piece_offset_comma_list;
	char delimiter;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s offset_comma_list delimiter\n", 
			 argv[ 0 ] );
		return 0;
	}

	piece_offset_comma_list = argv[ 1 ];
	delimiter = *argv[ 2 ];

	initialize_trim_array( trim_array );
	piece_inverse_utility( piece_offset_comma_list, delimiter );
	return 0;

} /* main() */



int initialize_trim_array( int trim_array[ MAX_FIELDS_TO_TRIM ] )
{
	int i;

	for( i = 0; i < MAX_FIELDS_TO_TRIM; i++ ) trim_array[ i ] = -1;
	return 1;

} /* initialize_trim_array() */


void piece_inverse_utility( char *piece_offset_comma_list, char delimiter )
{
	char input_buffer[ 1024 ];
	int p;

	populate_trim_array( trim_array, piece_offset_comma_list );

	while( get_line( input_buffer, stdin ) )
	{
		for( p = 0; trim_array[ p ] != -1; p++ )
		{
			piece_inverse(	input_buffer, 
					delimiter, 
					trim_array[ p ] - p );
		}
		printf( "%s\n", input_buffer );
	}
} /* piece_inverse_utility() */

/* Convert a string of numbers separated by a comma to an array of integer. */
/* ------------------------------------------------------------------------ */
void populate_trim_array( int *trim_array, char *piece_offset_comma_list )
{
	char piece_buffer[ 256 ];
	int p;

	for( p = 0;
	     piece(	piece_buffer, 
			',', 
			piece_offset_comma_list, 
			p );
	     p++ )
	{
		trim_array[ p ] = atoi( piece_buffer );
	}
} /* populate_trim_array() */
