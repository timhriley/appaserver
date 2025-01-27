/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/create_table.c 		     		*/
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
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"
#include "create_table.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	boolean execute_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	CREATE_TABLE *create_table;

	application_name = environment_exit_application_name( argv[ 0 ] );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process folder execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	execute_boolean = ( *argv[ 3 ] == 'y' );

	appaserver_error_argv_append_file( argc, argv, application_name );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !*folder_name
	||   strcmp( folder_name, "folder" ) == 0 )
	{
		printf( "<h3>Please enter in a folder.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	create_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		create_table_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				folder_name ),
			execute_boolean,
			appaserver_parameter->data_directory );

	if ( execute_boolean )
	{
		if ( system( create_table->shell_filename ) ){}
		process_increment_count( process_name );
		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		list_html_table_display(
			create_table->sql_statement_list,
			"SQL Statement" /* heading_string */ );

		printf( "<h3>Created %s</h3>\n",
			create_table->shell_filename );
	}

	document_close();

	return 0;
}
