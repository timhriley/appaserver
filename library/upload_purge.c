/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/upload_purge.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "application.h"
#include "upload_purge.h"

UPLOAD_PURGE *upload_purge_new(
		char *application_name,
		char *key,
		char *table_name,
		char *column_name,
		char *appaserver_parameter_upload_directory )
{
	UPLOAD_PURGE *upload_purge;

	if ( !application_name
	||   !table_name
	||   !column_name
	||   !appaserver_parameter_upload_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	upload_purge = upload_purge_calloc();

	upload_purge->directory =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_purge_directory(
			application_name,
			key,
			appaserver_parameter_upload_directory );

	upload_purge->application_not_null_data_list =
		application_not_null_data_list(
			table_name,
			column_name );

	upload_purge->filename_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		upload_purge_filename_system_string(
			UPLOAD_PURGE_OLDER_MINUTES,
			upload_purge->directory );

	upload_purge->filename_list =
		upload_purge_filename_list(
			upload_purge->filename_system_string );

	upload_purge->system_list =
		upload_purge_system_list(
			upload_purge->application_not_null_data_list,
			upload_purge->filename_list );

	return upload_purge;
}

UPLOAD_PURGE *upload_purge_calloc( void )
{
	UPLOAD_PURGE *upload_purge;

	if ( ! ( upload_purge = calloc( 1, sizeof ( UPLOAD_PURGE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return upload_purge;
}

char *upload_purge_directory(
		char *application_name,
		char *key,
		char *upload_directory )
{
	static char directory[ 128 ];

	if ( !application_name
	||   !upload_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( key && *key )
	{
		snprintf(
			directory,
			sizeof ( directory ),
			"%s/%s/%s",
			upload_directory,
			application_name,
			key );
	}
	else
	{
		snprintf(
			directory,
			sizeof ( directory ),
			"%s/%s",
			upload_directory,
			application_name );
	}

	return directory;
}

char *upload_purge_filename_system_string(
		const int upload_purge_older_minutes,
		char *directory )
{
	static char system_string[ 128 ];

	if ( !directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: directory is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"cd %s && "
		"find . -type f -mmin +%d -print |"
		"sed 's,^./,,'",
		directory,
		upload_purge_older_minutes );

	return system_string;
}

LIST *upload_purge_system_list(
		LIST *application_not_null_data_list,
		LIST *upload_purge_filename_list )
{
	LIST *system_list = list_new();
	char *filename;

	if ( list_rewind( upload_purge_filename_list ) )
	do {
		filename = list_get( upload_purge_filename_list );

		if ( !list_string_exists(
			filename,
			application_not_null_data_list ) )
		{
			list_set(
				system_list,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				upload_purge_system_string(
					filename ) );
		}

	} while ( list_next( upload_purge_filename_list ) );

	if ( !list_length( system_list ) )
	{
		list_free( system_list );
		system_list = NULL;
	}

	return system_list;
}

void upload_purge_execute(
		char *directory,
		LIST *system_list,
		boolean display_only_boolean )
{
	char *system_string;

	if ( !directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: directory is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( chdir( directory ) != 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: chdir(%s) failed.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			directory );
		exit( 1 );
	}

	if ( list_rewind( system_list ) )
	do {
		system_string = list_get( system_list );

		if ( display_only_boolean )
			printf( "%s\n", system_string );
		else
			if ( system( system_string ) ){}

	} while ( list_next( system_list ) );
}

char *upload_purge_system_string( char *filename )
{
	char system_string[ 128 ];

	if ( !filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: filename is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		filename );

	return strdup( system_string );
}

LIST *upload_purge_filename_list( char *filename_system_string )
{
	if ( !filename_system_string )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: filename_system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	list_pipe_fetch( filename_system_string );
}

