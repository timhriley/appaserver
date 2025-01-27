/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/drop_application.c 			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "application_create.h"
#include "application_delete.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *drop_application_name;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	APPLICATION_DELETE *application_delete;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( strcmp( application_name, APPLICATION_CREATE_TEMPLATE_NAME ) != 0 )
	{
		char message[ 128 ];

		sprintf(message,
			"application_name must be %s.",
			APPLICATION_CREATE_TEMPLATE_NAME );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process drop_application execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	drop_application_name = argv[ 2 ];
	execute = ( *argv[ 3 ] == 'y' );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title_string */ );

	if ( !*drop_application_name
	||   strcmp( drop_application_name, "drop_application" ) == 0 )
	{
		printf( "<h3>Please enter in an existing application.</h3>\n" );

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

	application_delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_delete_new(
			drop_application_name,
			appaserver_parameter->document_root,
			appaserver_parameter->data_directory,
			appaserver_parameter->upload_directory,
			appaserver_parameter->log_directory,
			appaserver_parameter->mount_point );

	if (	application_delete->application_name_invalid
	||	!application_delete->application_exists_boolean )
	{
		char message[ 128 ];

		sprintf(message,
			"application_delete_new(%s) returned invalid.",
			drop_application_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
	else
	if ( execute )
	{
		if ( system(
			application_delete->
				application_log->
				delete_system_string ) ){}

		if ( system(
			application_delete->
				application_backup->
				delete_file_system_string ) ){}

		if ( system(
			application_delete->
				application_backup->
				delete_directory_system_string ) ){}

		if ( system(
			application_delete->
				application_data->
				delete_system_string ) ){}

		if ( system(
			application_delete->
				drop_system_string ) ){}

		printf( "<h3>Process complete.</h3>\n" );
	}
	else
	{
		printf( "<h3>Will execute:</h3>\n" );

		printf( "<p>%s\n",
			application_delete->
				application_log->
				delete_system_string );

		printf( "<p>%s\n",
			application_delete->
				application_backup->
				delete_file_system_string );

		printf( "<p>%s\n",
			application_delete->
				application_backup->
				delete_directory_system_string );

		printf( "<p>%s\n",
			application_delete->
				application_data->
				delete_system_string );

		printf( "<p>%s\n",
			application_delete->
				application_log->
				delete_system_string );

		printf( "<p>%s\n",
			application_delete->drop_system_string );

	}

	document_close();

	return 0;
}
