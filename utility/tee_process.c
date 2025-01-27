/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/tee_process.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char input_buffer[ 65536 ];
	FILE *output_pipe;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s \"command_string\"\n",
			argv[ 0 ] );
		exit( 1 );
	}

	output_pipe = popen( argv[ 1 ], "w" );

	if ( !output_pipe )
	{
		fprintf(stderr,
			"ERROR %s: cannot open pipe\n",
			argv[ 0 ] );
		exit( 1 );
	}

	while( string_input( input_buffer, stdin, 65536 ) )
	{
		fprintf( output_pipe, "%s\n", input_buffer );
		fflush( output_pipe );

		printf( "%s\n", input_buffer );
		fflush( stdout );
	}

	pclose( output_pipe );
	return 0;
}
