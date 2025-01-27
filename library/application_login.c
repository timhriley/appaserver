/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_login.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "application_login.h"

APPLICATION_LOGIN *application_login_new(
		const char *application_create_template_name,
		const char *application_create_default_title,
		char *application_name,
		char *application_title,
		char *document_root,
		char *mount_point )
{
	APPLICATION_LOGIN *application_login;
	char *destination_filename;
	char *source_filename;

	if ( !application_name
	||   !document_root
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

	application_login = application_login_calloc();

	application_login->title =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		application_login_title(
			application_create_default_title,
			application_title );

	application_login->destination_directory =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_login_destination_directory(
			application_name,
			document_root );

	destination_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_login_destination_filename(
			"php" /* extension */,
			application_login->destination_directory );

	source_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_login_source_filename(
			application_create_template_name,
			"php" /* extension */,
			mount_point );

	application_login->directory_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_login_directory_system_string(
			application_login->destination_directory );

	application_login->php_file_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_login_file_system_string(
			application_name,
			application_login->title,
			source_filename,
			destination_filename );

	application_login->delete_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_login_delete_system_string(
			application_login->destination_directory );

	return application_login;
}

APPLICATION_LOGIN *application_login_calloc( void )
{
	APPLICATION_LOGIN *application_login;

	if ( ! ( application_login =
			calloc( 1,
				sizeof ( APPLICATION_LOGIN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_login;
}

char *application_login_title(
		const char *application_create_default_title,
		char *application_title )
{
	if ( !application_title
	||   !*application_title
	||   strcmp( application_title, "application_title" ) == 0 )
		return (char *)application_create_default_title;
	else
		return application_title;
}

char *application_login_source_filename(
		const char *application_create_template_name,
		const char *extension,
		char *mount_point )
{
	static char filename[ 128 ];

	sprintf(filename,
		"%s/%s/index.%s",
		mount_point,
		application_create_template_name,
		extension );

	return filename;
}

char *application_login_destination_directory(
		char *application_name,
		char *document_root )
{
	static char directory[ 128 ];

	if ( !application_name
	||   !document_root )
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
		directory,
		sizeof ( directory ),
		"%s/appaserver/%s",
		document_root,
		application_name );

	return directory;
}

char *application_login_destination_filename(
		const char *extension,
		char *destination_directory )
{
	static char filename[ 256 ];

	if ( !destination_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"destination_directory is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		filename,
		sizeof ( filename ),
		"%s/index.%s",
		destination_directory,
		extension );

	return filename;
}

char *application_login_file_system_string(
		char *application_name,
		char *application_login_title,
		char *application_login_source_filename,
		char *application_login_destination_filename )
{
	char system_string[ 1024 ];

	if ( !application_name
	||   !application_login_title
	||   !application_login_source_filename
	||   !application_login_destination_filename )
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
		"cat %s | "
		"sed \"s/title_change_me/%s/\" | "
		"sed \"s/application_change_me/%s/\" "
		"> %s && "
		"chmod g+w %s",
		application_login_source_filename,
		application_login_title,
		application_name,
		application_login_destination_filename,
		application_login_destination_filename );

	return strdup( system_string );
}

char *application_login_delete_system_string(
		char *application_login_destination_directory )
{
	char system_string[ 256 ];

	sprintf(system_string,
		"rm -r %s",
		application_login_destination_directory );

	return strdup( system_string );
}

char *application_login_directory_system_string(
		char *application_login_destination_directory )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"mkdir %s && "
		/* ----------------------------------------------------- */
		/* Running as user=www-data is not setting the SGID bit. */
		/* Also, chmod( this, 02770 ) doesn't work.		 */
		/* The workaround is to join group=www-data.		 */
		/* ----------------------------------------------------- */
		"chmod g+ws %s",
		application_login_destination_directory,
		application_login_destination_directory );

	return strdup( system_string );
}
