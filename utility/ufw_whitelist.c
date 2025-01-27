/* $APPASERVER_HOME/utility/ufw_whitelist.c				*/
/* -------------------------------------------------------------------- */
/* Note: links to ufw_blacklist.e					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "fopen_path.h"

#define WHITELIST_FILE	"ufw_whitelist.dat"
#define BLACKLIST_FILE	"ufw_blacklist.dat"

int main( int argc, char **argv )
{
	FILE *input_file;
	char buffer[ 1024 ];
	FILE *output_pipe;
	char *input_filename;

	/* stub */
	/* ---- */
	if ( ( argc = 1 ) ){}

	if ( strcmp( argv[ 0 ], "ufw_blacklist.e" ) == 0 )
		input_filename = BLACKLIST_FILE;
	else
		input_filename = WHITELIST_FILE;

	output_pipe = popen( "sort -n", "w" );

	if ( ( input_file = fopen_path( input_filename, "r" ) ) )
	{
		while( get_line( buffer, input_file ) )
		{
			fprintf( output_pipe, "%s\n", buffer );

		}
		pclose( output_pipe );
	}

	return 0;
}

