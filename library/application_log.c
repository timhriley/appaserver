/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_log.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver_error.h"
#include "application_log.h"

APPLICATION_LOG *application_log_new(
		const char *application_log_extension,
		char *application_name,
		char *log_directory )
{
	APPLICATION_LOG *application_log;

	if ( !application_name
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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
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
		const char *application_log_extension,
		char *application_name,
		char *log_directory )
{
	char filename[ 128 ];

	if ( !application_name
	||   !log_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		filename,
		sizeof ( filename ),
		"%s/appaserver_%s.%s",
		log_directory,
		application_name,
		application_log_extension );

	return strdup( filename );
}

char *application_log_create_system_string(
		char *application_log_filename )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
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

	if ( !application_log_filename )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: application_log_filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		application_log_filename );

	return strdup( system_string );
}

boolean application_log_exists_boolean(
		char *application_name,
		char *application_log_filename )
{
	if ( !application_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: application_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( application_name, "hydrology" ) == 0
	||   strcmp( application_name, "benthic" ) == 0
	||   strcmp( application_name, "donner" ) == 0
	||   strcmp( application_name, "tnt" ) == 0
	||   strcmp( application_name, "creel" ) == 0
	||   strcmp( application_name, "waterquality" ) == 0
	||   strcmp( application_name, "wadingbird" ) == 0
	||   strcmp( application_name, "audubon" ) == 0
	||   strcmp( application_name, "cesi" ) == 0
	||   strcmp( application_name, "sparrow" ) == 0
	||   strcmp( application_name, "panther" ) == 0
	||   strcmp( application_name, "modeland" ) == 0
	||   strcmp( application_name, "vegetation" ) == 0
	||   strcmp( application_name, "freshwaterfish" ) == 0 )
	{
		return 1;
	}

	return
	application_exists_boolean(
		application_log_filename );
}

