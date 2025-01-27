/* joinfiles.c					*/
/* -------------------------------------------- */
/* Input:    1.	delimiter (command line)	*/
/* 	     2.	two filenames			*/
/* Output:	a single line containing	*/
/*		each line of both files.	*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

#define BUFFER_SIZE 4096

void joinfiles( char *file1_name, char *file2_name, char delimiter  );

void joinfiles( char *file1_name, char *file2_name, char delimiter  )
{
	FILE *file1, *file2;
	char buffer1[ BUFFER_SIZE ];
	char buffer2[ BUFFER_SIZE ];

	file1 = open_file( file1_name, "r" );
	file2 = open_file( file2_name, "r" );

	while( get_line( buffer1, file1 ) )
	{
		if ( !get_line( buffer2, file2 ) )
		{
			printf( "%s\n", buffer1 );
		}
		else
		{
			printf( "%s%c%s\n", buffer1, delimiter, buffer2 );
		}
	}
	fclose( file1 );
	fclose( file2 );

} /* joinfiles() */


int main( int argc, char **argv )
{
	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s file1 file2 delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	joinfiles( argv[ 1 ], argv[ 2 ], *argv[ 3 ] );
	return 0;

} /* main() */

