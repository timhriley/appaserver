/* $APPASERVER_HOME/library/role_folder.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "folder.h"
#include "role_folder.h"

char *role_folder_where(
			char *role_name,
			char *folder_name )
{
	static char where[ 128 ];

	sprintf(where,
		"role = '%s' and folder = '%s'",
		role_name,
		folder_name );

	return where;
}

LIST *role_folder_list(	char *role_name,
			char *folder_name )
{
	return
	role_folder_system_list(
		role_folder_system_string(
			ROLE_FOLDER_SELECT,
			ROLE_FOLDER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_where(
				role_name,
				folder_name ) ) );
}

ROLE_FOLDER *role_folder_calloc( void )
{
	ROLE_FOLDER *role_folder;

	if ( ! ( role_folder = calloc( 1, sizeof( ROLE_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return role_folder;
}

ROLE_FOLDER *role_folder_new( 
			char *role_name,
			char *folder_name,
			char *permission )
{
	ROLE_FOLDER *role_folder = role_folder_calloc();

	role_folder->role_name = role_name;
	role_folder->folder_name = folder_name;
	role_folder->permission = permission;

	return role_folder;
}

ROLE_FOLDER *role_folder_parse( char *input )
{
	ROLE_FOLDER *role_folder;
	char role_name[ 128 ];
	char folder_name[ 128 ];
	char permission[ 128 ];

	if ( !input || !*input ) return (ROLE_FOLDER *)0;

	/* See ROLE_FOLDER_SELECT */
	/* ---------------------- */
	piece( role_name, SQL_DELIMITER, input, 0 );
	piece( folder_name, SQL_DELIMITER, input, 1 );
	piece( permission, SQL_DELIMITER, input, 2 );

	role_folder =
		role_folder_new(
			strdup( role_name ),
			strdup( folder_name ),
			strdup( permission ) );

	return role_folder;
}

char *role_folder_system_string(
			char *role_folder_select,
			char *role_folder_table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		role_folder_select,
		role_folder_table,
		where );

	return strdup( system_string );
}

LIST *role_folder_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *pipe;
	LIST *list = {0};
	ROLE_FOLDER *role_folder;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( role_folder = role_folder_parse( input ) ) )
		{
			if ( !list ) list = list_new();
			list_set( list, role_folder );
		}
	}

	pclose( pipe );
	return list;
}

boolean role_folder_insert( LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !list_rewind( role_folder_list ) ) return 0;

	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp( role_folder->permission, "insert" ) == 0 )
			return 1;

	} while ( list_next( role_folder_list ) );

	return 0;
}

boolean role_folder_update( LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !list_rewind( role_folder_list ) ) return 0;

	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp( role_folder->permission, "update" ) == 0 )
			return 1;

	} while ( list_next( role_folder_list ) );

	return 0;
}

boolean role_folder_lookup( LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !list_rewind( role_folder_list ) ) return 0;

	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp( role_folder->permission, "lookup" ) == 0 )
			return 1;

	} while ( list_next( role_folder_list ) );

	return 0;
}

char *role_folder_lookup_where( char *role_name )
{
	static char where[ 128 ];

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"role_name = '%s' and "
		"permission in ('lookup','update')",
		role_name );

	return where;
}

char *role_folder_insert_where( char *role_name )
{
	static char where[ 128 ];

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"role_name = '%s' and "
		"permission = 'insert'",
		role_name );

	return where;
}

LIST *role_folder_lookup_list( char *role_name )
{
	return
	role_folder_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		role_folder_system_string(
			ROLE_FOLDER_SELECT,
			ROLE_FOLDER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_lookup_where( role_name ) ) );
}

LIST *role_folder_insert_list( char *role_name )
{
	return
	role_folder_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		role_folder_system_string(
			ROLE_FOLDER_SELECT,
			ROLE_FOLDER_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_insert_where( role_name ) ) );
}

LIST *role_folder_name_list(
			LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	LIST *folder_name_list = list_new();

	if ( list_rewind( role_folder_list ) )
	{
		do {
			role_folder =
				list_get(
					role_folder_list );

			if ( list_exists_string(
				role_folder->folder_name,
				folder_name_list ) )
			{
				continue;
			}

			list_add_string_in_order(
				folder_name_list,
				role_folder->folder_name );

		} while ( list_next( role_folder_list ) );
	}

	return folder_name_list;
}

ROLE_FOLDER *role_folder_permission_seek(
			char *folder_name,
			char *permission,
			LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( list_rewind( role_folder_list ) )
	{
		do {
			role_folder =
				list_get(
					role_folder_list );

			if ( string_strcmp(
				role_folder->folder_name,
				folder_name ) == 0
			&&   string_strcmp(
				role_folder->permission,
				permission ) == 0 )
			{
				return role_folder;
			}

		} while ( list_next( role_folder_list ) );
	}

	return (ROLE_FOLDER *)0;
}

boolean role_folder_insert_exists(
			char *folder_name,
			LIST *role_folder_list )
{
	if ( role_folder_permission_seek(
		folder_name,
		"insert",
		role_folder_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

