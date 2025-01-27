/* findsed.c */
/* --------- */
/* Tim Riley */
/* --------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "name_arg.h"
#include "timlib.h"
#include "column.h"
#include "sed.h"

#define EXCLUDE_DIR_COMMAND	" | grep -v '/%s/'"

void fetch_parameters(	NAME_ARG *arg, 
			char **find_string,
			char **search_string,
			char **replace_string,
			char **testing_string,
			char **excludedir_string );

void build_find_string_buffer(	char *destination, 
				char *find_string,
				char *exclude_dir );

void append_exclude_dir_maybe( char *find_string_buffer, char *exclude_dir );

void sed_this_file( 	char *this_file, 
			char *search_string, 
			char *replace_string,
			char *testing_string );

void findsed(	char *find_string, 
		char *search_string, 
		char *replace_string, 
		char *testing_string, 
		char *excludedir_string );

void setup_arg( NAME_ARG *arg, int argc, char **argv );

int main( int argc, char **argv )
{
	char *find_string;
	char *search_string;
	char *replace_string;
	char *testing_string;
	char *excludedir_string;

        NAME_ARG *arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );
	fetch_parameters(	arg, 
				&find_string, 
				&search_string, 
				&replace_string, 
				&testing_string, 
				&excludedir_string );

	if ( argc == 1 )
	{
		exit_usage( arg );
	}

	findsed(	find_string, 
			search_string, 
			replace_string, 
			testing_string, 
			excludedir_string );

	if ( *testing_string == 'y' )
		printf( "Testing complete\n" );
	else
		printf( "Process complete\n" );

	return 0;

}


void fetch_parameters(	NAME_ARG *arg, 
			char **find_string,
			char **search_string,
			char **replace_string,
			char **testing_string,
			char **excludedir_string )
{
	*find_string = fetch_arg( arg, "find" );
	*search_string = fetch_arg( arg, "search" );
	*replace_string = fetch_arg( arg, "replace" );
	*testing_string = fetch_arg( arg, "testing" );
	*excludedir_string = fetch_arg( arg, "excludedir" );
}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "find" );
        set_default_value( arg, ticket, "*" );

        ticket = add_valid_option( arg, "search" );
        ticket = add_valid_option( arg, "replace" );

        ticket = add_valid_option( arg, "excludedir" );
        set_default_value( arg, ticket, "backup trash" );

        ticket = add_valid_option( arg, "testing" );
        add_valid_value( arg, ticket, "yes" );
        add_valid_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "yes" );

        ins_all( arg, argc, argv );

}


void findsed(	char *find_string, 
		char *search_string, 
		char *replace_string, 
		char *testing_string, 
		char *excludedir_string )
{
	FILE *p;
	char this_file[ 512 ];
	char find_string_buffer[ 512 ];

	build_find_string_buffer(	find_string_buffer, 
					find_string, 
					excludedir_string );

	p = popen( find_string_buffer, "r" );

	while( get_line( this_file, p ) )
	{
		printf( "%s\n", this_file );
		sed_this_file(	this_file, 
				search_string, 
				replace_string,
				testing_string );
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
		fprintf( stderr, "findsed() has no regular expressions!\n" );
		exit( 1 );
	}

	ptr += sprintf( ptr, "find . -type f \\(" );

	for( i = 0; i < number_of_regular_expressions; i++ )
	{
		if ( i )
		{
			ptr += sprintf( ptr, " -o" );
		}

		column( column_buffer, i, find_string );

		ptr += sprintf( ptr, " -name '%s'", column_buffer );
	}

	sprintf( ptr, " \\) -xdev -print" );
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

void sed_this_file( 	char *this_file, 
			char *search_string, 
			char *replace_string,
			char *testing_string )
{
	char sys_string[ 512 ];
	char tmpfile_string[ 512 ];
	FILE *input_file;
	FILE *output_file = {0};
	char input_buffer[ 4096 ];
	SED *sed;
	register int line_number = 0;

	if ( !*search_string ) return;

	if ( *testing_string == 'n' )
	{
		sprintf( tmpfile_string, "/tmp/findsed_%d.dat", getpid() );
		output_file = fopen( tmpfile_string, "w" );
	
		if ( !output_file )
		{
			fprintf(stderr,
				"ERROR in %s/%s(): cannot open %s for write\n",
				__FILE__,
				__FUNCTION__,
				tmpfile_string );
			exit( 1 );
		}
	}

	input_file = fopen( this_file, "r" );

	if ( !input_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s for read\n",
			__FILE__,
			__FUNCTION__,
			this_file );
		exit( 1 );
	}

	sed = new_sed( search_string, replace_string );

	while( get_line( input_buffer, input_file ) )
	{
		line_number++;

		if ( sed_will_replace( input_buffer, sed ) )
		{
			printf( "%6d before: %s\n", line_number, input_buffer );
			sed_search_replace( input_buffer, sed );
			printf( "%6d after:  %s\n", line_number, input_buffer );
		}

		if ( *testing_string == 'n' )
		{
			fprintf( output_file, "%s\n", input_buffer );
		}
	}

	if ( *testing_string == 'n' )
	{
		fclose( output_file );
		sprintf(	sys_string, 
				"/bin/cp %s %s", 
				tmpfile_string, 
				this_file );

		system( sys_string );

		sprintf(	sys_string, 
				"/bin/rm %s", 
				tmpfile_string );
		system( sys_string );
	}

	fclose( input_file );
	free( sed );

}

