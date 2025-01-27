/* decimal_count.c					*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	unsigned count_up_to;
	unsigned count;
	char one_line_yn = 'n';
	char input_buffer[ 1024 ];

	if ( argc < 2 )
	{
		fprintf( stderr,
		"Usage: %s count_up_to [one_line_yn]\n", argv[ 0 ] );
		return 1;
	}

	count_up_to = atoi( argv[ 1 ] );

	if ( argc == 3 ) one_line_yn = *argv[ 2];

	if ( !count_up_to )
	{
		count = 0;

		while ( timlib_get_line( input_buffer, stdin, 1024 ) )
		{
			printf( "%s_%d\n", input_buffer, ++count );
		}

	}
	else
	{
		for ( count = 1; count <= count_up_to; count++ )
		{
			if ( one_line_yn == 'y' )
			{
				if ( count != 1 ) printf( "," );
	
				printf( "%d", count );
			}
			else
			{
				printf( "%d\n", count );
			}
		}
	}

	if ( one_line_yn == 'y' ) printf( "\n" );

	return 0;
}
