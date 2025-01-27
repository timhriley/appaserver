/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/application_data.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application_data.h"

APPLICATION_DATA *application_data_new(
		char *application_name,
		char *data_directory )
{
	APPLICATION_DATA *application_data;
	char *mount_point;

	if ( !application_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	application_data = application_data_calloc();

	application_data->directory =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_data_directory(
			application_name,
			data_directory );

	application_data->create_directory_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_data_create_directory_system_string(
			application_data->directory );

	mount_point =
		/* Returns component of global_appaserver_parameter */
		/* ------------------------------------------------ */
		appaserver_parameter_mount_point();

	application_data->texlive_2019_link_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_data_textlive_link_system_string(
			".texlive2019",
			application_data->directory,
			mount_point );

	application_data->texlive_2021_link_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_data_textlive_link_system_string(
			".texlive2021",
			application_data->directory,
			mount_point );

	application_data->delete_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		application_data_delete_system_string(
			application_data->directory );

	return application_data;
}

APPLICATION_DATA *application_data_calloc( void )
{
	APPLICATION_DATA *application_data;

	if ( ! ( application_data =
			calloc( 1,
				sizeof ( APPLICATION_DATA ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return application_data;
}

char *application_data_directory(
		char *application_name,
		char *data_directory )
{
	static char directory[ 128 ];

	snprintf(
		directory,
		sizeof ( directory ),
		"%s/%s",
		data_directory,
		application_name );

	return directory;
}

char *application_data_create_directory_system_string(
		char *application_data_directory )
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
		application_data_directory,
		application_data_directory,
		application_data_directory );

	return strdup( system_string );
}

char *application_data_delete_system_string(
		char *application_data_directory )
{
	char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm -r %s",
		application_data_directory );

	return strdup( system_string );
}

char *application_data_textlive_link_system_string(
		const char *texlive_directory,
		char *application_data_directory,
		char *appaserver_parameter_mount_point )
{
	char system_string[ 1024 ];

	if ( !application_data_directory
	||   !appaserver_parameter_mount_point )
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
		"ln -s %s/zfonts %s/%s",
		appaserver_parameter_mount_point,
		application_data_directory,
		texlive_directory );

	return strdup( system_string );
}

