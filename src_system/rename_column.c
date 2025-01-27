/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/rename_column.c     			*/
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
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "process.h"
#include "security.h"
#include "rename_column.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *old_column;
	char *table_name;
	char *column_name;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	RENAME_COLUMN *rename_column;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s process old_column table_name column_name execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	old_column = argv[ 2 ];
	table_name = argv[ 3 ];
	column_name = argv[ 4 ];
	execute = ( *argv[ 5 ] == 'y' );

	appaserver_parameter = appaserver_parameter_new();

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !*old_column
	||   strcmp( old_column, "old_column" ) == 0 )
	{
		printf( "<h3>Please enter in an old column name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	if ( !*column_name
	||   strcmp( column_name, "column_name" ) == 0 )
	{
		printf( "<h3>Please select a Table/Column name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	rename_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rename_column_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				table_name ) /* folder_name */,
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				old_column ) /* old_attribute_name */,
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				column_name ) /* new_attribute_name */,
			appaserver_parameter->data_directory );

	printf( "<p>%s\n", rename_column->execute_system_string );

	if ( execute )
	{
		char *results;

		fflush( stdout );
		results =
			string_pipe_fetch(
				rename_column->execute_system_string );

		if ( results ) printf( "<p>%s\n", results );

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
				rename_column->process_filename );

		fprintf(output_file,
			"%s\n",
			rename_column->shell_script );

		fclose( output_file );

		if ( system(
			appaserver_execute_bit_system_string(
				rename_column->process_filename ) ) ){}

		printf( "<h3>Created: %s</h3>\n",
			rename_column->process_filename );
	}

	document_close();

	return 0;
}
