/* utility/group_sum.c */
/* ------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

void group_sum( char delimiter );

int main( int argc, char **argv )
{
	char delimiter = {0};

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	group_sum( delimiter );
	
	return 0;

} /* main() */


void group_sum( char delimiter )
{
	char input_buffer[ 1024 ];
	char compare_buffer[ 1024 ];
	char old_compare_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	char value_buffer[ 1024 ];
	double sum = 0.0;
	int first_time = 1;

	*old_compare_buffer = '\0';

	while( get_line( input_buffer, stdin ) )
	{
		piece( piece_buffer, delimiter, input_buffer, 0 );

		if ( !piece(	value_buffer,
				delimiter,
				input_buffer,
				1 ) )
		{
			fprintf( stderr,
			"Error: cannot find delimiter in (%s)\n",
				 input_buffer );
			continue;
		}

		strcpy( compare_buffer, piece_buffer );

		if ( first_time )
		{
			first_time = 0;	
			sum = atof( value_buffer );
			strcpy( old_compare_buffer, compare_buffer );
			continue;
		}

		if ( strcmp( compare_buffer, old_compare_buffer ) == 0 )
		{
			sum += atof( value_buffer );
		}
		else
		{
			printf( "%s%c%.5lf\n",
				old_compare_buffer,
				delimiter,
				sum );
			sum = atof( value_buffer );
		}

		strcpy( old_compare_buffer, compare_buffer );
	}

	if ( !first_time )
	{
		printf( "%s%c%.5lf\n",
			old_compare_buffer,
			delimiter,
			sum );
	}

} /* group_sum() */

