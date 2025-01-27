/* utility/commit.c */
/* ---------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	int commit_after_n_rows;
	int this_row;
	char instring[ 1024 ];

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "usage: %s commit_after_n_rows\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	commit_after_n_rows = atoi( argv[ 1 ] );
	if ( commit_after_n_rows <= 0 )
	{
		fprintf( stderr, 
			 "%s: supply a positive number to commit after.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	this_row = 0;
	while( get_line( instring, stdin ) )
	{
		printf( "%s\n", instring );
		if ( ++this_row == commit_after_n_rows )
		{
			this_row = 0;
			printf( "commit;\n" );
		}
	}

	printf( "commit;\n" );
	return 0;

} /* main() */
