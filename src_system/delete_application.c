/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/delete_application.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "application.h"
#include "session.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "process.h"
#include "document.h"
#include "security.h"
#include "application_create.h"
#include "application_delete.h"

int main( int argc, char **argv )
{
	char *current_application_name;
	char *delete_application_name;
	char *process_name;
	boolean execute_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	APPLICATION_DELETE *application_delete;

	current_application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		current_application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process delete_application execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	delete_application_name = argv[ 2 ];
	execute_boolean = (*argv[ 3 ] == 'y');

	if ( strcmp(	current_application_name,
			APPLICATION_TEMPLATE_NAME ) != 0
	&&   strcmp(	current_application_name,
			APPLICATION_ADMIN_NAME ) != 0 )
	{
		char message[ 128 ];

		sprintf(message,
			"current_application_name must be %s or %s.",
			APPLICATION_TEMPLATE_NAME,
			APPLICATION_ADMIN_NAME );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( process_name, "process" ) != 0 )
	{
		document_process_output(
			current_application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title_string */ );
	}

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	if ( !*delete_application_name
	||   strcmp(	delete_application_name,
			"delete_application" ) == 0 )
	{
		printf(
		"<h3>Please enter a delete application name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	application_delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_delete_new(
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				delete_application_name ),
			appaserver_parameter->document_root,
			appaserver_parameter->data_directory,
			appaserver_parameter->upload_directory,
			appaserver_parameter->log_directory,
			appaserver_parameter->mount_point );

	if ( application_delete->application_name_invalid
	||   !application_delete->application_exists_boolean )
	{
		if ( application_delete->application_name_invalid )
		{
			printf(
			"<h3>ERROR: invalid application name.</h3>\n" );
		}
		else
		{
			printf(
			"<h3>ERROR: The application doesn't exist.</h3>\n" );
		}

		if ( strcmp( process_name, "process" ) != 0 )
		{
			/* ------------------------ */
			/* Both </body> and </html> */
			/* ------------------------ */
			document_close();
		}

		exit( 1 );
	}

	application_delete_system(
		execute_boolean,
		application_delete->
			application_log->
			delete_system_string );

	application_delete_system(
		execute_boolean,
		application_delete->
			application_backup->
			delete_file_system_string );

	application_delete_system(
		execute_boolean,
		application_delete->
			application_backup->
			delete_directory_system_string );

	application_delete_system(
		execute_boolean,
		application_delete->
			application_data->
			delete_system_string );

	application_delete_system(
		execute_boolean,
		application_delete->
			application_login->
			delete_system_string );

	application_delete_system(
		execute_boolean,
		application_delete->
			application_upload->
			delete_system_string );

	application_delete_system( 
		execute_boolean,
		application_delete->drop_system_string );

	if ( execute_boolean )
	{
		process_execution_count_increment( process_name );
		printf( "<h3>Process complete.</h3>\n" );
	}

	if ( strcmp( process_name, "process" ) != 0 )
	{
		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
	}

	return 0;
}

