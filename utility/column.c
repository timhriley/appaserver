/* $APPASERVER_HOME/utility/column.c */
/* --------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"

int main( int argc, char **argv )
{
	char buffer[ 8192 ];
	char component[ 8192 ];
	char offset_piece[ 20 ];
	int i, offset;

	if ( argc != 2 )
	{
		fprintf( stderr, "usage: column.e offset_1[,offset_n...]\n" );
		exit( 1 );
	}

	/* For each input line */
	/* ------------------- */
	while( get_line( buffer, stdin ) )
	{
		/* For each column requested */
		/* ------------------------- */
		for( i = 0; piece( offset_piece, ',', argv[ 1 ], i ); i++ )
		{
			offset = atoi( offset_piece );

			/* Get the column */
			/* -------------- */
			if ( !column(	component, 
					offset, 
					buffer ) )
			{
/*
				fprintf( stderr, 
				"Warning: Not enough columns in: (%s)\n",
				 	buffer );
				printf( "%s\n", buffer );
*/
				continue;
			}

			if ( i == 0 )
				printf( "%s", component );
			else
				printf( " %s", component );
		}
		printf( "\n" );
		fflush( stdout );
	}
	return 0;
}
