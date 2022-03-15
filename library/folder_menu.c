/* $APPASERVER_HOME/library/folder_menu.c	 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* --------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "list.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "folder.h"
#include "role_folder.h"
#include "folder_menu.h"

FOLDER_MENU *folder_menu_calloc( void )
{
	FOLDER_MENU *folder_menu;

	if ( ! ( folder_menu = 
			calloc( 1, sizeof( FOLDER_MENU ) ) ) )
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

FOLDER_MENU *folder_menu_new(
			char *application_name,
			char *session_key,
			char *role_name,
			char *data_directory )
{
	FOLDER_MENU *folder_menu;

	if ( !application_name
	||   !session_key
	||   !role_name
	||   !data_directory )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	folder_menu = folder_menu_calloc();

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

	folder_menu->lookup_folder_name_list =
		folder_menu_lookup_folder_name_list(
			role_name );

	folder_menu->insert_folder_name_list =
		folder_menu_insert_folder_name_list(
			role_name );

	if ( folder_menu->role->folder_count )
	{
		if ( ! ( folder_menu->filename =
			   /* --------------------------- */
			   /* Returns heap memory or null */
			   /* --------------------------- */
			   folder_menu_filename(
				application_name,
				session_key,
				role_name,
				data_directory ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_filename() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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
			LIST *lookup_folder_name_list,
			char *filename )
{
	LIST *count_list;
	char *folder_name;

	if ( file_exists( filename ) )
	{
		return folder_menu_count_read_list( filename );
	}

	if ( !list_rewind( lookup_folder_name_list ) ) return (LIST *)0;

	count_list = list_new();

	do {
		folder_name = list_get( lookup_folder_name_list );

		list_set(
			count_list,
			folder_menu_count_new(
				application_name,
				folder_name ) );

	} while ( list_next( lookup_folder_name_list ) );

	folder_menu_count_list_write( filename, count_list );

	return count_list;
}

FOLDER_MENU_COUNT *folder_menu_count_calloc( void )
{
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( ! ( folder_menu_count = 
			calloc( 1, sizeof( FOLDER_MENU_COUNT ) ) ) )
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

FOLDER_MENU_COUNT *folder_menu_count_new(
			char *application_name,
			char *folder_name )
{
	FOLDER_MENU_COUNT *folder_menu_count = folder_menu_count_calloc();

	folder_menu_count->folder_name = folder_name;

	folder_menu_count->count =
		folder_menu_count_fetch(
			folder_table_name(
				application_name,
				folder_name ) );

	folder_menu_count->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		folder_menu_count_display(
			folder_menu_count->count );

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

LIST *folder_menu_count_read_list( char *filename )
{
	FILE *input_file;
	char input[ 256 ];
	char folder_name[ 128 ];
	char count[ 128 ];
	LIST *count_list;
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filename );
		exit( 1 );
	}

	count_list = list_new();

	while ( string_input( input, input_file, 256 ) )
	{
		folder_menu_count = folder_menu_count_calloc();

		piece( folder_name, SQL_DELIMITER, input, 0 );
		folder_menu_count->folder_name = strdup( folder_name );

		piece( count, SQL_DELIMITER, input, 1 );
		folder_menu_count->count = strtoul( count, (char **)0, 0 );

		list_set( count_list, folder_menu_count );
	}

	fclose( input_file );
	return count_list;
}

void folder_menu_count_list_write(
			char *filename,
			LIST *count_list )
{
	FILE *output_file;
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( !list_rewind( count_list ) ) return;

	if ( ! ( output_file = fopen( filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fopen(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			filename );
		exit( 1 );
	}

	do {
		folder_menu_count =
			list_get(
				count_list );

		fprintf(output_file,
			"%s%c%ld\n",
			folder_menu_count->folder_name,
			SQL_DELIMITER,
			folder_menu_count->count );

	} while ( list_next( count_list ) );

	fclose( output_file );
}

char *folder_menu_filename(
			char *application_name,
			char *session_key,
			char *role_name,
			char *data_directory )
{
	char filename[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !role_name
	||   !data_directory )
	{
		return (char *)0;
	}

	sprintf( filename,
		 "%s/%s_%s_%s.txt",
		 data_directory,
		 application_name,
		 role_name,
		 session_key );

	return strdup( filename );
}

FOLDER_MENU_COUNT *folder_menu_count_seek(
			char *folder_name,
			LIST *folder_menu_count_list )
{
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( ! list_rewind( folder_menu_count_list ) )
		return (FOLDER_MENU_COUNT *)0;

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

	return (FOLDER_MENU_COUNT *)0;
}

char *folder_menu_count_display(
			unsigned long count )
{
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	return place_commas_in_unsigned_long( count );
}
