/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_create.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "security.h"
#include "appaserver_error.h"
#include "spool.h"
#include "application.h"
#include "application_create.h"

APPLICATION_CREATE *application_create_new(
		char *application_name,
		char *application_title,
		char *document_root,
		char *data_directory,
		char *upload_directory,
		char *log_directory,
		char *mount_point )
{
	APPLICATION_CREATE *application_create;

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

	application_create = application_create_calloc();

	application_create->application_name_invalid =
		application_name_invalid(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name );

	if ( application_create->application_name_invalid )
		return application_create;

	application_create->database_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_create_database_system_string(
			application_name );

	application_create->application_log =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_log_new(
			APPLICATION_LOG_EXTENSION,
			application_name,
			log_directory );

	application_create->application_log_exists_boolean =
		application_log_exists_boolean(
			application_name,
			application_create->application_log->filename );

	if ( application_create->application_log_exists_boolean )
		return application_create;

	application_create->application_backup =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_backup_new(
			APPLICATION_TEMPLATE_NAME,
			application_name,
			mount_point );

	application_create->application_data =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_data_new(
			application_name,
			data_directory );

	application_create->application_upload =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_upload_new(
			application_name,
			upload_directory );

	application_create->application_login =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		application_login_new(
			APPLICATION_TEMPLATE_NAME,
			APPLICATION_CREATE_DEFAULT_TITLE,
			application_name,
			application_title,
			document_root,
			mount_point );

	return application_create;
}

APPLICATION_CREATE *application_create_calloc( void )
{
	APPLICATION_CREATE *application_create;

	if ( ! ( application_create =
			calloc( 1, sizeof ( APPLICATION_CREATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_create;
}

boolean application_create_execute(
		boolean execute_boolean,
		APPLICATION_CREATE *application_create )
{
	if ( !application_create
	||   !application_create->application_log
	||   !application_create->application_backup
	||   !application_create->application_data
	||   !application_create->application_upload
	||   !application_create->application_login )
	{
		char message[ 128 ];

		sprintf(message, "application_create is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	application_create->application_name_invalid
	||	application_create->application_log_exists_boolean )
	{
		char message[ 128 ];

		sprintf(message, "application_create is invalid." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !application_create_system(
		execute_boolean,
		application_create->database_system_string ) )
	{
		return 0;
	}

	application_create_system(
		execute_boolean,
		application_create->
			application_log->
			create_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_backup->
			create_directory_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_backup->
			create_file_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_data->
			create_directory_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_data->
			texlive_2019_link_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_data->
			texlive_2021_link_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_upload->
			create_directory_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_login->
			directory_system_string );

	application_create_system(
		execute_boolean,
		application_create->
			application_login->
			php_file_system_string );

	return 1;
}

boolean application_create_system(
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
		if ( spool_fetch(
			system_string,
			1 /* capture_stderr_boolean */ ) )
		{
			return 0;
		}
	}
	else
	{
		printf( "<p>%s\n", system_string );
	}

	return 1;
}

char *application_create_database_system_string( char *application_name )
{
	char system_string[ 128 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"create database %s;\" | sql.e mysql",
		application_name );

	return strdup( system_string );
}

