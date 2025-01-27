/* utility/mail_if_output.c	       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

void mail_if_output(	char *subject,
			char **address_list );

void build_sys_string(	char *sys_string,
			char *subject,
			char **address_list );

int main( int argc, char **argv )
{
	if ( argc < 3 )
	{
		fprintf( stderr,
			 "Usage: %s subject address_1 [... address_n]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	mail_if_output( argv[ 1 ], &argv[ 2 ] );

	return 0;

} /* main() */

void mail_if_output(	char *subject,
			char **address_list )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *output_pipe = {0};

	build_sys_string( sys_string, subject, address_list );

	while( get_line( input_buffer, stdin ) )
	{
		if ( output_pipe )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}
		else
		if ( *input_buffer )
		{
			output_pipe = popen( sys_string, "w" );
			fprintf( output_pipe, "%s\n", input_buffer );
		}
	}

	if ( output_pipe ) pclose( output_pipe );

} /* mail_if_output() */

void build_sys_string(	char *sys_string,
			char *subject,
			char **address_list )
{
	char *ptr = sys_string;

	ptr += sprintf(	ptr,
			"mail -s \"%s\" %s",
			subject,
			*address_list++ );

	while( *address_list ) ptr += sprintf( ptr, " %s", *address_list++ );

} /* build_sys_string() */

