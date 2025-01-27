/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_log.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "application_log.h"

APPLICATION_LOG *application_log_new(
		char *application_log_extension,
		char *application_name,
		char *log_directory )
{
	APPLICATION_LOG *application_log;

	if ( !application_log_extension
	||   !application_name
	||   !log_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_log = application_log_calloc();

	application_log->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_log_filename(
			application_log_extension,
			application_name,
			log_directory );

	application_log->create_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_log_create_system_string(
			application_log->filename );

	application_log->delete_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_log_delete_system_string(
			application_log->filename );

	return application_log;
}

APPLICATION_LOG *application_log_calloc( void )
{
	APPLICATION_LOG *application_log;

	if ( ! ( application_log = calloc( 1, sizeof ( APPLICATION_LOG ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_log;
}

char *application_log_filename(
		char *application_log_extension,
		char *application_name,
		char *log_directory )
{
	static char filename[ 128 ];

	sprintf(filename,
		"%s/appaserver_%s.%s",
		log_directory,
		application_name,
		application_log_extension );

	return filename;
}

char *application_log_create_system_string(
		char *application_log_filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"touch %s && "
		"chmod g+w %s && "
		"chmod o-rwx %s",
		application_log_filename,
		application_log_filename,
		application_log_filename );

	return strdup( system_string );
}

char *application_log_delete_system_string(
		char *application_log_filename )
{
	char system_string[ 256 ];

	sprintf(system_string,
		"rm %s",
		application_log_filename );

	return strdup( system_string );
}
