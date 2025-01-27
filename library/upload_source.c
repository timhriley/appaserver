/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/upload_source.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "String.h"
#include "basename.h"
#include "session.h"
#include "upload_source.h"

UPLOAD_SOURCE *upload_source_new(
			char *application_name,
			char *directory_filename_session,
			char *mount_point )
{
	UPLOAD_SOURCE *upload_source;

	if ( !application_name
	||   !directory_filename_session
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

	upload_source = upload_source_calloc();

	upload_source->file_exists_boolean =
		upload_source_file_exists_boolean(
			directory_filename_session );

	if ( !upload_source->file_exists_boolean )
		return upload_source;

	upload_source->directory_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_directory_filename(
			directory_filename_session );

	upload_source->filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_filename(
			upload_source->directory_filename );

	upload_source->destination_directory =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_destination_directory(
			application_name,
			mount_point );

	upload_source->destination_directory_exists_boolean =
		upload_source_file_exists_boolean(
			upload_source->destination_directory
				/* directory_filename_session */ );

	if ( !upload_source->destination_directory_exists_boolean )
		return upload_source;

	upload_source->destination_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_destination_filename(
			upload_source->filename,
			upload_source->destination_directory );

	upload_source->copy_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_copy_system_string(
			directory_filename_session,
			upload_source->destination_filename );

	upload_source->chmod_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_chmod_system_string(
			upload_source->destination_filename );

	upload_source->confirm_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_source_confirm_system_string(
			upload_source->filename,
			upload_source->destination_directory );

	return upload_source;
}

UPLOAD_SOURCE *upload_source_calloc( void )
{
	UPLOAD_SOURCE *upload_source;

	if ( ! ( upload_source = calloc( 1, sizeof ( UPLOAD_SOURCE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return upload_source;
}

boolean upload_source_file_exists_boolean( char *directory_filename_session )
{
	char system_string[ 1024 ];

	if ( !directory_filename_session ) return 0;

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo %s | stat.e 2>&1 | grep 'No such file' | wc -l",
		directory_filename_session );

	return 1 - atoi( string_pipe_fetch( system_string ) );
}

char *upload_source_directory_filename( char *directory_filename_session )
{
	static char directory_filename[ 128 ];

	if ( !directory_filename_session )
	{
		char message[ 128 ];

		sprintf(message, "directory_filename_session is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy(
		directory_filename,
		directory_filename_session );

	return
	/* ------------------------ */
	/* Returns filename_session */
	/* ------------------------ */
	session_right_trim(
		directory_filename
			/* filename_session */ );
}

char *upload_source_filename( char *upload_source_directory_filename )
{
	if ( !upload_source_directory_filename )
	{
		char message[ 128 ];

		sprintf(message, "upload_source_directory_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	basename_filename(
		upload_source_directory_filename /* argv_0 */ );
}

char *upload_source_destination_directory(
			char *application_name,
			char *mount_point )
{
	static char directory[ 64 ];

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

	sprintf(directory,
		"%s/src_%s",
		mount_point,
		application_name );

	return directory;
}

char *upload_source_destination_filename(
			char *upload_source_filename,
			char *upload_source_destination_directory )
{
	static char filename[ 256 ];

	if ( !upload_source_filename
	||   !upload_source_destination_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(filename,
		"%s/%s",
		upload_source_destination_directory,
		upload_source_filename );

	return filename;
}

char *upload_source_copy_system_string(
			char *directory_filename_session,
			char *upload_source_destination_filename )
{
	static char system_string[ 512 ];

	if ( !directory_filename_session
	||   !upload_source_destination_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"cp %s %s",
		directory_filename_session,
		upload_source_destination_filename );

	return system_string;
}

char *upload_source_chmod_system_string(
			char *upload_source_destination_filename )
{
	static char system_string[ 512 ];

	if ( !upload_source_destination_filename )
	{
		char message[ 128 ];

		sprintf(message,
			"upload_source_destination_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"chmod g+w %s && chmod -x %s",
		upload_source_destination_filename,
		upload_source_destination_filename );

	return system_string;
}

char *upload_source_confirm_system_string(
			char *upload_source_filename,
			char *upload_source_destination_directory )
{
	static char system_string[ 512 ];

	if ( !upload_source_filename
	||   !upload_source_destination_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"(cd %s && ls -l %s) 2>&1 | "
		"html_paragraph_wrapper.e",
		upload_source_destination_directory,
		upload_source_filename );

	return system_string;
}
