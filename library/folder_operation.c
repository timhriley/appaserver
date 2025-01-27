/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_operation.c				*/
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
#include "role_folder.h"
#include "folder_operation.h"

LIST *folder_operation_list(
		char *folder_name,
		char *role_name,
		boolean fetch_operation,
		boolean fetch_process )
{
	return
	folder_operation_system_list(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_operation_system_string(
			FOLDER_OPERATION_SELECT,
			FOLDER_OPERATION_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_operation_where(
				FOLDER_PRIMARY_KEY,
				folder_name,
				role_name ) ),
		fetch_operation,
		fetch_process );
}

FOLDER_OPERATION *folder_operation_calloc( void )
{
	FOLDER_OPERATION *folder_operation;

	if ( ! ( folder_operation = calloc( 1, sizeof ( FOLDER_OPERATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return folder_operation;
}

FOLDER_OPERATION *folder_operation_new( 
		char *folder_name,
		char *role_name,
		char *operation_name )
{
	FOLDER_OPERATION *folder_operation = folder_operation_calloc();

	folder_operation->folder_name = folder_name;
	folder_operation->role_name = role_name;
	folder_operation->operation_name = operation_name;

	return folder_operation;
}

FOLDER_OPERATION *folder_operation_parse(
		char *input,
		boolean fetch_operation,
		boolean fetch_process )
{
	FOLDER_OPERATION *folder_operation;
	char folder_name[ 128 ];
	char role_name[ 128 ];
	char operation_name[ 128 ];

	if ( !input || !*input ) return NULL;

	/* See FOLDER_OPERATION_SELECT */
	/* ------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( role_name, SQL_DELIMITER, input, 1 );
	piece( operation_name, SQL_DELIMITER, input, 2 );

	folder_operation =
		folder_operation_new(
			strdup( folder_name ),
			strdup( role_name ),
			strdup( operation_name ) );

	if ( fetch_operation )
	{
		folder_operation->operation =
			operation_fetch(
				operation_name,
				fetch_process );
	}

	return folder_operation;
}

char *folder_operation_system_string(
		char *folder_operation_select,
		char *folder_operation_table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		folder_operation_select,
		folder_operation_table,
		where );

	return strdup( system_string );
}

LIST *folder_operation_system_list(
		char *system_string,
		boolean fetch_operation,
		boolean fetch_process )
{
	char input[ 1024 ];
	FILE *pipe;
	LIST *list = {0};
	FOLDER_OPERATION *folder_operation;

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( folder_operation =
			folder_operation_parse(
				input,
				fetch_operation,
				fetch_process ) ) )
		{
			if ( !list ) list = list_new();
			list_set( list, folder_operation );
		}
	}

	pclose( pipe );
	return list;
}

LIST *folder_operation_name_list( LIST *folder_operation_list )
{
	FOLDER_OPERATION *folder_operation;
	LIST *name_list;

	if ( !list_rewind( folder_operation_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		folder_operation =
			list_get(
				folder_operation_list );

		list_set( name_list, folder_operation->operation_name );

	} while ( list_next( folder_operation_list ) );

	return name_list;
}

char *folder_operation_where(
		const char *folder_primary_key,
		char *folder_name,
		char *role_name )
{
	static char where[ 128 ];

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

	if ( role_name )
	{
		snprintf(
			where,
			sizeof ( where ),
			"%s = '%s' and role = '%s'",
			folder_primary_key,
			folder_name,
			role_name );
	}
	else
	{
		snprintf(
			where,
			sizeof ( where ),
			"%s = '%s'",
			folder_primary_key,
			folder_name );
	}

	return where;
}

boolean folder_operation_delete_boolean( LIST *folder_operation_list )
{
	FOLDER_OPERATION *folder_operation;

	if ( !list_rewind( folder_operation_list ) ) return 0;

	do {
		folder_operation =
			list_get(
				folder_operation_list );

		if ( string_strncmp(
			folder_operation->operation_name,
			"delete" ) == 0 )
		{
			return 1;
		}

	} while ( list_next( folder_operation_list ) );

	return 0;
}
