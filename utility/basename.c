/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/basename.c					*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "boolean.h"
#include "String.h"
#include "filename.h"

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
			filename_directory(
				filename ) );
	}
	else
	{
		printf( "%s\n",
			filename_basename(
				filename,	
				strip_extension ) );
	}

	return 0;
}

void basename_stdin( void )
{
	char filename[ 1024 ];

	while( string_input( filename, stdin, sizeof ( filename ) ) )
	{
		printf( "%s\n",
			filename_basename(
				filename,	
				0 /* not strip_extension */ ) );
	}
}
