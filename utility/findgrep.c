/* $APPASERVER_HOME/utility/findgrep.c			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "name_arg.h"
#include "timlib.h"
#include "column.h"
#include "sed.h"
#include "boolean.h"

#define EXCLUDE_DIR_COMMAND	" | grep -v '/%s/'"
#define BUFFER_SIZE		131072

void fetch_parameters(	NAME_ARG *arg, 
			char **find_string,
			char **grep_string,
			char **excludedir_string,
			boolean *execute_vi,
			boolean *supress_output );

void build_find_string_buffer(	char *destination, 
				char *find_string,
				char *exclude_dir );

void append_exclude_dir_maybe( char *find_string_buffer, char *exclude_dir );

void grep_this_file( 	char *vi_string,
			char *this_file,
			char *grep_string,
			boolean execute_vi,
			boolean supress_output );

void findgrep(	char *vi_string,
		char *find_string, 
		char *grep_string, 
		boolean execute_vi,
		boolean supress_output,
		char *excludedir_string );

void setup_arg( NAME_ARG *arg, int argc, char **argv );

int main( int argc, char **argv )
{
	char *find_string;
	char *grep_string;
	char *excludedir_string;
	boolean execute_vi = 0;
	boolean supress_output = 0;
	char vi_string[ BUFFER_SIZE ];

        NAME_ARG *arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(	arg, 
				&find_string, 
				&grep_string, 
				&excludedir_string,
				&execute_vi,
				&supress_output );

	if ( argc == 1 )
	{
		exit_usage( arg );
	}

	if ( execute_vi ) strcpy( vi_string, "vi" );

	findgrep(	vi_string,
			find_string, 
			grep_string, 
			execute_vi,
			supress_output,
			excludedir_string );

	if ( execute_vi )
	{
		if ( strcmp( vi_string, "vi" ) == 0 )
		{
			printf( "There were no matching patterns\n" );
		}
		else
		{
			if ( system( vi_string ) ){}
		}
	}

	return 0;

}

void fetch_parameters(	NAME_ARG *arg, 
			char **find_string,
			char **grep_string,
			char **excludedir_string,
			boolean *execute_vi,
			boolean *supress_output )
{
	*find_string = fetch_arg( arg, "find" );
	*grep_string = fetch_arg( arg, "grep" );
	*excludedir_string = fetch_arg( arg, "excludedir" );
	*execute_vi = ( ( * ( fetch_arg( arg, "vi" ) ) ) == 'y' );

	*supress_output =
		( ( * ( fetch_arg( arg, "supress_output" ) ) ) == 'y' );

}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "find" );
        set_default_value( arg, ticket, "*" );

        ticket = add_valid_option( arg, "grep" );

        ticket = add_valid_option( arg, "excludedir" );
        set_default_value( arg, ticket, "backup trash" );

        ticket = add_valid_option( arg, "vi" );
	add_valid_value( arg, ticket, "y" );
	add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ticket = add_valid_option( arg, "supress_output" );
	add_valid_value( arg, ticket, "y" );
	add_valid_value( arg, ticket, "n" );
        set_default_value( arg, ticket, "n" );

        ins_all( arg, argc, argv );

}

void findgrep(	char *vi_string,
		char *find_string, 
		char *grep_string, 
		boolean execute_vi,
		boolean supress_output,
		char *excludedir_string )
{
	FILE *p;
	char this_file[ 512 ];
	char find_string_buffer[ 512 ];

	build_find_string_buffer(
		find_string_buffer, 
		find_string, 
		excludedir_string );

	p = popen( find_string_buffer, "r" );

	while( timlib_get_line( this_file, p, BUFFER_SIZE ) )
	{
		grep_this_file(	vi_string,
				this_file, 
				grep_string,
				execute_vi,
				supress_output );
	}
	pclose( p );
}

void build_find_string_buffer(	char *destination, 
				char *find_string,
				char *exclude_dir )
{
	char *ptr = destination;
	int i;
	int number_of_regular_expressions = count_columns( find_string );
	char column_buffer[ 512 ];

	if ( number_of_regular_expressions == 0 )
	{
		fprintf( stderr,
			 "%s() has no regular expressions!\n",
			 __FUNCTION__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "find . -xdev -type f \\(" );

	for( i = 0; i < number_of_regular_expressions; i++ )
	{
		if ( i )
		{
			ptr += sprintf( ptr, " -o" );
		}

		column( column_buffer, i, find_string );

		ptr += sprintf( ptr, " -name '%s'", column_buffer );
	}

	sprintf( ptr, " \\) -print" );
	append_exclude_dir_maybe( destination, exclude_dir );
}

void append_exclude_dir_maybe( char *find_string_buffer, char *exclude_dir )
{
	int str_len;
	char exclude_this[ 128 ];
	int number_of_columns = count_columns( exclude_dir );

	while ( number_of_columns-- )
	{
		column( exclude_this, number_of_columns, exclude_dir );

		str_len = strlen( find_string_buffer );

		sprintf( find_string_buffer + str_len,
			 EXCLUDE_DIR_COMMAND,
			 exclude_this );
	}

}

void grep_this_file( 	char *vi_string,
			char *this_file, 
			char *grep_string,
			boolean execute_vi,
			boolean supress_output )
{
	FILE *input_file;
	char input_buffer[ BUFFER_SIZE ];
	SED *sed;
	int first_time = 1;
	register int line_number = 0;

	if ( !*grep_string ) return;

	input_file = fopen( this_file, "r" );

	if ( !input_file )
	{
		fprintf(stderr,
			"Warning in %s/%s(): cannot open %s for read\n",
			__FILE__,
			__FUNCTION__,
			this_file );
		return;
	}

	sed = new_sed( grep_string, (char *)0 );

	while( get_line( input_buffer, input_file ) )
	{
		line_number++;

		if ( sed_will_replace( input_buffer, sed ) )
		{
			if ( first_time )
			{
				printf( "%s\n", this_file );

				if ( execute_vi )
				{
					int str_len = strlen( vi_string );
					if ( str_len +
					     strlen( this_file ) + 1 > 
					     BUFFER_SIZE )
					{
						fprintf( stderr, 
					"There are too many files to vi\n" );
						exit( 1 );
					}
	
					sprintf( vi_string + str_len,
						 " %s", 
						 this_file );
				}
				first_time = 0;
			}

			if ( !supress_output )
			{
				printf( "%6d: %s\n",
					line_number,
					input_buffer );
			}
		}
	}

	fclose( input_file );
	free( sed );
}

