/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/rename_table.c			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"
#include "appaserver.h"
#include "rename_table.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *old_table;
	char *table_name;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	RENAME_TABLE *rename_table;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s process old_tabel table_name execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	old_table = argv[ 2 ];
	table_name = argv[ 3 ];
	execute = ( *argv[ 4 ] == 'y' );

	appaserver_parameter = appaserver_parameter_new();

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !*old_table
	||   strcmp( old_table, "old_table" ) == 0 )
	{
		printf( "<h3>Please enter in an old table name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	if ( !*table_name
	||   strcmp( table_name, "table_name" ) == 0 )
	{
		printf( "<h3>Please select a new table.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	rename_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rename_table_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				old_table /* old_folder_name */ ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				table_name /* new_folder_name */ ),
			appaserver_parameter->data_directory );

	printf( "<p>%s\n",
		rename_table->
			execute_system_string );

	printf( "<p>%s\n",
		rename_table->
			drop_index_system_string );

	printf( "<p>%s\n",
		rename_table->
			create_table_primary_index_system_string );

	if ( execute )
	{
		char *result;

		fflush( stdout );
		result =
			string_pipe_fetch(
				rename_table->execute_system_string );

		if ( result ) printf( "<p>%s\n", result );

		if ( system( rename_table->
				drop_index_system_string ) ){}

		if ( system( rename_table->
				create_table_primary_index_system_string ) ){}

		process_increment_count( process_name );

		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		FILE *output_file;

		output_file =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_output_file(
				rename_table->process_filename );

		fprintf(output_file,
			"%s\n",
			rename_table->shell_script );

		fclose( output_file );

		if ( system(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_execute_bit_system_string(
				rename_table->process_filename ) ) ){}

		printf( "<h3>Created: %s</h3>\n",
			rename_table->process_filename );
	}

	document_close();

	return 0;
}
