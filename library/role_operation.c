/* $APPASERVER_HOME/library/role_operation.c				*/
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
#include "role_folder.h"
#include "role_operation.h"

LIST *role_operation_fetch_list(
			char *folder_name,
			char *role_name )
{
	return
	role_operation_system_list(
		role_folder_primary_where(
			role_name,
			folder_name ) );
}

ROLE_OPERATION *role_operation_new( 
			char *folder_name,
			char *role_name,
			char *operation_name )
{
	ROLE_OPERATION *role_operation;

	if ( ! ( role_operation = calloc( 1, sizeof( ROLE_OPERATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	role_operation->folder_name = folder_name;
	role_operation->role_name = role_name;
	role_operation->operation_name = operation_name;

	return role_operation;
}

ROLE_OPERATION *role_operation_parse(
			char *input )
{
	ROLE_OPERATION *role_operation;
	char folder_name[ 128 ];
	char role_name[ 128 ];
	char operation_name[ 128 ];

	if ( !input || !*input ) return (ROLE_OPERATION *)0;

	/* See ROLE_OPERATION_SELECT */
	/* ------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( role_name, SQL_DELIMITER, input, 1 );
	piece( operation_name, SQL_DELIMITER, input, 2 );

	role_operation =
		role_operation_new(
			strdup( folder_name ),
			strdup( role_name ),
			strdup( operation_name ) );

	return role_operation;
}

char *role_operation_system_string(
			char *where )
{
	char system_string[ 1024 ];

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		ROLE_OPERATION_SELECT,
		ROLE_OPERATION_TABLE,
		where );

	return strdup( system_string );
}

LIST *role_operation_system_list(
			char *system_string )
{
	char input[ 1024 ];
	FILE *pipe;
	LIST *list = {0};
	ROLE_OPERATION *role_operation;

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 1024 ) )
	{
		if ( ( role_operation = role_operation_parse( input ) ) )
		{
			if ( !list ) list = list_new();
			list_set( list, role_operation );
		}
	}

	pclose( pipe );
	return list;
}

LIST *role_operation_name_list(
			LIST *role_operation_list )
{
	ROLE_OPERATION *role_operation;
	LIST *name_list;

	if ( !list_rewind( role_operation_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		role_operation =
			list_get(
				role_operation_list );

		list_set( name_list, role_operation->operation_name );

	} while ( list_next( role_operation_list ) );

	return name_list;
}

char *role_operation_primary_where(
			char *folder_name,
			char *role_name,
			char *operation_name )
{
	static char where[ 256 ];

	sprintf(where,
		"folder = '%s' and role = '%s' and operation = '%s'",
		folder_name,
		role_name,
		operation_name );

	return where;
}

