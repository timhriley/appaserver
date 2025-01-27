/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_backup.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application_backup.h"

APPLICATION_BACKUP *application_backup_new(
		const char *application_create_template_name,
		char *application_name,
		char *mount_point )
{
	APPLICATION_BACKUP *application_backup;

	if ( !application_name
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

	application_backup = application_backup_calloc();

	application_backup->appaserver_parameter_backup_directory =
		/* ------------------------------------------------ */
		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		appaserver_parameter_backup_directory();

	application_backup->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_backup_filename(
			application_name,
			application_backup->
				appaserver_parameter_backup_directory );

	application_backup->create_file_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_backup_create_file_system_string(
			application_create_template_name,
			application_name,
			mount_point,
			application_backup->filename );

	application_backup->delete_file_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_backup_delete_file_system_string(
			application_backup->filename );

	application_backup->create_directory_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_backup_create_directory_system_string(
			APPLICATION_BACKUP_MKDIR_SCRIPT,
			application_name,
			application_backup->
				appaserver_parameter_backup_directory );

	application_backup->directory_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_backup_directory_name(
			application_name,
			application_backup->
				appaserver_parameter_backup_directory );

	application_backup->delete_directory_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_backup_delete_directory_system_string(
			application_backup->directory_name );

	return application_backup;
}

APPLICATION_BACKUP *application_backup_calloc( void )
{
	APPLICATION_BACKUP *application_backup;

	if ( ! ( application_backup =
			calloc( 1,
				sizeof ( APPLICATION_BACKUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_backup;
}

char *application_backup_filename(
		char *application_name,
		char *backup_directory )
{
	static char filename[ 128 ];

	if ( !application_name
	||   !backup_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filename,
		sizeof ( filename ),
		"%s/mysqldump_%s.config",
		backup_directory,
		application_name );

	return filename;
}

char *application_backup_create_file_system_string(
		const char *application_create_template_name,
		char *application_name,
		char *mount_point,
		char *application_backup_filename )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"sed 's/change_me/%s/' "
		"< %s/%s/mysqldump_%s.config "
		"> %s && "
		"chmod o-rw %s && "
		"chmod g+w %s",
		application_name,
		mount_point,
		application_create_template_name,
		application_create_template_name,
		application_backup_filename,
		application_backup_filename,
		application_backup_filename );

	return strdup( system_string );
}

char *application_backup_directory_name(
		char *application_name,
		char *appaserver_parameter_backup_directory )
{
	static char directory_name[ 128 ];

	if ( !application_name
	||   !appaserver_parameter_backup_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		directory_name,
		sizeof ( directory_name ),
		"%s/%s",
		appaserver_parameter_backup_directory,
		application_name );

	return directory_name;
}

char *application_backup_delete_file_system_string(
		char *application_backup_filename )
{
	char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		application_backup_filename );

	return strdup( system_string );
}

char *application_backup_delete_directory_system_string(
		char *application_backup_directory_name )
{
	char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm -fr %s",
		application_backup_directory_name );

	return strdup( system_string );
}

char *application_backup_create_directory_system_string(
		const char *application_backup_mkdir_script,
		char *application_name,
		char *backup_directory )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !backup_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s",
		application_backup_mkdir_script,
		application_name,
		backup_directory );

	return strdup( system_string );
}

