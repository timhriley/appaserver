/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role_folder.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "folder.h"
#include "role_folder.h"

char *role_folder_where(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name,
		char *folder_name )
{
	static char where[ 128 ];

	if ( !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"role = '%s' and %s.%s = '%s'",
		role_name,
		role_folder_table,
		folder_primary_key,
		folder_name );

	return where;
}

LIST *role_folder_list(
		char *role_name,
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
				ROLE_FOLDER_TABLE,
				FOLDER_PRIMARY_KEY,
				role_name,
				folder_name ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_left_join(
				ROLE_FOLDER_TABLE,
				FOLDER_TABLE,
				FOLDER_PRIMARY_KEY ) ) );
}

ROLE_FOLDER *role_folder_calloc( void )
{
	ROLE_FOLDER *role_folder;

	if ( ! ( role_folder = calloc( 1, sizeof ( ROLE_FOLDER ) ) ) )
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
	char subschema_name[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See ROLE_FOLDER_SELECT */
	/* ---------------------- */
	piece( role_name, SQL_DELIMITER, input, 0 );
	piece( folder_name, SQL_DELIMITER, input, 1 );
	piece( permission, SQL_DELIMITER, input, 2 );
	piece( subschema_name, SQL_DELIMITER, input, 3 );

	if ( strcasecmp( folder_name, "null" ) == 0 ) return NULL;

	role_folder =
		role_folder_new(
			strdup( role_name ),
			strdup( folder_name ),
			strdup( permission ) );

	if ( *subschema_name )
	{
		role_folder->subschema_name = strdup( subschema_name );
	}

	return role_folder;
}

char *role_folder_system_string(
		const char *role_folder_select,
		const char *role_folder_table,
		char *where,
		char *role_folder_left_join )
{
	char system_string[ 1024 ];

	if ( !where
	||   !role_folder_left_join )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"select %s from %s %s where %s;\" | sql.e",
		role_folder_select,
		role_folder_table,
		role_folder_left_join,
		where );

	return strdup( system_string );
}

FILE *role_folder_input_pipe( char *system_string )
{
	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return popen( system_string, "r" );
}

LIST *role_folder_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_pipe = role_folder_input_pipe( system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( list, role_folder_parse( input ) );
	}

	pclose( input_pipe );

	return list;
}

boolean role_folder_insert_boolean(
		const char *role_permssion_insert,
		char *folder_name,
		LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_folder_list ) )
	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp(
			role_folder->folder_name,
			folder_name ) == 0
		&&   strcmp(
			role_folder->permission,
			role_permssion_insert ) == 0 )
		{
			return 1;
		}

	} while ( list_next( role_folder_list ) );

	return 0;
}

boolean role_folder_update_boolean(
		const char *role_permssion_update,
		char *folder_name,
		LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_folder_list ) )
	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp(
			role_folder->folder_name,
			folder_name ) == 0
		&&   strcmp(
			role_folder->permission,
			role_permssion_update ) == 0 )
		{
			return 1;
		}

	} while ( list_next( role_folder_list ) );

	return 0;
}

boolean role_folder_lookup_boolean(
		const char *role_permission_lookup,
		const char *role_permission_update,
		char *folder_name,
		LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( role_folder_list ) )
	do {
		role_folder = list_get( role_folder_list );

		if ( strcmp(	role_folder->folder_name,
				folder_name ) == 0 )
		{
			if ( strcmp(
				role_folder->permission,
				role_permission_lookup ) == 0
			||   strcmp(
				role_folder->permission,
				role_permission_update ) == 0 )
			{
				return 1;
			}
		}

	} while ( list_next( role_folder_list ) );

	return 0;
}

char *role_folder_lookup_where(
		const char *role_permission_lookup,
		const char *role_permission_update,
		char *role_name )
{
	static char where[ 128 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"role = '%s' and permission in ('%s','%s')",
		role_name,
		role_permission_lookup,
		role_permission_update );

	return where;
}

char *role_folder_insert_where(
		const char *role_permission_insert,
		char *role_name )
{
	static char where[ 128 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"role = '%s' and permission = '%s'",
		role_name,
		role_permission_insert );

	return where;
}

LIST *role_folder_lookup_list( char *role_name )
{
	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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
			role_folder_lookup_where(
				ROLE_PERMISSION_LOOKUP,
				ROLE_PERMISSION_UPDATE,
				role_name ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_left_join(
				ROLE_FOLDER_TABLE,
				FOLDER_TABLE,
				FOLDER_PRIMARY_KEY ) ) );
}

LIST *role_folder_insert_list( char *role_name )
{
	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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
			role_folder_insert_where(
				ROLE_PERMISSION_INSERT,
				role_name ),
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			role_folder_left_join(
				ROLE_FOLDER_TABLE,
				FOLDER_TABLE,
				FOLDER_PRIMARY_KEY ) ) );
}

