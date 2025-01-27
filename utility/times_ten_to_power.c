/* utility/times_ten_to_power.c */
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

/* Global variables */
/* ---------------- */

/* Prototypes */
/* ---------- */
char *get_new_value(	char *buffer,
			char delimiter,
			int piece_offset );

int main( int argc, char **argv )
{
	int piece_offset;
	char delimiter;
	char buffer[ 65536 ];
	char *new_value;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			 "Usage: %s delimiter piece_offset\n", 
			 argv[ 0 ] );
		return 0;
	}

	delimiter = *argv[ 1 ];
	piece_offset = atoi( argv[ 2 ] );

	while( get_line( buffer, stdin ) )
	{
		if ( ! ( new_value = get_new_value(	buffer,
							delimiter,
							piece_offset ) ) )
		{
			printf( "%s\n", buffer );
		}
		else
		{
			printf( "%s\n", piece_replace(	buffer, 
							delimiter,
							new_value,
							piece_offset ) );
		}
		fflush( stdout );
	}

	return 0;

} /* main() */


char *get_new_value(	char *buffer,
			char delimiter,
			int piece_offset )
{
	char value_string[ 128 ];
	double value_double;
	static char new_value[ 128 ];
	static int line_number = 0;

	line_number++;

	if ( !piece( value_string, delimiter, buffer, piece_offset ) )
	{
		return (char *)0;
	}

	if ( !character_exists( value_string, 'e' )
	&& ( !character_exists( value_string, '-' )
	||   !character_exists( value_string, '+' ) ) )
	{
		strcpy( new_value, value_string );
		return new_value;
	}

	value_double = atof( value_string );

	sprintf(	new_value,
			"%.8lf",
			value_double );
	return new_value;

} /* get_new_value() */

