/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_upload.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "application_upload.h"

APPLICATION_UPLOAD *application_upload_new(
		char *application_name,
		char *download_directory )
{
	APPLICATION_UPLOAD *application_upload;

	if ( !application_name
	||   !download_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_upload = application_upload_calloc();

	application_upload->directory =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_upload_directory(
			application_name,
			download_directory );

	application_upload->create_directory_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_upload_create_directory_system_string(
			application_upload->directory );

	application_upload->delete_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_upload_delete_system_string(
			application_upload->directory );

	return application_upload;
}

APPLICATION_UPLOAD *application_upload_calloc( void )
{
	APPLICATION_UPLOAD *application_upload;

	if ( ! ( application_upload =
			calloc( 1,
				sizeof ( APPLICATION_UPLOAD ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_upload;
}

char *application_upload_directory(
		char *application_name,
		char *download_directory )
{
	static char directory[ 128 ];

	snprintf(
		directory,
		sizeof ( directory ),
		"%s/%s",
		download_directory,
		application_name );

	return directory;
}

char *application_upload_create_directory_system_string(
		char *application_upload_directory )
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
		"chmod g+ws %s && "
		"chmod o-rwx %s",
		application_upload_directory,
		application_upload_directory,
		application_upload_directory );

	return strdup( system_string );
}

char *application_upload_delete_system_string(
		char *application_upload_directory )
{
	char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm -r %s",
		application_upload_directory );

	return strdup( system_string );
}
