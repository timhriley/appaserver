/* utility/basename.c					*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "basename.h"
#include "boolean.h"

void basename_stdin( void );

int main( int argc, char **argv )
{
	char *filename;
	boolean strip_extension;
	boolean directory = 0;

	if ( argc == 1 )
	{
		basename_stdin();
		exit( 0 );
	}

	if ( argc < 3 )
	{
		fprintf( stderr,
		"Usage: %s filename strip_extension_yn [directory_yn]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	filename = argv[ 1 ];
	strip_extension = ( *argv[ 2 ] == 'y' );

	if ( argc == 4 )
		directory = ( *argv[ 3 ] == 'y' );

	if ( directory )
	{
		printf( "%s\n",
			basename_get_directory(
				filename ) );
	}
	else
	{
		printf( "%s\n",
			basename_get_base_name(
				filename,	
				strip_extension ) );
	}

	return 0;

} /* main() */

void basename_stdin( void )
{
	char filename[ 1024 ];

	while( get_line( filename, stdin ) )
	{
		printf( "%s\n",
			basename_get_base_name(
				filename,	
				0 /* not strip_extension */ ) );
	}
}
