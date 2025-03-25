/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/create_application.c			*/
/* --------------------------------------------------------------------	*/
/* This process creates a new application that is a clone of the	*/
/* existing application.						*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "document.h"
#include "application.h"
#include "process.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"
#include "application_clone.h"

#define NON_TEMPLATE_APPLICATION_OKAY 0

int main( int argc, char **argv )
{
	char *current_application_name;
	char *login_name;
	char *process_name;
	char *application_title;
	char *destination_application_name;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	APPLICATION_CLONE *application_clone;

	current_application_name = environ_exit_application_name( argv[ 0 ] );

	if ( !NON_TEMPLATE_APPLICATION_OKAY
	&&   strcmp(	current_application_name,
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

	appaserver_error_stderr( argc, argv );

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s login process destination_application title execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	process_name = argv[ 2 ];
	destination_application_name = argv[ 3 ];
	application_title = argv[ 4 ];
	execute = ( *argv[ 5 ] == 'y' );

	document_process_output(
		current_application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title_string */ );

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

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

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
			(char *)0 /* application_clone_sql_gz_filename */,
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
			application_log_exists_boolean )
	{
		char message[ 128 ];

		sprintf(message,
			"application_clone_new(%s) returned invalid.",
			destination_application_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
	else
	if ( execute )
	{
		environment_set(
			"DATABASE",
			destination_application_name );

		if ( !application_create_execute(
			1 /* execute_boolean */,
			application_clone->application_create ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
			"application_create_execute() returned false." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( application_clone->gz_system_string )
		{
			if ( system( application_clone->gz_system_string ) ){}
		}
		else
		if ( application_clone->database_system_string )
		{
			if ( system(
				application_clone->
					database_system_string ) ){}
		}

		if ( system(
			application_clone->
				application_system_string ) ){}

		if ( system(
			application_clone->
				insert_user_system_string ) ){}

		if ( system(
			application_clone->
				insert_role_system_string ) ){}

		process_execution_count_increment( process_name );
		printf( "<h3>Process complete.</h3>\n" );
	}
	else
	{
		printf( "<h3>Will execute:</h3>\n" );

		application_create_execute(
			0 /* not execute_boolean */,
			application_clone->application_create );

		if ( application_clone->gz_system_string )
		{
			printf(	"<p>%s\n",
				application_clone->gz_system_string );
		}
		else
		{
			printf(	"<p>%s\n",
				application_clone->database_system_string );
		}

		printf(	"<p>%s\n",
			application_clone->application_system_string );

		printf(	"<p>%s\n",
			application_clone->insert_user_system_string );

		printf(	"<p>%s\n",
			application_clone->insert_role_system_string );
	}

	document_close();

	return 0;
}
