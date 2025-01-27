/* dot_process2option_value.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"

#define DOT_PROCESS_TAG	".process="

void execute_dot_process( char *process_string );

int main( int argc, char **argv )
{
	char *application;
	char input_string[ 1024 ];
	char process_string[ 1024 ];
	char replace_application[ 128 ];
	int dot_process_tag_length;

	application = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application );

	dot_process_tag_length = strlen( DOT_PROCESS_TAG );
	sprintf( replace_application, " %s", application );

	while( get_line( input_string, stdin ) )
	{
		if ( strncmp(	input_string,
				DOT_PROCESS_TAG,
				dot_process_tag_length ) == 0 )
		{
			piece(	process_string,
				'=',
				input_string,
				1 );

			search_replace_string(
				process_string,
				" application",
				replace_application );

			execute_dot_process( process_string );
		}
		else
		{
			printf( "%s\n", input_string );
		}
	}

	return 0;
}

void execute_dot_process( char *process_string )
{
	char input_buffer[ 1024 ];
	char format_buffer[ 1024 ];
	char key_buffer[ 1024 ];
	FILE *input_pipe;

	input_pipe = popen( process_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( character_exists( input_buffer, '^' ) )
		{
			piece(	format_buffer,
				'^',
				input_buffer,
				0 );

			format_initial_capital( format_buffer, format_buffer );

			piece(	key_buffer,
				'^',
				input_buffer,
				1 );
		}
		else
		{
			format_initial_capital( format_buffer, input_buffer );
			strcpy( key_buffer, input_buffer );
		}
		printf( "<option value=\"%s\">%s\n",
			key_buffer,
			format_buffer );
	}
	pclose( input_pipe );
}
