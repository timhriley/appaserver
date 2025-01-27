/* utility/add_line.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int add_line( char *line, int add_after_line );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr, 
			"Usage: add_line.e \"string\" add_after_every_line\n" );
		fprintf( stderr, 
			 "Note: add_after_every_line is 0 based\n" );
		exit( 1 );
	}

	add_line( argv[ 1 ], atoi( argv[ 2 ] ) );
	return 0;
	
} /* main() */

int add_line( char *line, int add_after_line )
{
	char buffer[ 1024 ];
	int x = 0;

	while( get_line( buffer, stdin ) )
	{
		puts( buffer );
		if ( x == add_after_line )
		{
			puts( line );
			x = 0;
		}
		else
			x++;
	}
	return 1;
} /* add_line() */

