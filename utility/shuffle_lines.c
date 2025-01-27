/* shuffle_lines.c				*/
/* -------------------------------------------- */
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define BUFFER_SIZE		4096

void shuffle_lines( char delimiter, FILE *file1, FILE *file2 );

void shuffle_lines( char delimiter, FILE *file1, FILE *file2 )
{
	char buffer1[ BUFFER_SIZE ];
	char buffer2[ BUFFER_SIZE ];

	while ( get_line( buffer1, file1 ) )
	{
		if ( get_line( buffer2, file2 ) )
		{
			printf( "%s%c%s\n",
				buffer1, delimiter, buffer2 );
		}
	}
} /* shuffle_lines() */


int main( int argc, char **argv )
{
	FILE *file1, *file2;
	char delimiter;

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s file1 file2 delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	if ( ! ( file1 = fopen( argv[ 1 ], "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s: Cannot open %s\n",
			 argv[ 0 ],
			 argv[ 1 ] );
		exit( 1 );
	}

	if ( ! ( file2 = fopen( argv[ 2 ], "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s: Cannot open %s\n",
			 argv[ 0 ],
			 argv[ 2 ] );
		exit( 1 );
	}

	delimiter = *argv[ 3 ];

	shuffle_lines( delimiter, file1, file2 );

	fclose( file1 );
	fclose( file2 );

	return 0;

} /* main() */

