/* append_radio_html.c		*/
/* ---------------------------- */
/* 				*/
/* Tim Riley			*/
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define MAX_SIZE	4096

int main( int argc, char **argv )
{
	char input_buffer[ MAX_SIZE + 1 ];
	int prepend = 0;
	int value = 0;
	int exit_usage = 0;

	if ( argc < 3 )
	{
		exit_usage = 1;
	}
	if ( argc == 3 && strcmp( argv[ 2 ], "prepend" ) != 0 )
	{
		exit_usage = 1;
	}
	else
		prepend = 1;

	if ( exit_usage )
	{
		fprintf( stderr, 
			 "Usage: %s variable_name [prepend]\n", 
			 argv[ 0 ] );
		exit( 1 );
	}


	while( get_line( input_buffer, stdin ) )
	{
		if ( prepend )
		{
			printf( 
		"<TD><INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%d\"%s\n",
			argv[ 1 ],
			value++,
			input_buffer );
		}
		else
		{
			printf( 
		"%s<TD><INPUT TYPE=\"radio\" NAME=\"%s\" VALUE=\"%d\"\n",
			input_buffer,
			argv[ 1 ],
			value++ );
		}
	}

	return 0;
} /* main() */

