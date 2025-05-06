/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_join.c	 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "security.h"
#include "sql.h"
#include "piece.h"
#include "folder.h"
#include "query.h"
#include "relation_one2m.h"
#include "relation_join.h"

RELATION_JOIN *relation_join_calloc( void )
{
	RELATION_JOIN *relation_join;

	if ( ! ( relation_join = calloc( 1, sizeof ( RELATION_JOIN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join;
}

RELATION_JOIN *relation_join_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_one2m_join_list,
		LIST *query_fetch_row_list )
{
	RELATION_JOIN *relation_join;
	QUERY_ROW *query_row;
	RELATION_JOIN_ROW *relation_join_row;

	if ( !list_length( one_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( relation_one2m_join_list ) ) return NULL;
	if ( !list_rewind( query_fetch_row_list ) ) return NULL;

	relation_join = relation_join_calloc();
	relation_join->relation_join_row_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		if ( !list_length( query_row->cell_list ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_row->cell_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		relation_join_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			relation_join_row_new(
				one_folder_primary_key_list,
				relation_one2m_join_list,
				query_row->cell_list );

		list_set(
			relation_join->relation_join_row_list,
			relation_join_row );

	} while ( list_next( query_fetch_row_list ) );

	return relation_join;
}

RELATION_JOIN_FOLDER *relation_join_folder_calloc( void )
{
	RELATION_JOIN_FOLDER *relation_join_folder;

	if ( ! ( relation_join_folder =
			calloc( 1, sizeof ( RELATION_JOIN_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join_folder;
}

RELATION_JOIN_ROW *relation_join_row_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_one2m_join_list,
		LIST *query_row_cell_list )
{
	RELATION_JOIN_ROW *relation_join_row;
	RELATION_ONE2M *relation_one2m_join;
	RELATION_JOIN_FOLDER *relation_join_folder;

	if ( !list_length( one_folder_primary_key_list )
	||   !list_rewind( relation_one2m_join_list )
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_join_row = relation_join_row_calloc();
	relation_join_row->relation_join_folder_list = list_new();

	do {
		relation_one2m_join =
			list_get(
				relation_one2m_join_list );

		relation_join_folder =
			relation_join_folder_new(
				one_folder_primary_key_list,
				relation_one2m_join,
				query_row_cell_list );

		if ( relation_join_folder )
		{
			list_set(
				relation_join_row->relation_join_folder_list,
				relation_join_folder );
		}

	} while ( list_next( relation_one2m_join_list ) );

	return relation_join_row;
}

RELATION_JOIN_ROW *relation_join_row_calloc( void )
{
	RELATION_JOIN_ROW *relation_join_row;

	if ( ! ( relation_join_row =
			calloc( 1,
				sizeof ( RELATION_JOIN_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_join_row;
}

RELATION_JOIN_FOLDER *relation_join_folder_new(
		LIST *one_folder_primary_key_list,
		RELATION_ONE2M *relation_one2m_join,
		LIST *query_row_cell_list )
{
	RELATION_JOIN_FOLDER *relation_join_folder;

	if ( !list_length( one_folder_primary_key_list )
	||   !relation_one2m_join
	||   !relation_one2m_join->many_folder
	||   !list_length(
		relation_one2m_join->
		many_folder->
		folder_attribute_primary_key_list )
	||   !list_length(
		relation_one2m_join->
		relation_foreign_key_list )
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_join_folder = relation_join_folder_calloc();
	relation_join_folder->relation_one2m_join = relation_one2m_join;

	relation_join_folder->select_name_list =
		relation_join_folder_select_name_list(
			relation_one2m_join->
				many_folder->
				folder_attribute_primary_key_list,
			relation_one2m_join->
				relation_foreign_key_list );

	if ( !list_length( relation_join_folder->select_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"relation_join_folder_select_name_list(%s) returned empty.",
			relation_one2m_join->many_folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_join_folder->select_name_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_join_folder_select_name_list_string(
			relation_join_folder->select_name_list );


	relation_join_folder->relation_join_folder_where =
		relation_join_folder_where_new(
			one_folder_primary_key_list,
			relation_one2m_join->
				relation_foreign_key_list,
			query_row_cell_list );

	if ( !relation_join_folder->relation_join_folder_where )
	{
		free( relation_join_folder );
		return NULL;
	}

	relation_join_folder->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_join_folder_system_string(
			relation_join_folder->select_name_list_string,
			relation_one2m_join->many_folder_name,
			relation_join_folder->
				relation_join_folder_where->
				string );

	relation_join_folder->delimited_list =
		relation_join_folder_delimited_list(
			relation_join_folder->system_string );

	return relation_join_folder;
}

RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_calloc( void )
{
	RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where;

	if ( ! ( relation_join_folder_where =
			calloc( 1, sizeof ( RELATION_JOIN_FOLDER_WHERE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return relation_join_folder_where;
}

char *relation_join_folder_system_string(
		char *select_name_list_string,
		char *many_folder_name,
		char *where_string )
{
	char system_string[ 1024 ];

	if ( !select_name_list_string
	||   !many_folder_name
	||   !where_string  )
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
		"select.sh \"%s\" %s \"%s\" select",
		select_name_list_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_table_name( many_folder_name ),
		where_string );

	return strdup( system_string );
}

LIST *relation_join_folder_delimited_list( char *system_string )
{
	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	list_pipe_fetch( system_string );
}

RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where_new(
		LIST *one_folder_primary_key_list,
		LIST *relation_foreign_key_list,
		LIST *query_row_cell_list )
{
	RELATION_JOIN_FOLDER_WHERE *relation_join_folder_where;

	if ( !list_length( one_folder_primary_key_list )
	||   !list_length( relation_foreign_key_list )
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_join_folder_where = relation_join_folder_where_calloc();

	relation_join_folder_where->query_cell_attribute_data_list =
		query_cell_attribute_data_list(
			one_folder_primary_key_list
				/* attribute_name_list */,
			query_row_cell_list );

	if ( !list_length(
		relation_join_folder_where->
			query_cell_attribute_data_list ) )
	{
		return NULL;
	}

	relation_join_folder_where->string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		relation_join_folder_where_string(
			relation_foreign_key_list,
			relation_join_folder_where->
				query_cell_attribute_data_list );

	if ( !relation_join_folder_where->string )
	{
		char message[ 256 ];

		sprintf(message,
		"relation_join_folder_where_string([%s]) returned empty.",
			list_display(
				relation_join_folder_where->
					query_cell_attribute_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_join_folder_where;
}

char *relation_join_folder_where_string(
		LIST *relation_foreign_key_list,
		LIST *query_cell_attribute_data_list )
{
	char where_string[ 1024 ];
	char *ptr = where_string;
	char *attribute_datum;
	char *sql_injection_escape;

	if ( !list_rewind( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "relation_foreign_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind(
		query_cell_attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "query_cell_attribute_data_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		if ( ptr != where_string ) ptr += sprintf( ptr, " and " );

		attribute_datum =
			list_get(
				query_cell_attribute_data_list );

		sql_injection_escape =
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				attribute_datum /* datum */ );

		ptr += sprintf(
			ptr,
			"%s = '%s'",
			(char *)list_get( relation_foreign_key_list ),
			sql_injection_escape );

		list_next( relation_foreign_key_list );

	} while ( list_next( query_cell_attribute_data_list ) );

	return strdup( where_string );
}

LIST *relation_join_folder_select_name_list(
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list )
{
	return
	list_subtract(
		many_folder_primary_key_list,
		relation_foreign_key_list );
}

char *relation_join_folder_select_name_list_string(
		LIST *select_name_list )
{
	if ( !list_length(
		select_name_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"select_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited(
		select_name_list,
		',' );
}

LIST *relation_join_row_folder_name_list(
		RELATION_JOIN_ROW *relation_join_row )
{
	static LIST *folder_name_list = {0};
	RELATION_JOIN_FOLDER *relation_join_folder;

	if ( !relation_join_row ) return NULL;

	if ( folder_name_list ) return folder_name_list;

	folder_name_list = list_new();

	if ( !list_rewind( relation_join_row->relation_join_folder_list ) )
		return NULL;

	do {
		relation_join_folder =
			list_get(
				relation_join_row->
					relation_join_folder_list );

		if ( !relation_join_folder->relation_one2m_join )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_join_folder->relation_one2m_join is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			folder_name_list,
			relation_join_folder->
				relation_one2m_join->
				many_folder_name );

	} while ( list_next( relation_join_row->relation_join_folder_list ) );

	return folder_name_list;
}

RELATION_JOIN_FOLDER *relation_join_folder_seek(
		char *folder_name,
		LIST *relation_join_folder_list )
{
	RELATION_JOIN_FOLDER *relation_join_folder;

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

	if ( list_rewind( relation_join_folder_list ) )
	do {
		relation_join_folder =
			list_get(
				relation_join_folder_list );

		if ( strcmp(
			relation_join_folder->
				relation_one2m_join->
				many_folder_name,
			folder_name ) == 0 )
		{
			return relation_join_folder;
		}

	} while ( list_next( relation_join_folder_list ) );

	return NULL;
}
