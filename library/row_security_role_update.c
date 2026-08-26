/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/row_security_role_update.c	   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "all.h"
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
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
	char buffer[ 128 ];

	if ( !input || !*input ) return NULL;

	row_security_role_update = row_security_role_update_calloc();

	row_security_role_update->role_name = role_name;

	/* See ROW_SECURITY_ROLE_UPDATE_SELECT */
	/* ----------------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	row_security_role_update->folder_name = strdup( folder_name );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer )
		row_security_role_update->attribute_not_null =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer )
		row_security_role_update->no_override_boolean =
			( *buffer == 'y' );

	row_security_role_update->
		relation_one2m_without_omit_drillthru_recursive_list =
		    relation_one2m_without_omit_drillthru_recursive_list(
			(LIST *)0 /* one2m_list Pass in NULL */,
			row_security_role_update->folder_name
				/* one_folder_name */,
			folder_attribute_fetch_primary_key_list(
				row_security_role_update->
					folder_name )
				/* one_folder_primary_key_list */ );

/*
	row_security_role_update->relation_one2m_recursive_list =
		relation_one2m_without_omit_drillthru_list(
			row_security_role_update->
				relation_one2m_recursive_list );
*/

	return row_security_role_update;
}

ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list_fetch(
		char *role_name,
		char *folder_name,
		boolean role_override_row_restrictions )
{
	ROW_SECURITY_ROLE_UPDATE_LIST *row_security_role_update_list;

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

	row_security_role_update_list = row_security_role_update_list_calloc();

	row_security_role_update_list->cache_list =
		/* --------------------- */
		/* Returns static LIST * */
		/* --------------------- */
		row_security_role_update_list_cache_list(
			role_name );

	if ( !list_length( row_security_role_update_list->cache_list ) )
	{
		free( row_security_role_update_list );
		return NULL;
	}

	row_security_role_update_list->
		row_security_role_update =
			row_security_role_update_seek(
				folder_name,
				row_security_role_update_list->cache_list );

	if ( row_security_role_update_list->row_security_role_update )
	{
		if (	role_override_row_restrictions
		&&	!row_security_role_update_list->
				row_security_role_update->
				no_override_boolean )
		{
			free( row_security_role_update_list );

			return NULL;
		}

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
				row_security_role_update_list->cache_list );

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

	if (	role_override_row_restrictions
	&&	!row_security_role_update_list->
			row_security_role_update_relation->
			no_override_boolean )
	{
		list_free( row_security_role_update_list->cache_list );
		free( row_security_role_update_list );

		return NULL;
	}

	row_security_role_update_list->attribute_not_null =
		row_security_role_update_list->
			row_security_role_update_relation->
			attribute_not_null;

	row_security_role_update_list->from =
		row_security_role_update_list->
			row_security_role_update_relation->
			relation_one2m->one_folder_name;

	row_security_role_update_list->join_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		row_security_role_update_list_join_where(
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

char *row_security_role_update_list_join_where(
		char *folder_name,
		char *one_folder_name,
		LIST *foreign_key_list )
{
	char where[ 1024 ];
	char *ptr = where;
	char *foreign_key;
	char table_name[ 128 ];
	char *one_table_name;

	if ( !folder_name
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
		appaserver_table_name(
			folder_name ) );

	one_table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_table_name(
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

	if ( list_rewind( list ) )
	do {
		row_security_role_update = list_get( list );

		if ( strcmp(
			folder_name,
			row_security_role_update->folder_name ) == 0 )
		{
			return row_security_role_update;
		}

	} while ( list_next( list ) );

	return NULL;
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

		if ( ( row_security_role_update->relation_one2m =
			row_security_role_update_relation_one2m(
			  row_security_role_update->
			   relation_one2m_without_omit_drillthru_recursive_list,
				folder_name ) ) )
		{
			return row_security_role_update;
		}

	} while ( list_next( list ) );

	return NULL;
}

RELATION_ONE2M *row_security_role_update_relation_one2m(
		LIST *relation_one2m_without_omit_drillthru_recursive_list,
		char *folder_name )
{
	RELATION_ONE2M *relation_one2m;

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

	if ( list_rewind(
		relation_one2m_without_omit_drillthru_recursive_list ) )
	do {
		relation_one2m =
			list_get(
			 relation_one2m_without_omit_drillthru_recursive_list );

		if ( !relation_one2m->many_folder_name
		||   !relation_one2m->relation )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_one2m is incomplete." );

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
			return relation_one2m;
		}

	} while ( list_next(
		    relation_one2m_without_omit_drillthru_recursive_list ) );

	return NULL;
}

LIST *row_security_role_update_list_cache_list( char *role_name )
{
	char *in_clause;
	static LIST *list;
	FILE *input_pipe;
	char input[ 256 ];

	if ( list ) return list;

	in_clause =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		role_folder_lookup_in_clause(
			ROLE_FOLDER_TABLE,
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE,
			FOLDER_PRIMARY_KEY,
			role_name );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_system_string(
				ROW_SECURITY_ROLE_UPDATE_SELECT,
				ROW_SECURITY_ROLE_UPDATE_TABLE,
				in_clause ) );

	list = list_new();

	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		list_set(
			list,
			row_security_role_update_parse(
				role_name,
				input ) );
	}

	pclose( input_pipe );

	return list;
}

