/* utility/piece_arithmetic.c */
/* -------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

void piece_arithmetic(	int piece_offset_1,
			int piece_offset_2,
			char *operation,
			char delimiter );

int main( int argc, char **argv )
{
	int piece_offset_1;
	int piece_offset_2;
	char delimiter;
	char *operation;

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s piece_offset_1 piece_offset_2 operation delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	piece_offset_1 = atoi( argv[ 1 ] );
	piece_offset_2 = atoi( argv[ 2 ] );
	operation = argv[ 3 ];
	delimiter = *argv[ 4 ];

	piece_arithmetic(	piece_offset_1,
				piece_offset_2,
				operation,
				delimiter );
	return 0;

} /* main() */

void piece_arithmetic(	int piece_offset_1,
			int piece_offset_2,
			char *operation,
			char delimiter )
{
	char buffer[ 4096 ];
	char piece_buffer_1[ 1024 ];
	char piece_buffer_2[ 1024 ];
	double results = 0.0;

	while( get_line( buffer, stdin ) )
	{
		if ( ! piece(	piece_buffer_1,
				delimiter,
				buffer,
				piece_offset_1 ) )
		{
			fprintf( stderr, 
				 "%s/%s(): cannot get piece: %d\n",
				 __FILE__,
				 __FUNCTION__,
				 piece_offset_1 );
			continue;
		}

		if ( strcmp( operation, "percentage" ) != 0 )
		{
			if ( ! piece(	piece_buffer_2,
					delimiter,
					buffer,
					piece_offset_2 ) )
			{
				fprintf( stderr, 
					 "%s/%s(): cannot get piece: %d\n",
					 __FILE__,
					 __FUNCTION__,
					 piece_offset_2 );
				continue;
			}
		}

		if ( strcmp( operation, "multiply" ) == 0 )
		{
			results =	atof( piece_buffer_1 ) *
					atof( piece_buffer_2 );
		}
		else
		if ( strcmp( operation, "addition" ) == 0
		||   strcmp( operation, "add" ) == 0 )
		{
			results =	atof( piece_buffer_1 ) +
					atof( piece_buffer_2 );
		}
		else
		if ( strcmp( operation, "division" ) == 0
		||   strcmp( operation, "divide" ) == 0 )
		{
			results =	atof( piece_buffer_1 ) /
					atof( piece_buffer_2 );
		}
		else
		if ( strcmp( operation, "subtraction" ) == 0
		||   strcmp( operation, "subtract" ) == 0 )
		{
			results =	atof( piece_buffer_1 ) -
					atof( piece_buffer_2 );
		}
		else
		if ( strcmp( operation, "percentage" ) == 0
		||   strcmp( operation, "percent" ) == 0 )
		{
			results =	atof( piece_buffer_1 ) *
					100.0;
		}

		printf( "%s%c%lf\n", buffer, delimiter, results );
	}
}


