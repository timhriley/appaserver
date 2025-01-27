/* $APPASERVER_HOME/utility/table.c	*/
/* ------------------------------------ */
/* Utility program for TABLE ADT       	*/
/* ------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "timlib.h"
#include "name_arg.h"

/* Prototypes */
/* ---------- */
void setup_arg( NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	NAME_ARG *arg, 
			char **config_filename, 
			char **input_filename, 
			char **optional_title,
			char **optional_title_continued,
			char **heading_off );

int main( int argc, char **argv )
{
        NAME_ARG *arg;
	TABLE *table;
	FILE *input_file;
	char *config_filename;
	char *input_filename;
	char *optional_title;
	char *optional_title_continued;
	char *heading_off;

	arg = init_arg( argv[ 0 ] );
	setup_arg( arg, argc, argv );
	fetch_parameters(	arg, 
				&config_filename, 
				&input_filename, 
				&optional_title,
				&optional_title_continued,
				&heading_off );
	if ( strcmp( input_filename, "stdin" ) == 0 )
		input_file = stdin;
	else
		input_file = open_file( input_filename, "r" );

	table = table_new();

	load_configuration_file( table, config_filename );

	if ( optional_title ) 
		set_title( table, optional_title );
	if ( optional_title_continued ) 
		set_title_continued( table, optional_title_continued );
	if ( *heading_off ) 
		set_heading_off( table );

	load_data_labeled( table, input_file );
	table_display( stdout, table );

	return 0;

}

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "config_file" );
        ticket = add_valid_option( arg, "input_file" );

        ticket = add_valid_option( arg, "title" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "title_continued" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "heading_off" );
	add_possible_value( arg, ticket, "yes" );
        set_default_value( arg, ticket, "" );

        ins_all( arg, argc, argv );

}

void fetch_parameters(	NAME_ARG *arg, 
			char **config_filename, 
			char **input_filename, 
			char **optional_title,
			char **optional_title_continued,
			char **heading_off )
{
	*config_filename = fetch_arg( arg, "config_file" );
	*input_filename = fetch_arg( arg, "input_file" );
	*optional_title = fetch_arg( arg, "title" );
	*optional_title_continued = fetch_arg( arg, "title_continued" );
	*heading_off = fetch_arg( arg, "heading_off" );
}

