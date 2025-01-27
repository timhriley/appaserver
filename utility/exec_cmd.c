/* /usr2/utility/exec_cmd.c */
/* ------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "variable.h"

void execute_command( char *command, int debug_mode );
int get_record_from_stdin( VARIABLE *v );

int main( int argc, char **argv )
{
	int debug = 0;
	int exit_usage = 0;

	if ( argc < 2 )
	{
		exit_usage = 1;
	}

	if ( argc == 3 )
	{
		if ( strcmp( argv[ 2 ], "debug" ) != 0 )
			exit_usage = 1;
		else
			debug = 1;
	}
	else
		debug = 0;

	if ( exit_usage )
	{
		fprintf(stderr,
			"Usage: exec_cmd.e 'command_and_arguments' [debug]\n" );
		exit( 1 );
	}

	execute_command( argv[ 1 ], debug );

	return 0;

} /* main() */


void execute_command( char *command, int debug_mode )
{
	VARIABLE *v;
	char sys_str[ 1024 ];

	while( 1 )
	{
		v = variable_init();

		if ( !get_record_from_stdin( v ) )
		{
			return;
		}

		replace_all_variables( sys_str, command, v );

		if ( debug_mode ) fprintf( stderr, "%s\n", sys_str );
		system( sys_str );

		variable_free( v );
	}

} /* execute_command() */

int get_record_from_stdin( VARIABLE *v )
{
	char buffer[ 512 ];
	char variable[ 128 ], value[ 128 ];

	while( get_line( buffer, stdin ) )
	{
		if ( !*buffer ) continue;

		if ( strcmp( buffer, ".begin" ) == 0 ) continue;

		if ( strcmp( buffer, ".end" ) == 0 ) return 1;

		piece( variable, '|', buffer, 0 );

		if ( !piece( value, '|', buffer, 1 ) )
		{
			fprintf( stderr, 
			"get_record_from_stdin() cannot parse field: (%s)\n",
				buffer );
			exit( 1 );
		}
		set_variable( v, strdup( variable ), strdup( value ) );
	}

	return 0;

} /* get_record_from_stdin() */