LIST *role_folder_name_list( LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	LIST *folder_name_list = list_new();

	if ( list_rewind( role_folder_list ) )
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

	return folder_name_list;
}

ROLE_FOLDER *role_folder_permission_seek(
		char *folder_name,
		char *permission,
		LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;

	if ( list_rewind( role_folder_list ) )
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

	return NULL;
}

boolean role_folder_insert_exists(
		char *folder_name,
		LIST *role_folder_list )
{
	if ( role_folder_permission_seek(
		folder_name,
		ROLE_PERMISSION_INSERT,
		role_folder_list ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *role_folder_left_join(
		const char *role_folder_table,
		const char *folder_table,
		const char *folder_primary_key )
{
	static char where[ 128 ];

	if ( *where ) return where;

	snprintf(
		where,
		sizeof ( where ),
		"left join %s on %s.%s = %s.%s",
		folder_table,
		role_folder_table,
		folder_primary_key,
		folder_table,
		folder_primary_key );

	return where;
}

LIST *role_folder_subschema_folder_name_list(
		char *subschema_name,
		LIST *role_folder_list )
{
	ROLE_FOLDER *role_folder;
	LIST *folder_name_list = list_new();

	if ( list_rewind( role_folder_list ) )
	do {
		role_folder =
			list_get(
				role_folder_list );

		if ( string_strcmp(
			role_folder->subschema_name,
			subschema_name ) == 0 )
		{
			if ( list_exists_string(
				role_folder->folder_name,
				folder_name_list ) )
			{
				continue;
			}

			list_add_string_in_order(
				folder_name_list,
				role_folder->folder_name );
		}
	} while ( list_next( role_folder_list ) );

	return folder_name_list;
}

LIST *role_folder_subschema_missing_folder_name_list(
		LIST *role_folder_lookup_list )
{
	ROLE_FOLDER *role_folder;
	LIST *folder_name_list;

	if ( !list_rewind( role_folder_lookup_list ) ) return (LIST *)0;

	folder_name_list = list_new();

	do {
		role_folder = list_get( role_folder_lookup_list );

		if ( !role_folder->subschema_name )
		{
			list_append_unique_string(
				folder_name_list,
				role_folder->folder_name );
		}

	} while( list_next( role_folder_lookup_list ) );

	return folder_name_list;
}

LIST *role_folder_subschema_name_list( LIST *role_folder_lookup_list )
{
	ROLE_FOLDER *role_folder;
	LIST *subschema_name_list;

	if ( !list_rewind( role_folder_lookup_list ) ) return (LIST *)0;

	subschema_name_list = list_new();

	do {
		role_folder =
			list_get(
				role_folder_lookup_list );

		if ( role_folder->subschema_name )
		{
			if ( !list_exists_string(
				role_folder->subschema_name,
				subschema_name_list ) )
			{
				list_add_string_in_order(
					subschema_name_list,
					role_folder->subschema_name );
			}
		}

	} while ( list_next( role_folder_lookup_list ) );

	return subschema_name_list;
}

char *role_folder_lookup_in_clause(
		const char *role_folder_table,
		const char *role_permission_lookup,
		const char *role_permission_update,
		const char *folder_primary_key,
		char *role_name )
{
	static char in_clause[ 256 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		in_clause,
		sizeof ( in_clause ),
		"%s in ( select distinct %s from %s %s )",
		folder_primary_key,
		folder_primary_key,
		role_folder_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_folder_permission_where(
			role_permission_lookup,
			role_permission_update,
			role_name ) );

	return in_clause;
}

char *role_folder_permission_where(
		const char *role_permission_lookup,
		const char *role_permission_update,
		char *role_name )
{
	static char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"where role = '%s' and permission in ('%s','%s')",
		role_name,
		role_permission_lookup,
		role_permission_update );

	return where;
}

char *role_folder_list_display( LIST *role_folder_list )
{
	char display[ 65536 ];
	char *ptr = display;
	ROLE_FOLDER *role_folder;

	*ptr = '\0';

	if ( list_rewind( role_folder_list ) )
	do {
		role_folder = list_get( role_folder_list );

		if ( ptr != display ) ptr += sprintf( ptr, "; " );

		ptr += sprintf( ptr,
			"role=%s,folder=%s,permission=%s,subschema=%s",
			role_folder->role_name,
			role_folder->folder_name,
			role_folder->permission,
			(role_folder->subschema_name)
				? role_folder->subschema_name
				: "" );

	} while ( list_next( role_folder_list ) );

	return strdup( display );
}

LIST *role_folder_fetch_name_list(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name )
{
	char where[ 128 ];
	char system_string[ 1024 ];

	if ( !role_name )
	{
		char message[ 128 ];

		sprintf(message, "role_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"role = '%s' and %s <> 'null'",
		role_name,
		folder_primary_key );

	sprintf(system_string,
		"select.sh %s %s \"%s\" | sort -u",
		folder_primary_key,
		role_folder_table,
		where );

	return list_pipe( system_string );
}

