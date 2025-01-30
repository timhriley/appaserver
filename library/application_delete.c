/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_delete.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "security.h"
#include "application.h"
#include "application_create.h"
#include "application_delete.h"

APPLICATION_DELETE *application_delete_new(
		char *application_name,
		char *document_root,
		char *data_directory,
		char *upload_directory,
		char *log_directory,
		char *mount_point )
{
	APPLICATION_DELETE *application_delete;

	if ( !application_name
	||   !document_root
	||   !data_directory
	||   !upload_directory
	||   !log_directory
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_delete = application_delete_calloc();

	application_delete->application_name_invalid =
		application_name_invalid(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name );

	if ( application_delete->application_name_invalid )
		return application_delete;

	application_delete->application_log =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_log_new(
			APPLICATION_LOG_EXTENSION,
			application_name,
			log_directory );

	application_delete->application_exists_boolean =
		application_exists_boolean(
			application_delete->application_log->filename );

	if ( !application_delete->application_exists_boolean )
		return application_delete;

	application_delete->application_backup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_backup_new(
			APPLICATION_TEMPLATE_NAME,
			application_name,
			mount_point );

	application_delete->application_data =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_data_new(
			application_name,
			data_directory );

	application_delete->application_upload =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_upload_new(
			application_name,
			upload_directory );

	application_delete->application_login =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_login_new(
			APPLICATION_TEMPLATE_NAME,
			APPLICATION_CREATE_DEFAULT_TITLE,
			application_name,
			(char *)0 /* application_title */,
			document_root,
			mount_point );

	application_delete->drop_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_delete_drop_system_string(
			application_name );

	return application_delete;
}

APPLICATION_DELETE *application_delete_calloc( void )
{
	APPLICATION_DELETE *application_delete;

	if ( ! ( application_delete =
			calloc( 1,
				sizeof ( APPLICATION_DELETE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_delete;
}

char *application_delete_drop_system_string( char *application_name )
{
	static char system_string[ 128 ];

	sprintf(system_string,
		"echo \"drop database %s;\" | sql.e",
		application_name );

	return system_string;
}

void application_delete_system(
		boolean execute_boolean,
		char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( execute_boolean )
	{
		if ( system( system_string ) ){}
	}
	else
	{
		printf( "<p>%s\n", system_string );
	}
}
