/* utility/statistics_on_group.c		*/
/* -------------------------------------------- */
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

#define STATISTICS_PROCESS "statistics.e 0 0 '' 2"

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char group[ 1024 ];
	char old_group[ 1024 ];
	char value[ 1024 ];
	int first_time = 1;
	FILE *p = {0};

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s output_heading\n", argv[ 0 ] );
		exit( 1 );
	}

	while( get_line( input_buffer, stdin ) )
	{
		piece( group, '|', input_buffer, 0 );

		if ( first_time )
		{
			p = popen( STATISTICS_PROCESS, "w" );
			strcpy( old_group, group );
			printf( "%s: %s\n", argv[ 1 ], old_group );
			fflush( stdout );
			first_time = 0;
		}
		else
		{
			if ( strcmp( old_group, group ) != 0 )
			{
				pclose( p );
				p = popen( STATISTICS_PROCESS, "w" );
				strcpy( old_group, group );
				printf( "%s: %s\n", argv[ 1 ], old_group );
				fflush( stdout );
			}
		}
		if ( piece( value, '|', input_buffer, 1 ) )
		{
			fprintf( p, "%s\n", value );
			fflush( p );
		}
	}
	if ( p ) pclose( p );
	return 0;
} /* main() */

