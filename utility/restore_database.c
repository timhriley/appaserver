/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/restore_database.c	       			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "restore_database.h"

char *restore_database_get_sql_statement_process_string(
		char *data_process_string,
		char *table_name,
		LIST *column_name_list,
		char delimiter )
{
	char column_name_list_string[ 4096 ];
	static char sql_statement_process_string[ 5000 ];

	list_buffered_display(
		column_name_list_string,
		column_name_list, 
		',' );

	snprintf(
		sql_statement_process_string,
		sizeof ( sql_statement_process_string ),
		"%s | insert_statement.e %s %s '%c'",
		data_process_string,
		table_name,
		column_name_list_string,
		delimiter );

	return sql_statement_process_string;
}

LIST *restore_database_get_column_name_list( char *describe_process )
{
	FILE *input_pipe;
	LIST *column_name_list = list_new_list();
	char input_buffer[ 1024 ];

	input_pipe = popen( describe_process, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		list_append_pointer(	column_name_list,
					strdup( input_buffer ) );
	}

	pclose( input_pipe );
	return column_name_list;
}

RESTORE_DATABASE *restore_database_new_restore_database(
			char *configuration_filename )
{
	RESTORE_DATABASE *r;

	r = (RESTORE_DATABASE *)calloc( 1, sizeof( RESTORE_DATABASE ) );
	r->configuration_filename = configuration_filename;
	return r;
}

RESTORE_TABLE *restore_database_new_restore_table( char *table_name )
{
	RESTORE_TABLE *r;

	r = (RESTORE_TABLE *)calloc( 1, sizeof( RESTORE_TABLE ) );
	r->table_name = table_name;
	return r;
}

int restore_database_load_configuration_file(
			LIST **restore_table_list,
			char *configuration_filename )
{
	FILE *input_file;
	LIST *local_restore_table_list = list_new_list();
	RESTORE_TABLE *restore_table = {0};
	char input_buffer[ 4096 ];
	char piece_buffer[ 4096 ];

	input_file = fopen( configuration_filename, "r" );

	if ( !input_file ) return 0;

	while( get_line( input_buffer, input_file ) )
	{
		if ( !*input_buffer || *input_buffer == '#' ) continue;

		piece( piece_buffer, '=', input_buffer, 0 );

		if ( strcmp( piece_buffer, "table" ) == 0 )
		{
			if ( piece( piece_buffer, '=', input_buffer, 1 ) )
			{
				if ( restore_table )
				{
					list_append_pointer(
						local_restore_table_list,
						(char *)restore_table );
				}

				restore_table =
					restore_database_new_restore_table(
						strdup( piece_buffer ) );

			}
		}
		else
		if ( strcmp( piece_buffer, "describe_process" ) == 0 )
		{
			if ( piece( piece_buffer, '=', input_buffer, 1 ) )
			{
				restore_table->column_name_list =
					restore_database_get_column_name_list(
						piece_buffer );
			}
		}
		else
		if ( strcmp( piece_buffer, "data_process" ) == 0 )
		{
			if ( piece( piece_buffer, '=', input_buffer, 1 ) )
			{
				restore_table->data_process_string =
						strdup( piece_buffer );

			}
		}
		else
		if ( strcmp( piece_buffer, "delimiter" ) == 0 )
		{
			if ( piece( piece_buffer, '=', input_buffer, 1 ) )
			{
				restore_table->delimiter = *piece_buffer;

			}
		}
	} /* while get_line() */

	if ( restore_table )
	{
		list_append_pointer(	local_restore_table_list,
					(char *)restore_table );
	}

	fclose( input_file );
	*restore_table_list = local_restore_table_list;
	return 1;
}

int main( int argc, char **argv )
{
	RESTORE_DATABASE *restore_database;
	RESTORE_TABLE *restore_table;

	if ( argc != 2 )
	{
		fprintf(stderr, "Usage: %s configuration_file\n", argv[ 0 ] );
		exit( 1 );
	}

	restore_database =
		restore_database_new_restore_database(
			argv[ 1 ] );

	if ( !restore_database_load_configuration_file(
					&restore_database->
						restore_table_list,
					restore_database->
						configuration_filename ) )
	{
		fprintf(stderr,
			"Error in %s: cannot load configuration file = %s\n",
			argv[ 1 ], restore_database->configuration_filename );
		exit( 1 );
	}

	if ( !list_rewind( restore_database->restore_table_list ) )
		exit( 0 );

	do {
		restore_table =
			(RESTORE_TABLE *)
				list_get_pointer(
					restore_database->
						restore_table_list );

		restore_database->sql_statement_process_string =
			restore_database_get_sql_statement_process_string(
				restore_table->data_process_string,
				restore_table->table_name,
				restore_table->column_name_list,
				restore_table->delimiter );

		if ( system(
			restore_database->
				sql_statement_process_string ) ){}

		list_free_string_list( restore_table->column_name_list );

	} while( list_next( restore_database->restore_table_list ) );

	return 0;
}

