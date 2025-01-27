/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/row_security_role_update.c	   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "folder.h"
#include "role_folder.h"
#include "folder_attribute.h"
#include "row_security_role_update.h"

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_calloc( void )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	if ( ! ( row_security_role_update =
			calloc(	1,
				sizeof ( ROW_SECURITY_ROLE_UPDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return row_security_role_update;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_parse(
			char *role_name,
			char *input )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	char folder_name[ 128 ];
	char attribute_not_null[ 128 ];

	if ( !input || !*input ) return NULL;

	row_security_role_update = row_security_role_update_calloc();

	/* See ROW_SECURITY_ROLE_UPDATE_SELECT */
	/* ----------------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	row_security_role_update->folder_name = strdup( folder_name );

	piece( attribute_not_null, SQL_DELIMITER, input, 1 );

	row_security_role_update->attribute_not_null =
		strdup( attribute_not_null );

	row_security_role_update->relation_one2m_recursive_list =
		relation_one2m_recursive_list(
			(LIST *)0 /* one2m_list */,
			role_name,
			row_security_role_update->folder_name
				/* one_folder_name */,
			folder_attribute_fetch_primary_key_list(
				row_security_role_update->
					folder_name )
				/* one_folder_primary_key_list */ );

	return row_security_role_update;
}

FILE *row_security_role_update_input_pipe( char *system_string )
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

char *row_security_role_update_system_string(
			char *select,
			char *table,
			char *role_folder_lookup_in_clause )
{
	static char system_string[ 256 ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		select,
		table,
		role_folder_lookup_in_clause );

	return system_string;
}

ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list_fetch(
		char *application_name,
		char *role_name,
		char *folder_name,
		boolean role_override_row_restrictions )
{
	ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list;
	FILE *input_pipe;
	char input[ 256 ];

	if ( !application_name
	||   !role_name
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

	if ( role_override_row_restrictions ) return NULL;

	row_security_role_update_list = row_security_role_update_list_calloc();
	row_security_role_update_list->list = list_new(); 

	row_security_role_update_list->role_folder_lookup_in_clause =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_folder_lookup_in_clause(
			ROLE_FOLDER_TABLE,
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			FOLDER_PRIMARY_KEY,
			role_name );

	input_pipe =
		row_security_role_update_input_pipe(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			row_security_role_update_system_string(
				ROW_SECURITY_ROLE_UPDATE_SELECT,
				ROW_SECURITY_ROLE_UPDATE_TABLE,
				row_security_role_update_list->
					role_folder_lookup_in_clause ) );

	while ( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			row_security_role_update_list->list,
			row_security_role_update_parse(
				role_name,
				input ) );
	}

	pclose( input_pipe );

	if ( !list_length( row_security_role_update_list->list ) )
	{
		list_free( row_security_role_update_list->list );
		free( row_security_role_update_list );
		return NULL;
	}

	row_security_role_update_list->
		row_security_role_update =
			row_security_role_update_seek(
				folder_name,
				row_security_role_update_list->list );

	if ( row_security_role_update_list->row_security_role_update )
	{
		row_security_role_update_list->attribute_not_null =
			row_security_role_update_list->
				row_security_role_update->
				attribute_not_null;

		return row_security_role_update_list;
	}

	row_security_role_update_list->
		row_security_role_update_relation =
			row_security_role_update_relation_seek(
				folder_name,
				row_security_role_update_list->list );

	if ( !row_security_role_update_list->row_security_role_update_relation )
	{
		free( row_security_role_update_list );
		return NULL;
	}

	if ( !row_security_role_update_list->
		row_security_role_update_relation->
		relation_one2m
	||   !list_length(
		row_security_role_update_list->
			row_security_role_update_relation->
			relation_one2m->
			relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
	"row_security_role_update_relation_seek(%s) returned incomplete.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	row_security_role_update_list->attribute_not_null =
		row_security_role_update_list->
			row_security_role_update_relation->
			attribute_not_null;

	row_security_role_update_list->from =
		row_security_role_update_list->
			row_security_role_update_relation->
			relation_one2m->one_folder_name;

	row_security_role_update_list->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		row_security_role_update_list_where(
			application_name,
			folder_name,
			row_security_role_update_list->
				row_security_role_update_relation->
				relation_one2m->one_folder_name,
			row_security_role_update_list->
				row_security_role_update_relation->
				relation_one2m->
				relation_foreign_key_list );

	return row_security_role_update_list;
}

ROW_SECURITY_ROLE_UPDATE_LIST *
	row_security_role_update_list_calloc(
		void )
{
	ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list;

	if ( ! ( row_security_role_update_list =
			calloc(	1,
				sizeof ( ROW_SECURITY_ROLE_UPDATE_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return row_security_role_update_list;
}

char *row_security_role_update_list_where(
			char *application_name,
			char *folder_name,
			char *one_folder_name,
			LIST *foreign_key_list )
{
	char where[ 1024 ];
	char *ptr = where;
	char *foreign_key;
	char table_name[ 128 ];
	char *one_table_name;

	if ( !application_name
	||   !folder_name
	||   !one_folder_name
	||   !list_rewind( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy(	table_name,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name ) );

	one_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			one_folder_name );

	do {
		foreign_key = list_get( foreign_key_list );

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s.%s = %s.%s",
			table_name,
			foreign_key,
			one_table_name,
			foreign_key );

	} while ( list_next( foreign_key_list ) );

	return strdup( where );
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_seek(
		char *folder_name,
		LIST *list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

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

	if ( !list_rewind( list ) ) return (ROW_SECURITY_ROLE_UPDATE *)0;

	do {
		row_security_role_update = list_get( list );

		if ( strcmp(
			folder_name,
			row_security_role_update->folder_name ) == 0 )
		{
			return row_security_role_update;
		}

	} while ( list_next( list ) );

	return (ROW_SECURITY_ROLE_UPDATE *)0;
}

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_relation_seek(
		char *folder_name,
		LIST *list )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

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

	if ( list_rewind( list ) )
	do {
		row_security_role_update = list_get( list );

		if ( row_security_role_update_relation_set(
			row_security_role_update /* in/out */,
			folder_name ) )
		{
			return row_security_role_update;
		}

	} while ( list_next( list ) );

	return (ROW_SECURITY_ROLE_UPDATE *)0;
}

boolean row_security_role_update_relation_set(
		ROW_SECURITY_ROLE_UPDATE *row_security_role_update,
		char *folder_name )
{
	RELATION_ONE2M *relation_one2m;

	if ( !row_security_role_update
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

	if ( list_rewind(
		row_security_role_update->
			relation_one2m_recursive_list ) )
	do {

		relation_one2m =
			list_get(
				row_security_role_update->
					relation_one2m_recursive_list );

		if ( !relation_one2m->many_folder_name )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_one2m->many_folder_name is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( strcmp(
			folder_name,
			relation_one2m->many_folder_name ) == 0 )
		{
			row_security_role_update->relation_one2m =
				relation_one2m;
			return 1;
		}

	} while ( list_next(
			row_security_role_update->
				relation_one2m_recursive_list ) );

	return 0;
}

