/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_menu.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "list.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "role_folder.h"
#include "folder_menu.h"

FOLDER_MENU *folder_menu_calloc( void )
{
	FOLDER_MENU *folder_menu;

	if ( ! ( folder_menu = 
			calloc( 1, sizeof ( FOLDER_MENU ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return folder_menu;
}

FOLDER_MENU *folder_menu_fetch(
		char *application_name,
		char *session_key,
		char *role_name,
		char *data_directory,
		boolean folder_menu_remove_boolean )
{
	FOLDER_MENU *folder_menu;

	if ( !application_name
	||   !session_key
	||   !role_name
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

	folder_menu = folder_menu_calloc();

	folder_menu->lookup_folder_name_list =
		folder_menu_lookup_folder_name_list(
			role_name );

	folder_menu->insert_folder_name_list =
		folder_menu_insert_folder_name_list(
			role_name );

	if ( ! ( folder_menu->role =
			role_fetch(
				role_name,
				0 /* not fetch_role_..._exclude_list */ ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		exit( 1 );
	}

	if ( folder_menu->role->folder_count )
	{
		folder_menu->filename =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_menu_filename(
				FOLDER_MENU_EXTENSION,
				application_name,
				session_key,
				role_name,
				data_directory );

		if ( folder_menu_remove_boolean )
		{
			folder_menu_remove_file( folder_menu->filename );
		}

		folder_menu->count_list =
			folder_menu_count_list(
				application_name,
				folder_menu->lookup_folder_name_list,
				folder_menu->filename );
	}

	return folder_menu;
}

LIST *folder_menu_lookup_folder_name_list(
		char *role_name )
{
	return role_folder_name_list(
		role_folder_lookup_list(
			role_name ) );
}

LIST *folder_menu_insert_folder_name_list(
		char *role_name )
{
	return role_folder_name_list(
		role_folder_insert_list(
			role_name ) );
}

LIST *folder_menu_count_list(
		char *application_name,
		LIST *folder_menu_lookup_folder_name_list,
		char *folder_menu_filename )
{
	LIST *list;
	char *system_string;
	FILE *input_pipe;
	char input[ 128 ];

	if ( !application_name )
	{
		char message[ 128 ];

		sprintf(message, "application_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !folder_menu_filename )
	{
		char message[ 128 ];

		sprintf(message, "folder_menu_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( folder_menu_lookup_folder_name_list ) ) return NULL;

	if ( !timlib_file_exists( folder_menu_filename ) )
	{
		list_write(
			folder_menu_lookup_folder_name_list,
			folder_menu_filename );
	}

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_menu_count_system_string(
			application_name,
			folder_menu_filename );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe( system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 128 ) )
	{
		list_set(
			list,
			folder_menu_count_parse(
				input ) );
	}

	pclose( input_pipe );
	return list;
}

FOLDER_MENU_COUNT *folder_menu_count_calloc( void )
{
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( ! ( folder_menu_count = 
			calloc( 1, sizeof ( FOLDER_MENU_COUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return folder_menu_count;
}

unsigned long folder_menu_count_fetch(
		char *folder_table_name )
{
	char system_string[ 1024 ];
	char *results;

	sprintf(system_string,
		"echo 'select count(1) from %s;' | sql_timeout.sh 1",
		folder_table_name );

	results = string_pipe_fetch( system_string );

	if ( !results || !*results )
		return 0;
	else
		return strtoul( results, (char **)0, 0 );
}

FOLDER_MENU_COUNT *folder_menu_count_parse( char *input )
{
	FOLDER_MENU_COUNT *folder_menu_count;
	char folder_name[ 128 ];
	char count_string[ 128 ];

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( folder_name, '=', input, 0 );
	piece( count_string, '=', input, 1 );

	folder_menu_count =
		folder_menu_count_new(
			strdup( folder_name ),
			count_string );

	return folder_menu_count;
}

FOLDER_MENU_COUNT *folder_menu_count_new(
		char *folder_name,
		char *count_string )
{
	FOLDER_MENU_COUNT *folder_menu_count =
		folder_menu_count_calloc();

	folder_menu_count->folder_name = folder_name;
	folder_menu_count->count = strtoul( count_string, (char **)0, 0 );

	folder_menu_count->display =
		folder_menu_count_display(
			folder_menu_count->count );

	return folder_menu_count;
}

char *folder_menu_filename(
		const char *folder_menu_extension,
		char *application_name,
		char *session_key,
		char *role_name,
		char *data_directory )
{
	static char filename[ 256 ];

	if ( !application_name
	||   !session_key
	||   !role_name
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

	sprintf( filename,
		 "%s/%s/%s_%s.%s",
		 data_directory,
		 application_name,
		 role_name,
		 session_key,
		 folder_menu_extension );

	return filename;
}

FOLDER_MENU_COUNT *folder_menu_count_seek(
		char *folder_name,
		LIST *folder_menu_count_list )
{
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( !list_rewind( folder_menu_count_list ) ) return NULL;

	do {
		folder_menu_count =
			list_get(
				folder_menu_count_list );

		if ( strcmp(
			folder_menu_count->folder_name,
			folder_name ) == 0 )
		{
			return folder_menu_count;
		}

	} while ( list_next( folder_menu_count_list ) );

	return NULL;
}

char *folder_menu_count_display( unsigned long count )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	place_commas_in_unsigned_long( count );
}

void folder_menu_remove_file( char *folder_menu_filename )
{
	timlib_remove_file( folder_menu_filename );
}

char *folder_menu_count_system_string(
		char *application_name,
		char *folder_menu_filename )
{
	static char system_string[ 512 ];

	if ( !application_name
	||   !folder_menu_filename )
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
		"cat %s | table_count.e %s",
		/* ------------------------ */
		/* Prevent compiler warning */
		/* ------------------------ */
		(folder_menu_filename) ? folder_menu_filename : "",
		(application_name) ? application_name : "" );

	return system_string;
}

