/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/clone_folder.c      			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "security.h"
#include "clone_folder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	char *attribute_name;
	char *old_data;
	char *new_data;
	boolean delete_boolean;
	boolean execute_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	CLONE_FOLDER *clone_folder;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s process folder attribute old_data new_data delete_yn execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	attribute_name = argv[ 3 ];
	old_data = argv[ 4 ];
	new_data = argv[ 5 ];
	delete_boolean = (*argv[ 6 ] == 'y');
	execute_boolean = (*argv[ 7 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	clone_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		clone_folder_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				folder_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				attribute_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				old_data ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				new_data ),
			delete_boolean,
			appaserver_parameter->data_directory );

	if ( !execute_boolean )
	{
		clone_folder_display(
			clone_folder->
				sql_statement_list );
	}
	else
	{
		if ( list_length( clone_folder->sql_statement_list ) )
		{
			clone_folder_execute(
				clone_folder->
					sql_statement_list );

			process_execution_count_increment( process_name );
			printf( "<h3>Process complete.</h3>\n" );
		}
		else
		{
			printf( "<h3>Nothing to process.</h3>\n" );
		}
	}

	return 0;
}
