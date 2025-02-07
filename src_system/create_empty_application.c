/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/create_empty_application.c		*/
/* --------------------------------------------------------------------	*/
/* If process_name is "process" then outputs the generated password.	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "application.h"
#include "session.h"
#include "process.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "document.h"
#include "security.h"
#include "application_clone.h"

int main( int argc, char **argv )
{
	char *current_application_name;
	char *session_key;
	char *login_name;
	char *process_name;
	char *destination_application_name;
	char *application_title;
	boolean execute_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	APPLICATION_CLONE *application_clone;

	current_application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		current_application_name );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s session login process destination_application application_title execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	process_name = argv[ 3 ];
	destination_application_name = argv[ 4 ];
	application_title = argv[ 5 ];
	execute_boolean = (*argv[ 6 ] == 'y');

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

	session_access_or_exit(
		current_application_name,
		session_key,
		login_name );

	if ( *process_name && strcmp( process_name, "process" ) != 0 )
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

	if ( !*destination_application_name
	||   strcmp(	destination_application_name,
			"destination_application" ) == 0 )
	{
		printf(
		"<h3>Please enter a destination application name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	application_clone =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_clone_new(
			current_application_name,
			login_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				destination_application_name ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			application_clone_sql_gz_filename(
				appaserver_parameter->mount_point ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				application_title ),
			appaserver_parameter->document_root,
			appaserver_parameter->data_directory,
			appaserver_parameter->upload_directory,
			appaserver_parameter->log_directory,
			appaserver_parameter->mount_point );

	if (	application_clone->
			application_create->
			application_name_invalid
	||	application_clone->
			application_create->
			application_exists_boolean )
	{
		if ( application_clone->
			application_create->
			application_name_invalid )
		{
			printf(
			"<h3>ERROR: Invalid application name.</h3>\n" );
		}
		else
		{
			printf(
		"<h3>ERROR: The application name already exists.</h3>\n" );
		}

		if ( *process_name && strcmp( process_name, "process" ) != 0 )
		{
			/* ------------------------ */
			/* Both </body> and </html> */
			/* ------------------------ */
			document_close();
		}

		exit( 1 );
	}

	if ( !application_create_execute(
		execute_boolean,
		application_clone->application_create ) )
	{
		printf(
		"<h3>ERROR: The database name already exists.</h3>\n" );

		if ( *process_name && strcmp( process_name, "process" ) != 0 )
		{
			/* ------------------------ */
			/* Both </body> and </html> */
			/* ------------------------ */
			document_close();
		}

		exit( 1 );
	}

	if ( application_clone->gz_system_string )
	{
		application_clone_system(
			execute_boolean,
			application_clone->gz_system_string );
	}
	else
	{
		application_clone_system(
			execute_boolean,
			application_clone->database_system_string );
	}

	application_clone_system(
		execute_boolean,
		application_clone->application_system_string );

	application_clone_system(
		execute_boolean,
		application_clone->insert_user_system_string );

	application_clone_system(
		execute_boolean,
		application_clone->insert_role_system_string );

	if ( execute_boolean )
	{
		if ( *process_name && strcmp( process_name, "process" ) != 0 )
		{
			printf( "<h2>Process Complete</h2>\n" );

			printf( "<h3>Login with %s/yourPassword</h3>\n",
				login_name );
		}
		else
		{
			printf( "%s\n",
				application_clone->
					appaserver_user->
					database_password );
		}
	}

	if ( *process_name && strcmp( process_name, "process" ) != 0 )
	{
		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
	}

	if ( execute_boolean
	&&   *process_name && strcmp( process_name, "process" ) != 0 )
	{
		process_execution_count_increment( process_name );
	}

	return 0;
}

