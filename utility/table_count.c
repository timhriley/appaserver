/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/utility/table_count.c		 		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "appaserver.h"
#include "boolean.h"
#include "folder.h"
#include "mysqldump.h"

#define BUFFER_SIZE	256

void table_count_show_tables(
		FILE *sql_pipe );

void table_count_stdin(
		FILE *sql_pipe,
		char *application_name );

void table_count_output(
		FILE *sql_pipe,
		char *folder_name,
		char *table_name );

int main( int argc, char **argv )
{
	FILE *sql_pipe;

	sql_pipe = popen( "sql_timeout.sh 1 | sed 's/\\^/=/'", "w" );

	if ( argc == 2 )
	{
		table_count_stdin(
			sql_pipe,
			argv[ 1 ] /* application_name */ );
	}
	else
	{
		table_count_show_tables( sql_pipe );
	}

	pclose( sql_pipe );
	return 0;
}

void table_count_output(
		FILE *sql_pipe,
		char *folder_name,
		char *table_name )
{
	fflush( sql_pipe );
	fprintf(sql_pipe,
		"select '%s', count(1) from %s;\n",
		folder_name,
		table_name );
	fflush( sql_pipe );
}

void table_count_stdin(
		FILE *sql_pipe,
		char *application_name )
{
	char folder_name[ 128 ];

	while( string_input( folder_name, stdin, 128 ) )
	{
		table_count_output(
			sql_pipe,
			folder_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
		       		folder_name ) );
	}
}

void table_count_show_tables( FILE *sql_pipe )
{
	char table_name[ 128 ];
	FILE *input_pipe = popen( "echo \"show tables;\" | sql.e", "r" );

	while( string_input( table_name, input_pipe, 128 ) )
	{
		table_count_output(
			sql_pipe,
			table_name /* folder_name */,
			table_name );
	}

	pclose( input_pipe );
}

