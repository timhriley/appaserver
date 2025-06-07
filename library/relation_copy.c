/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/relation_copy.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "folder.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "process.h"
#include "widget.h"
#include "sql.h"
#include "role.h"
#include "piece.h"
#include "query.h"
#include "relation_copy.h"

RELATION_COPY *relation_copy_new(
		DICTIONARY *dictionary /* in/out */,
		LIST *folder_attribute_list,
		LIST *relation_mto1_list,
		int row_number )
{
	RELATION_COPY *relation_copy;
	RELATION_MTO1 *relation_mto1;

	if ( !dictionary
	||   !list_length( folder_attribute_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	relation_copy = relation_copy_calloc();

	if ( list_rewind( relation_mto1_list ) )
	do {
		relation_mto1 =
			list_get(
				relation_mto1_list );

		if ( relation_mto1->
			relation->
			copy_common_columns )
		{
			if ( !relation_copy->
				folder_attribute_non_primary_name_list )
			{
				relation_copy->
				    folder_attribute_non_primary_name_list =
					folder_attribute_non_primary_name_list(
						folder_attribute_list );
			}

			relation_copy_set(
				dictionary /* in/out */,
				relation_copy->
					folder_attribute_non_primary_name_list,
				relation_mto1,
				row_number );
		}

	} while ( list_next( relation_mto1_list ) );

	return relation_copy;
}

RELATION_COPY *relation_copy_calloc( void )
{
	RELATION_COPY *relation_copy;

	if ( ! ( relation_copy = calloc( 1, sizeof ( RELATION_COPY ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return relation_copy;
}

void relation_copy_set(
		DICTIONARY *dictionary /* in/out */,
		LIST *folder_attribute_non_primary_name_list,
		RELATION_MTO1 *relation_mto1,
		int row_number )
{
	LIST *common_name_list;
	LIST *where_query_cell_list;
	char *where_string;
	char *system_string;
	QUERY_FETCH *query_fetch;
	LIST *result_query_cell_list;
	QUERY_CELL *query_cell;

	if ( !dictionary
	||   !relation_mto1
	||   !relation_mto1->one_folder
	||   !list_length(
		relation_mto1->
			one_folder->
				folder_attribute_name_list )
	||   !list_length( relation_mto1->relation_foreign_key_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	common_name_list =
		relation_mto1_common_name_list(
			folder_attribute_non_primary_name_list,
			1 /* copy_common_columns */,
			relation_mto1->
				one_folder->
				folder_attribute_name_list );

	if ( !list_length( common_name_list ) ) return;

	where_query_cell_list =
		relation_copy_where_query_cell_list(
			dictionary,
			relation_mto1->relation_foreign_key_list,
			relation_mto1->
				one_folder->
				folder_attribute_primary_key_list,
			row_number );

	if ( !where_query_cell_list ) return;

	if ( ! ( where_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_cell_where_string(
				where_query_cell_list ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_cell_where_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		relation_copy_system_string(
			SQL_DELIMITER,
			common_name_list /* select_name_list */,
			relation_mto1->one_folder_name,
			where_string );

	query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			relation_mto1->one_folder_name,
			(LIST *)0 /* folder_attribute_list */,
			date_convert_blank,
			common_name_list
				/* query_select_name_list */,
			system_string
				/* query_system_string */,
			0 /* not input_save_boolean */ );

	result_query_cell_list =
		relation_copy_result_query_cell_list(
			query_fetch->row_list );

	if ( list_rewind( result_query_cell_list ) )
	do {
		query_cell =
			list_get(
				result_query_cell_list );

		dictionary_set(
			dictionary,
			/* ------------------------------------------------ */
			/* Returns query_cell_attribute_name or heap memory */
			/* ------------------------------------------------ */
			relation_copy_attribute_name(
				query_cell->attribute_name,
				row_number ),
			query_cell->select_datum );

	} while ( list_next( result_query_cell_list ) );
}

LIST *relation_copy_where_query_cell_list(
		DICTIONARY *dictionary,
		LIST *relation_foreign_key_list,
		LIST *folder_attribute_primary_key_list,
		int row_number )
{
	LIST *query_cell_list = list_new();
	char *primary_key;
	char *foreign_key;
	char *get;
	QUERY_CELL *query_cell;

	list_rewind( relation_foreign_key_list );

	if ( list_rewind( folder_attribute_primary_key_list ) )
	do {
		primary_key =
			list_get(
				folder_attribute_primary_key_list );

		foreign_key = list_get( relation_foreign_key_list );

		if ( ! ( get =
				dictionary_index_get(
					foreign_key,
					dictionary,
					row_number /* index */ ) ) )
		{
			list_free( query_cell_list );
			return NULL;
		}

		query_cell =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_cell_simple_new(
				primary_key /* attribute_name */,
				get /* select_datum */ );

			list_set( query_cell_list, query_cell );
			list_next( relation_foreign_key_list );

	} while ( list_next( folder_attribute_primary_key_list ) );

	return query_cell_list;
}

char *relation_copy_system_string(
		const char sql_delimiter,
		LIST *select_name_list,
		char *folder_name,
		char *where_string )
{
	char system_string[ 1024 ];
	char *display_delimited;

	if ( !list_length( select_name_list )
	||   !folder_name
	||   !where_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	display_delimited =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_display_delimited(
			select_name_list,
			(char)sql_delimiter ),

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh %s %s \"%s\"",
		display_delimited,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_table_name(
			folder_name ),
		where_string );

	free( display_delimited );

	return strdup( system_string );
}

LIST *relation_copy_result_query_cell_list( LIST *query_fetch_row_list )
{
	QUERY_ROW *query_row;

	if ( list_length( query_fetch_row_list ) != 1 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"expecting list length of one; got=%d.",
			list_length( query_fetch_row_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_row = list_first( query_fetch_row_list );

	return query_row->cell_list;
}

char *relation_copy_attribute_name(
		char *query_cell_attribute_name,
		int row_number )
{
	if ( !query_cell_attribute_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !row_number ) return query_cell_attribute_name;

	return
	strdup(
		/* --------------------------------------- */
		/* Returns attribute_name or static memory */
		/* --------------------------------------- */
		dictionary_attribute_name_append_row_number(
			query_cell_attribute_name,
			row_number  ) );
}
