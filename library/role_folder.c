/* $APPASERVER_HOME/library/role_folder.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environ.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "list.h"
#include "folder.h"
#include "role_folder.h"

ROLE_FOLDER *role_folder_fetch(
			char *role_name,
			char *folder_name )
{
	return role_folder_new(
			role_name,
			folder_name );
}

ROLE_FOLDER *role_folder_new(
			char *role_name,
			char *folder_name )
{
	ROLE_FOLDER *role_folder;

	role_folder = (ROLE_FOLDER *)calloc( 1, sizeof( ROLE_FOLDER ) );

	role_folder->role_name = role_name;
	role_folder->folder_name = folder_name;

	role_folder_load(
		&role_folder->insert_yn,
		&role_folder->update_yn,
		&role_folder->lookup_yn,
		&role_folder->delete_yn,
		role_name,
		folder_name );

	return role_folder;
}

void role_folder_load( 	
			char *insert_yn,
			char *update_yn,
			char *lookup_yn,
			char *delete_yn,
			char *role_name,
			char *folder_name )
{
	char sys_string[ 1024 ];
	LIST *results_list;
	char *permission;
	char *role_operation_table_name;

	sprintf( sys_string, 
		 "permission4role_folder.sh %s '' %s %s 2>>%s",
		 environment_application_name(),
		 role_name,
		 folder_name,
		 appaserver_error_get_filename( application_name ) );

	results_list = pipe2list( sys_string );

	if ( !list_rewind( results_list ) )
	{
		*insert_yn = 'n';
		*update_yn = 'n';
		*lookup_yn = 'n';
		return;
	}

	do {
		permission = list_get_string( results_list );

		if ( strcmp( permission, "insert" ) == 0 ) *insert_yn = 'y';
		if ( strcmp( permission, "update" ) == 0 ) *update_yn = 'y';
		if ( strcmp( permission, "lookup" ) == 0 ) *lookup_yn = 'y';

	} while( list_next( results_list ) );

	role_operation_table_name =
		get_table_name( application_name,
				"role_operation" ) ;

	sprintf( sys_string,
		 "echo \"	select 'y'				 "
		 "		from %s					 "
		 "		where folder = '%s'			 "
		 "		  and role = '%s'			 "
		 "		  and operation = 'delete';\"		|"
		 "sql.e							 ",
		 role_operation_table_name,
		 folder_name,
		 role_name );

	permission = pipe2string( sys_string );

	if ( permission && *permission == 'y' )
		*delete_yn = 'y';
	else
		*delete_yn = 'n';
}

void role_folder_populate_folder_insert_permission(
			LIST *folder_list, 
			LIST *role_folder_insert_list,
			char *application_name )
{
	FOLDER *folder;
	char *folder_name;

	if ( list_rewind( role_folder_insert_list ) )
	{
		do {
			folder_name = 
				list_get_pointer( role_folder_insert_list );

			if ( strcmp( folder_name, "null" ) == 0 ) continue;

			folder = folder_seek_folder( folder_list, folder_name );

			if ( !folder )
			{
				char msg[ 1024 ];
				sprintf( msg,
			"Warning in %s/%s()/%d: cannot find folder = (%s)",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 folder_name );
				appaserver_output_error_message(
					application_name, msg, (char *)0 );
				continue;
			}
			folder->insert_permission = 1;
		} while( list_next( role_folder_insert_list ) );
	}
}

void role_folder_populate_folder_update_permission(
			LIST *folder_list, 
			LIST *role_folder_update_list,
			char *application_name )
{
	FOLDER *folder;
	char *folder_name;

	if ( list_rewind( role_folder_update_list ) )
	{
		do {
			folder_name = 
				list_get_pointer( role_folder_update_list );

			if ( strcmp( folder_name, "null" ) == 0 ) continue;

			folder = folder_seek_folder( folder_list, folder_name );

			if ( !folder )
			{
				char msg[ 1024 ];
				sprintf( msg,
			"Warning in %s.%s(): cannot find folder = (%s)",
					 __FILE__,
					 __FUNCTION__,
					 folder_name );
				appaserver_output_error_message(
					application_name, msg, (char *)0 );
				continue;
			}
			folder->update_permission = 1;
		} while( list_next( role_folder_update_list ) );
	}

}

void role_folder_populate_folder_lookup_permission(
			LIST *folder_list, 
			LIST *role_folder_lookup_list,
			char *application_name )
{
	FOLDER *folder;
	char *folder_name;

	if ( list_rewind( role_folder_lookup_list ) )
	{
		do {
			folder_name = 
				list_get_pointer( role_folder_lookup_list );

			if ( strcmp( folder_name, "null" ) == 0 ) continue;

			folder = folder_seek_folder( folder_list, folder_name );

			if ( !folder )
			{
				char msg[ 1024 ];
				sprintf( msg,
			"Warning in %s.%s(): cannot find folder = (%s)",
					 __FILE__,
					 __FUNCTION__,
					 folder_name );
				appaserver_output_error_message(
					application_name, msg, (char *)0 );
				continue;
			}
			folder->lookup_permission = 1;
		} while( list_next( role_folder_lookup_list ) );
	}

}

LIST *role_folder_insert_list_fetch(
			char *role_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "get_role_folder_insert_list.sh %s '' %s 2>/dev/null",
		 environment_application_name(),
		 role_name );

	return pipe2list( sys_string );
}

LIST *role_folder_lookup_list_fetch(
			char *role_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "get_role_folder_lookup_list.sh %s '' %s 2>/dev/null",
		 environment_application_name(),
		 role_name );

	return pipe2list( sys_string );
}

LIST *role_folder_update_list_fetch(
			char *role_name )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, 
		 "get_role_folder_update_list.sh %s '' %s 2>/dev/null",
		 environment_application_name(),
		 role_name );

	return pipe2list( sys_string );
}

char *role_folder_display( ROLE_FOLDER *role_folder )
{
	char buffer[ 4096 ];

	sprintf( buffer,
		"role_name = %s\n"
		"folder_name = %s\n"
		"insert_yn = %d\n"
		"update_yn = %d\n"
		"lookup_yn = %d\n",
		role_folder->role_name,
		role_folder->folder_name,
		role_folder->insert_yn,
		role_folder->update_yn,
		role_folder->lookup_yn );
	return strdup( buffer );
}

boolean role_folder_viewonly( ROLE_FOLDER *role_folder )
{
	if ( !role_folder ) return 0;

	return (role_folder->update_yn != 'y' &&
		role_folder->lookup_yn == 'y' );

}

