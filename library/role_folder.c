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

char *role_folder_primary_where(
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

LIST *role_folder_list(
			char *role_name,
			char *folder_name )
{
	return
	role_folder_system_list(
		role_folder_primary_where(
			role_name,
			folder_name ) );
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

char *role_folder_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		ROLE_FOLDER_SELECT,
		ROLE_FOLDER_TABLE,
		where );

	return strdup( system_string );
}

LIST *role_folder_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *pipe;
	LIST *list = {0};
	ROLE_FOLDER *role_folder;

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

char *role_folder_subschema_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh \"%s\" %s,%s \"%s\"",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_folder_subschema_select(),
		ROLE_FOLDER_TABLE,
		FOLDER_TABLE,
		where );

	return strdup( system_string );
}

LIST *role_folder_subschema_list(
			char *role_name )
{
	LIST *role_folder_list = list_new();
	char input[ 256 ];
	FILE *input_pipe;

	input_pipe =
		popen(
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			role_folder_subschema_system_string(
				/* -------------------------- */
				/* Safely returns heap memory */
				/* -------------------------- */
				role_folder_subschema_where(
					role_name ) ),
			"r" );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			role_folder_list,
			role_folder_subschema_parse( input ) );
	}

	pclose( input_pipe );
	return role_folder_list;
}

char *role_folder_subschema_select( void )
{
	static char select[ 256 ];

	sprintf(select,
		"%s.role,%s.folder,%s.permission,%s.subschema",
		ROLE_FOLDER_TABLE,
		ROLE_FOLDER_TABLE,
		ROLE_FOLDER_TABLE,
		FOLDER_TABLE );

	return select;
}

char *role_folder_subschema_where( char *role_name )
{
	char where[ 256 ];

	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"%s.role = '%s' and %s.folder = %s.folder",
		ROLE_FOLDER_TABLE,
		role_name,
		ROLE_FOLDER_TABLE,
		FOLDER_TABLE );

	return strdup( where );
}

ROLE_FOLDER *role_folder_subschema_parse( char *input )
{
	ROLE_FOLDER *role_folder = role_folder_calloc();
	char piece_buffer[ 128 ];

	/* See role_folder_subschema_select() */
	/* ---------------------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );
	role_folder->role_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	role_folder->folder_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	role_folder->permission = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	role_folder->subschema_name = strdup( piece_buffer );

	return role_folder;
}

LIST *role_folder_subschema_name_list(
			LIST *role_folder_lookup_insert_list )
{
	ROLE_FOLDER *role_folder;
	LIST *name_list = list_new();

	if ( list_rewind( role_folder_lookup_insert_list ) )
	{
		do {
			role_folder =
				list_get(
					role_folder_lookup_insert_list );

			if ( role_folder->subschema_name
			&&   *role_folder->subschema_name )
			{
				if ( list_exists_string(
					role_folder->subschema_name,
					name_list ) )
				{
					continue;
				}

				list_add_string_in_order(
					name_list,
					role_folder->subschema_name );
			}
		} while ( list_next( role_folder_lookup_insert_list ) );
	}

	return name_list;
}

LIST *role_folder_lookup_name_list(
			char *subschema_name,
			LIST *role_folder_lookup_list )
{
	ROLE_FOLDER *role_folder;
	LIST *name_list = list_new();

	if ( list_rewind( role_folder_lookup_list ) )
	{
		do {
			role_folder =
				list_get(
					role_folder_lookup_list );

			if ( ( ( string_strcmp(
					role_folder->permission,
					"lookup" ) == 0 )
			||     ( string_strcmp(
					role_folder->permission,
					"update" ) == 0 ) )
			&& 	 string_strcmp(
					role_folder->subschema_name,
					subschema_name ) == 0 )
			{
				if ( list_exists_string(
					role_folder->folder_name,
					name_list ) )
				{
					continue;
				}

				list_add_string_in_order(
					name_list,
					role_folder->folder_name );
			}
		} while ( list_next( role_folder_lookup_list ) );
	}

	return name_list;
}

LIST *role_folder_insert_name_list(
			char *subschema_name,
			LIST *role_folder_insert_list )
{
	ROLE_FOLDER *role_folder;
	LIST *name_list = list_new();

	if ( list_rewind( role_folder_insert_list ) )
	{
		do {
			role_folder =
				list_get(
					role_folder_insert_list );

			if ( ( string_strcmp(
					role_folder->permission,
					"insert" ) == 0 )
			&& 	 string_strcmp(
					role_folder->subschema_name,
					subschema_name ) == 0 )
			{
				if ( list_exists_string(
					role_folder->folder_name,
					name_list ) )
				{
					continue;
				}

				list_add_string_in_order(
					name_list,
					role_folder->folder_name );
			}
		} while ( list_next( role_folder_insert_list ) );
	}

	return name_list;
}

