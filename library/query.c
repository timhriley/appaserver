/* -------------------------------------------------------------------- */
/* $APPASERVER_LIBRARY/library/query.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "sql.h"
#include "String.h"
#include "column.h"
#include "folder.h"
#include "form.h"
#include "date.h"
#include "process.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "attribute.h"
#include "prompt_lookup.h"
#include "query.h"

char *query_system_string(
		char *application_name,
		char *select_string,
		char *from_string,
		char *where_string,
		char *group_by_string,
		char *order_string,
		int max_rows )
{
	char system_string[ STRING_SYSTEM_BUFFER ];

	if ( !select_string || !*select_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: select_string is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !from_string || !*from_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: from_string is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !where_string || !*where_string ) where_string = "1 = 1";

	if (	strlen( application_name ) +
		strlen( select_string ) +
		strlen( from_string ) +
		strlen( where_string ) +
		128 >= STRING_SYSTEM_BUFFER )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_SYSTEM_BUFFER );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.e	application=%s"
		"		select=\"%s\""
		"		folder=%s"
		"		where=\"%s\""
		"		group=\"%s\""
		"		order=\"%s\""
		"		maxrows=%d"
		"		quick=%s",
		application_name,
		select_string,
		from_string,
		where_string,
		(group_by_string) ? group_by_string : "",
		(order_string) ? order_string : "",
		max_rows,
		QUERY_QUICK_YES_NO );

	return strdup( system_string );
}

char *query_data_list_string(
		DICTIONARY *query_dictionary,
		char *query_key )
{
	return dictionary_fetch( query_key, query_dictionary );
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
		char *many_table_name,
		LIST *foreign_key_list,
		char *data_list_string,
		char multi_attribute_key_delimiter )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	char datum[ 1024 ];
	int p = 0;

	if ( !list_rewind( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !data_list_string
	||   !*data_list_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"data_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_row = query_drop_down_row_calloc();

	query_drop_down_row->data_list_string = data_list_string;
	query_drop_down_row->query_drop_down_datum_list = list_new();

	do {
		if ( !piece(	datum,
				multi_attribute_key_delimiter,
				data_list_string,
				p++ ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"piece(%s,%d) returned empty.\n",
				data_list_string,
				p - 1 );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !*datum )
		{
			char message[ 256 ];

			snprintf(
				message,
				sizeof ( message ),
				"*datum is empty. data_list_string=[%s]",
				data_list_string );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			query_drop_down_row->query_drop_down_datum_list,
			query_drop_down_datum_new(
				many_table_name,
				(char *)list_get( foreign_key_list )
					/* foreign_key */,
				datum ) );

	} while ( list_next( foreign_key_list ) );

	query_drop_down_row->query_drop_down_datum_list_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_drop_down_datum_list_where(
			query_drop_down_row->query_drop_down_datum_list,
			"and" /* conjunction */ );

	return query_drop_down_row;
}

QUERY_ATTRIBUTE *query_attribute_calloc( void )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( ! ( query_attribute = calloc( 1, sizeof ( QUERY_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_attribute;
}

QUERY_SELECT *query_select_calloc( void )
{
	QUERY_SELECT *query_select;

	if ( ! ( query_select = calloc( 1, sizeof ( QUERY_SELECT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_select;
}

QUERY_SELECT *query_select_new(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null,
		char *folder_name,
		char *attribute_name )
{
	QUERY_SELECT *query_select;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_select = query_select_calloc();

	query_select->attribute_name = attribute_name;

	query_select->full_attribute_name =
		/* ------------------------------------- */
		/* Returns heap memory or attribute_name */
		/* ------------------------------------- */
		query_select_full_attribute_name(
			application_name,
			relation_mto1_isa_list_length,
			attribute_not_null,
			folder_name,
			attribute_name );

	return query_select;
}

char *query_select_full_attribute_name(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null,
		char *folder_name,
		char *attribute_name )
{
	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------------------- */
	/* Returns heap memory or attribute_name */
	/* ------------------------------------- */
	attribute_full_attribute_name(
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_table_name(
			application_name,
			folder_name,
			relation_mto1_isa_list_length,
			attribute_not_null ),
		attribute_name );
}

LIST *query_select_table_edit_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		int relation_mto1_isa_list_length,
		char *attribute_not_null )
{
	LIST *query_select_list;
	FOLDER_ATTRIBUTE *folder_attribute;
	boolean skip_boolean;
	boolean need_but_not_get_boolean = 1;

	if ( !application_name
	||   !folder_name
	||   !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_select_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		skip_boolean =
			query_select_skip_boolean(
				folder_name,
				folder_attribute->folder_name,
				folder_attribute->attribute_name,
				no_display_name_list,
				exclude_lookup_attribute_name_list,
				folder_attribute->primary_key_index,
				attribute_not_null );

		if ( skip_boolean ) continue;

		query_select_need_but_not_get_boolean(
			&need_but_not_get_boolean,
			attribute_not_null,
			folder_attribute->attribute_name );

		list_set(
			query_select_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				attribute_not_null,
				folder_attribute->folder_name,
				folder_attribute->attribute_name ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( need_but_not_get_boolean )
	{
		list_set(
			query_select_list,
			query_select_new(
				(char *)0 /* application_name */,
				0 /* relation_mto1_is_list_length */,
				(char *)0 /* attribute_not_null */,
				(char *)0 /* folder_name */,
				attribute_not_null ) );
	}

	return query_select_list;
}

LIST *query_select_list(
		LIST *folder_attribute_primary_list )
{
	LIST *list = list_new();
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( list_rewind( folder_attribute_primary_list ) )
	do {
		folder_attribute = list_get( folder_attribute_primary_list );

		list_set(
			list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				(char *)0 /* application_name */,
				0 /* relation_mto1_isa_list_length */,
				(char *)0 /* attribute_not_null */,
				(char *)0 /* folder_name */,
				folder_attribute->attribute_name ) );

	} while ( list_next( folder_attribute_primary_list ) );

	return list;
}

char *query_select_list_string(
		LIST *query_select_list,
		LIST *common_name_list )
{
	char list_string[ 2048 ];
	char *ptr = list_string;
	QUERY_SELECT *query_select;
	char *common_name;

	if ( list_rewind( query_select_list ) )
	do {
		query_select = list_get( query_select_list );

		if ( ptr != list_string ) ptr += sprintf( ptr, "," );

		if ( !query_select->full_attribute_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: full_attribute_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( list_length( common_name_list )
		&&   list_last_boolean( query_select_list ) )
		{
			ptr += sprintf(
				ptr,
				"concat(%s,' ['",
				query_select->full_attribute_name );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"%s",
				query_select->full_attribute_name );
		}

	} while ( list_next( query_select_list ) );

	if ( list_rewind( common_name_list ) )
	do {
		common_name = list_get( common_name_list );
		ptr += sprintf( ptr, ",%s", common_name );

		if ( list_last_boolean( common_name_list ) )
		{
			ptr += sprintf( ptr, ",']')" );
		}

	} while ( list_next( common_name_list ) );

	if ( ptr == list_string )
		return NULL;
	else
		return strdup( list_string );
}

LIST *query_select_name_list( LIST *query_select_list )
{
	LIST *name_list;
	QUERY_SELECT *query_select;

	if ( !list_rewind( query_select_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		query_select = list_get( query_select_list );

		if ( !query_select->attribute_name )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set( name_list, query_select->attribute_name );

	} while ( list_next( query_select_list ) );

	return name_list;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc( void )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof ( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		exit( 1 );
	}

	return query_drop_down_row;
}

char *query_multi_select_drop_down_where(
		LIST *query_drop_down_row_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;
	boolean first_time = 1;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( !list_rewind( query_drop_down_row_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_drop_down_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( query_drop_down_row_list ) > 1 )
		ptr += sprintf( ptr, "(" );

	do {
		query_drop_down_row =
			list_get(
				query_drop_down_row_list );

		if ( !query_drop_down_row->query_drop_down_datum_list_where )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: where is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if (	strlen( where ) +
			strlen( query_drop_down_row->
					query_drop_down_datum_list_where ) +
			7 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !first_time )
			ptr += sprintf( ptr, " or " );
		else
			first_time = 0;

		ptr +=
			sprintf(ptr,
				"%s",
				query_drop_down_row->
					query_drop_down_datum_list_where );

		free(
			query_drop_down_row->
				query_drop_down_datum_list_where );

	} while ( list_next( query_drop_down_row_list ) );

	if ( list_length( query_drop_down_row_list ) > 1 )
		ptr += sprintf( ptr, ")" );

	return strdup( where );
}

char *query_relation_character_string(
		enum query_relation query_relation_enum )
{
	if ( query_relation_enum == query_equal )
		return "=";
	else
	if ( query_relation_enum == query_not_equal )
		return "<>";
	else
	if ( query_relation_enum == query_greater_than )
		return ">";
	else
	if ( query_relation_enum == query_greater_than_equal_to )
		return ">=";
	else
	if ( query_relation_enum == query_less_than )
		return "<";
	else
	if ( query_relation_enum == query_less_than_equal_to )
		return "<=";
	else
	if ( query_relation_enum == query_begins )
		return QUERY_BEGINS;
	else
	if ( query_relation_enum == query_contains )
		return QUERY_CONTAINS;
	else
	if ( query_relation_enum == query_or )
		return QUERY_OR;
	else
	if ( query_relation_enum == query_not_contains )
		return QUERY_NOT_CONTAINS;
	else
	if ( query_relation_enum == query_between )
		return QUERY_BETWEEN;
	else
	if ( query_relation_enum == query_is_null )
		return QUERY_IS_NULL;
	else
	if ( query_relation_enum == query_not_null )
		return QUERY_NOT_NULL;

	fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid query_relation_enum = %d.\n",
		__FILE__,
		__FUNCTION__,
		__LINE__,
		query_relation_enum );
	exit( 1 );
}

QUERY_TABLE_EDIT *query_table_edit_calloc( void )
{
	QUERY_TABLE_EDIT *query_table_edit;

	if ( ! ( query_table_edit = calloc( 1, sizeof ( QUERY_TABLE_EDIT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_table_edit;
}

QUERY_TABLE_EDIT *query_table_edit_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *query_dictionary,
		DICTIONARY *sort_dictionary,
		/* ------------------------- */
		/* Null if not participating */
		/* ------------------------- */
		ROW_SECURITY_ROLE_UPDATE_LIST *
			row_security_role_update_list,
		int max_rows )
{
	QUERY_TABLE_EDIT *query_table_edit;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_table_edit = query_table_edit_calloc();

	query_table_edit->query_select_table_edit_list =
		query_select_table_edit_list(
			application_name,
			folder_name,
			folder_attribute_append_isa_list,
			no_display_name_list,
			exclude_lookup_attribute_name_list,
			list_length( relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */,
			(row_security_role_update_list)
				? row_security_role_update_list->
					attribute_not_null
				: (char *)0 );

	if ( !list_length( query_table_edit->query_select_table_edit_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_select_table_edit_list(%s) is empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_table_edit->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_table_edit->query_select_table_edit_list,
			(LIST *)0 /* common_name_list */ );

	query_table_edit->query_select_name_list =
		query_select_name_list(
			query_table_edit->query_select_table_edit_list );

	query_table_edit->query_from_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(row_security_role_update_list)
				? row_security_role_update_list->from
				: (char *)0 );

	query_table_edit->query_table_edit_where =
		/* ---------------------------- */
		/* Safely returns.		*/
		/* Note: frees each where.	*/
		/* ---------------------------- */
		query_table_edit_where_new(
			application_name,
			folder_name,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			security_entity_where,
			query_dictionary,
			(row_security_role_update_list)
				? row_security_role_update_list->where
				: (char *)0 );

	query_table_edit->query_order_string =
		/* ----------------------------------------------- */
		/* Returns query_select_list_string or heap memory */
		/* ----------------------------------------------- */
		query_order_string(
			folder_attribute_primary_key_list,
			folder_attribute_name_list,
			sort_dictionary );

	query_table_edit->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_table_edit->query_select_list_string,
			query_table_edit->query_from_string,
			query_table_edit->query_table_edit_where->string,
			(char *)0 /* group_by_string */,
			query_table_edit->query_order_string,
			max_rows );

	query_table_edit->destination_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	query_table_edit->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			folder_name,
			folder_attribute_append_isa_list,
			query_table_edit->destination_enum,
			query_table_edit->query_select_name_list,
			query_table_edit->query_system_string,
			0 /* not input_save_boolean */ );

	return query_table_edit;
}

char *query_order_string(
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		DICTIONARY *sort_dictionary )
{
	char *order_string = {0};

	if ( dictionary_length( sort_dictionary )
	&&   folder_attribute_name_list )
	{
		LIST *key_list =
			dictionary_key_list(
				sort_dictionary );

		order_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_key_list_order_string(
				ATTRIBUTE_MULTI_KEY_DELIMITER,
				FORM_SORT_ASCEND_LABEL,
				FORM_SORT_DESCEND_LABEL,
				folder_attribute_name_list,
				key_list
					/* sort_dictionary_key_list */ );
	}

	if ( !order_string )
	{
		order_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			list_delimited_string(
				folder_attribute_primary_key_list,
				',' /* delimiter */ );
	}

	return order_string;
}

char *query_key_list_order_string(
		const char attribute_multi_key_delimiter,
		const char *form_sort_ascend_label,
		const char *form_sort_descend_label,
		LIST *folder_attribute_name_list,
		LIST *sort_dictionary_key_list )
{
	char *first_key;
	char *key_order_string = {0};
	char *attribute_name_list_string;

	if ( list_length( sort_dictionary_key_list ) != 1 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Expecting list length of 1." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	first_key = list_first( sort_dictionary_key_list );

	string_strncmp(
	first_key,
	FORM_SORT_ASCEND_LABEL );

	if ( string_strncmp(
		first_key,
		(char *)form_sort_ascend_label ) == 0 )
	{
		attribute_name_list_string =
			first_key + strlen( form_sort_ascend_label );

		key_order_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_key_order_string(
				attribute_multi_key_delimiter,
				folder_attribute_name_list,
				attribute_name_list_string,
				0 /* not descend_boolean */ );
	}

	if ( string_strncmp(
		first_key,
		(char *)form_sort_descend_label ) == 0 )
	{
		attribute_name_list_string =
			first_key + strlen( form_sort_descend_label );

		key_order_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_key_order_string(
				attribute_multi_key_delimiter,
				folder_attribute_name_list,
				attribute_name_list_string,
				1 /* descend_boolean */ );
	}

	return key_order_string;
}

char *query_key_order_string(
		const char attribute_multi_key_delimiter,
		LIST *folder_attribute_name_list,
		char *attribute_name_list_string,
		boolean descend_boolean )
{
	char order_string[ 1024 ];
	char *ptr = order_string;
	int p;
	char attribute_name[ 128 ];

	if ( !attribute_name_list_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name_list_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	for (	p = 0;
		piece(	attribute_name,
			attribute_multi_key_delimiter,
			attribute_name_list_string,
			p );
		p++ )
	{
		if ( list_string_boolean(
			attribute_name,
			folder_attribute_name_list ) )
		{
			if ( ptr != order_string ) ptr += sprintf( ptr, "," );

			ptr += sprintf(
				ptr,
				"%s",
				attribute_name );

			if ( descend_boolean )
				ptr += sprintf( ptr, " desc" );
		}
	}

	if ( ptr == order_string )
		return NULL;
	else
		return strdup( order_string );
}

QUERY_TABLE_EDIT_WHERE *query_table_edit_where_calloc( void )
{
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;

	if ( ! ( query_table_edit_where =
			calloc( 1, sizeof ( QUERY_TABLE_EDIT_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_table_edit_where;
}

QUERY_TABLE_EDIT_WHERE *query_table_edit_where_new(
		char *application_name,
		char *folder_name,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		char *row_security_role_update_list_where )
{
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;

	if ( !application_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_table_edit_where = query_table_edit_where_calloc();

	query_table_edit_where->relation_mto1_isa_list_length =
		list_length(
			relation_mto1_isa_list );

	query_table_edit_where->query_table_name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_table_name(
			application_name,
			folder_name,
			query_table_edit_where->
				relation_mto1_isa_list_length,
			row_security_role_update_list_where
				/* attribute_not_null */ );

	query_table_edit_where->query_drop_down_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_list_new(
			relation_mto1_list
				/* relation_mto1_to_one_list */,
			query_dictionary /* dictionary */,
			query_table_edit_where->query_table_name
				/* many_table_name */ );

	query_table_edit_where->query_isa_drop_down_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_isa_drop_down_list_new(
			relation_mto1_isa_list,
			query_dictionary /* dictionary */ );

	query_table_edit_where->drop_down_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_table_edit_where_drop_down_where(
			query_table_edit_where->
				query_drop_down_list->
				query_drop_down_relation_where,
			query_table_edit_where->
				query_isa_drop_down_list->where );

	query_table_edit_where->query_attribute_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_attribute_list_new(
			application_name,
			folder_attribute_append_isa_list,
			query_dictionary /* dictionary */,
			list_length( relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */,
			row_security_role_update_list_where );

	query_table_edit_where->query_isa =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_isa_new(
			application_name,
			relation_mto1_isa_list );

	query_table_edit_where->string =
		/* ---------------------------- */
		/* Returns heap memory or null. */
		/* Note: frees each where.	*/
		/* ---------------------------- */
		query_table_edit_where_string(
			security_entity_where,
			query_table_edit_where->drop_down_where
				/* query_drop_down_relation_where */,
			query_table_edit_where->
				query_attribute_list->
				where,
			query_table_edit_where->
				query_isa->
				join_where,
			row_security_role_update_list_where );

	return query_table_edit_where;
}

char *query_table_edit_where_string(
		/* --------------------- */
		/* Static memory or null */
		/* --------------------- */
		char *security_entity_where,
		char *query_drop_down_relation_where,
		char *query_attribute_list_where,
		char *query_isa_join_where,
		char *row_security_role_update_list_where )
{
	char string[ STRING_WHERE_BUFFER ];
	char *ptr = string;

	*ptr = '\0';

	if ( query_drop_down_relation_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_relation_where );

		free( query_drop_down_relation_where );
	}

	if ( query_attribute_list_where )
	{
		if (	strlen( string ) +
			strlen( query_attribute_list_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );

		free( query_attribute_list_where );
	}

	if ( query_isa_join_where )
	{
		if (	strlen( string ) +
			strlen( query_isa_join_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_isa_join_where );

		free( query_isa_join_where );
	}

	if ( security_entity_where )
	{
		if (	strlen( string ) +
			strlen( security_entity_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	if ( row_security_role_update_list_where )
	{
		if (	strlen( string ) +
			strlen( row_security_role_update_list_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			row_security_role_update_list_where );

		free( row_security_role_update_list_where );
	}

	if ( ptr == string )
		return (char *)0;
	else
		return strdup( string );
}

QUERY_RELATION *query_relation_calloc( void )
{
	QUERY_RELATION *query_relation;

	if ( ! ( query_relation = calloc( 1, sizeof ( QUERY_RELATION ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_relation;
}

QUERY_RELATION *query_relation_new(
		DICTIONARY *dictionary,
		char *attribute_name,
		char *datatype_name )
{
	QUERY_RELATION *query_relation;
	char *key;
	char *yes_no_key;
	char *operator_extract;

	query_relation = query_relation_calloc();

	key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_relation_key(
			QUERY_RELATION_OPERATOR_PREFIX,
			attribute_name );

	yes_no_key =
		/* ------------------------------ */
		/* Returns program memory or null */
		/* ------------------------------ */
		query_relation_yes_no_key(
			attribute_name );

	if ( ! ( operator_extract =
			query_relation_operator_extract(
				dictionary,
				key,
				yes_no_key ) ) )
	{
		free( query_relation );
		return NULL;
	}

	query_relation->query_relation_enum =
		query_relation_enum(
			QUERY_EQUAL,
			QUERY_NOT_EQUAL,
			QUERY_LESS_THAN,
			QUERY_LESS_THAN_EQUAL_TO,
			QUERY_GREATER_THAN,
			QUERY_GREATER_THAN_EQUAL_TO,
			QUERY_BETWEEN,
			QUERY_BEGINS,
			QUERY_CONTAINS,
			QUERY_NOT_CONTAINS,
			QUERY_OR,
			QUERY_IS_NULL,
			QUERY_NOT_NULL,
			datatype_name,
			operator_extract );

	query_relation->character_string =
		query_relation_character_string(
			query_relation->query_relation_enum );

	return query_relation;
}

char *query_relation_key(
		char *query_relation_operator_prefix,
		char *attribute_name )
{
	static char key[ 128 ];

	if ( !query_relation_operator_prefix
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(key,
		"%s%s",
		query_relation_operator_prefix,
		attribute_name );

	return key;
}

char *query_relation_yes_no_key( char *attribute_name )
{
	if ( attribute_is_yes_no( attribute_name ) )
	{
		return QUERY_EQUAL;
	}
	else
	{
		return (char *)0;
	}
}

char *query_relation_operator_extract(
		DICTIONARY *dictionary,
		char *query_relation_key,
		char *query_relation_yes_no_key )
{
	char *extract = {0};

	if ( query_relation_key )
	{
		extract =
			dictionary_get(
				query_relation_key,
				dictionary );
	}
	else
	if ( query_relation_yes_no_key )
	{
		extract =
			dictionary_get(
				query_relation_yes_no_key,
				dictionary );
	}

	return extract;
}

enum query_relation query_relation_enum(
		char *appaserver_equal,
		char *appaserver_not_equal,
		char *appaserver_less_than,
		char *appaserver_less_than_equal_to,
		char *appaserver_greater_than,
		char *appaserver_greater_than_equal_to,
		char *appaserver_between,
		char *appaserver_begins,
		char *appaserver_contains,
		char *appaserver_not_contains,
		char *appaserver_or,
		char *appaserver_null,
		char *appaserver_not_null,
		char *datatype_name,
		char *operator_extract )
{
	enum query_relation query_relation = query_unknown;

	if ( !operator_extract )
	{
		char message[ 128 ];

		sprintf(message, "operator_extract is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( operator_extract, appaserver_equal ) == 0 )
		query_relation = query_equal;
	else
	if ( strcmp( operator_extract, appaserver_not_equal ) == 0 )
		query_relation = query_not_equal;
	else
	if ( strcmp( operator_extract, appaserver_less_than ) == 0 )
		query_relation = query_less_than;
	else
	if ( strcmp( operator_extract, appaserver_less_than_equal_to ) == 0 )
		query_relation = query_less_than_equal_to;
	else
	if ( strcmp( operator_extract, appaserver_greater_than ) == 0 )
		query_relation = query_greater_than;
	else
	if ( strcmp( operator_extract, appaserver_greater_than_equal_to ) == 0 )
		query_relation = query_greater_than_equal_to;
	else
	if ( strcmp( operator_extract, appaserver_between ) == 0 )
		query_relation = query_between;
	else
	if ( strcmp( operator_extract, appaserver_begins ) == 0 )
		query_relation = query_begins;
	else
	if ( strcmp( operator_extract, appaserver_contains ) == 0 )
		query_relation = query_contains;
	else
	if ( strcmp( operator_extract, appaserver_not_contains ) == 0 )
		query_relation = query_not_contains;
	else
	if ( strcmp( operator_extract, appaserver_or ) == 0 )
		query_relation = query_or;
	else
	if ( strcmp( operator_extract, appaserver_null ) == 0 )
		query_relation = query_is_null;
	else
	if ( strcmp( operator_extract, appaserver_not_null ) == 0 )
		query_relation = query_not_null;

	if ( query_relation == query_equal
	&&   attribute_is_date_time( datatype_name ) )
	{
		query_relation = query_begins;
	}

	return query_relation;
}

QUERY_ATTRIBUTE *query_attribute_new(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		char *datatype_name )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !attribute_name
	||   !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_attribute is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_attribute = query_attribute_calloc();

	query_attribute->attribute_name = attribute_name;

	query_attribute->attribute_is_yes_no =
		attribute_is_yes_no(
			attribute_name );

	if ( query_attribute->attribute_is_yes_no )
	{
		query_attribute->where =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_attribute_yes_no_where(
				dictionary,
				table_name,
				attribute_name );

		if ( !query_attribute->where )
		{
			free( query_attribute );
			return NULL;
		}
		else
			return query_attribute;
	}

	query_attribute->query_relation =
		query_relation_new(
			dictionary,
			attribute_name,
			datatype_name );

	if ( !query_attribute->query_relation )
	{
		free( query_attribute );
		return NULL;
	}

	query_attribute->attribute_is_number =
		attribute_is_number(
			datatype_name );

	query_attribute->attribute_is_date_time =
		attribute_is_date_time(
			datatype_name );

	if (	query_attribute->query_relation->query_relation_enum ==
		query_is_null )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_is_null_where(
				attribute_full_attribute_name(
					table_name,
					attribute_name ) );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_not_null )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_not_null_where(
				attribute_full_attribute_name(
					table_name,
					attribute_name ) );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_or )
	{
		query_attribute->where =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_attribute_or_where(
				dictionary,
				table_name,
				attribute_name,
				query_attribute->attribute_is_number );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_between )
	{
		query_attribute->where =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_attribute_between_where(
				dictionary,
				table_name,
				attribute_name,
				query_attribute->attribute_is_number,
				query_attribute->attribute_is_date_time,
				query_attribute->
					attribute_is_current_date_time );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_begins )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_begins_where(
				dictionary,
				table_name,
				attribute_name );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_contains )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_contains_where(
				dictionary,
				table_name,
				attribute_name );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_not_contains )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_not_contains_where(
				dictionary,
				table_name,
				attribute_name );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_equal )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_equal_where(
				dictionary,
				table_name,
				attribute_name,
				query_attribute->attribute_is_number );
	}
	else
	if (	query_attribute->query_relation->query_relation_enum ==
		query_not_equal )
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_not_equal_where(
				dictionary,
				table_name,
				attribute_name,
				query_attribute->attribute_is_number );
	}
	else
	{
		query_attribute->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_attribute_generic_where(
				dictionary,
				table_name,
				attribute_name,
				query_attribute->
					query_relation->
					character_string,
				query_attribute->attribute_is_number );
	}

	if ( !query_attribute->where )
	{
		free( query_attribute );
		query_attribute = NULL;
	}

	return query_attribute;
}

char *query_attribute_is_null_where(
		char *attribute_full_attribute_name )
{
	char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"%s is null",
		attribute_full_attribute_name );

	return strdup( where );
}

char *query_attribute_not_null_where(
		char *attribute_full_attribute_name )
{
	char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"%s is not null",
		attribute_full_attribute_name );

	return strdup( where );
}

char *query_attribute_begins_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name )
{
	char where[ 256 ];
	char *extract;

	if ( !dictionary
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			0 /* not attribute_is_number */ ) ) )
	{
		return (char *)0;
	}

	sprintf(where,
		"%s like '%s%c'",
		attribute_full_attribute_name(
			table_name,
			attribute_name ),
		extract,
		'%' );

	return strdup( where );
}

char *query_attribute_contains_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name )
{
	char where[ 1024 ];
	char *ptr = where;
	char *extract;
	char *full_attribute_name;

	if ( !dictionary
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			0 /* not attribute_is_number */ ) ) )
	{
		return (char *)0;
	}

	full_attribute_name =
		/* ------------------------------------- */
		/* Returns attribute_name or heap memory */
		/* ------------------------------------- */
		attribute_full_attribute_name(
			table_name,
			attribute_name );

	ptr += sprintf(
		ptr,
		"(%s like '%c%s%c' or ",
		full_attribute_name,
		'%',
		extract,
		'%' );

	ptr += sprintf(
		ptr,
		"%s sounds like '%s')",
		full_attribute_name,
		extract );

	return strdup( where );
}

char *query_attribute_not_contains_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name )
{
	char where[ 256 ];
	char *extract;

	if ( !dictionary
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			0 /* not attribute_is_number */ ) ) )
	{
		return (char *)0;
	}

	sprintf(where,
		"%s not like '%c%s%c'",
		attribute_full_attribute_name(
			table_name,
			attribute_name ),
		'%',
		extract,
		'%' );

	return strdup( where );
}

char *query_attribute_equal_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number )
{
	char where[ 256 ];
	char *extract;
	char *full_attribute_name;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (char *)0;

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number ) ) )
	{
		if ( ! ( extract =
				dictionary_get(
					attribute_name,
					dictionary ) ) )
		{
			return (char *)0;
		}
	}

	full_attribute_name =
		attribute_full_attribute_name(
			table_name,
			attribute_name );

	if ( attribute_is_number )
	{
		sprintf(where,
			"%s = %s",
			full_attribute_name,
			extract );
	}
	else
	{
		sprintf(where,
			"%s = '%s'",
			full_attribute_name,
			extract );
	}

	return strdup( where );
}

char *query_attribute_not_equal_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number )
{
	char where[ 256 ];
	char *extract;
	char *full_attribute_name;

	if ( !dictionary
	||   !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number ) ) )
	{
		return (char *)0;
	}

	full_attribute_name =
		attribute_full_attribute_name(
			table_name,
			attribute_name );

	if ( attribute_is_number )
	{
		sprintf(where,
			"(%s <> %s or %s is null)",
			full_attribute_name,
			extract,
			full_attribute_name );
	}
	else
	{
		sprintf(where,
			"(%s <> '%s' or %s is null)",
			full_attribute_name,
			extract,
			full_attribute_name );
	}

	return strdup( where );
}

char *query_attribute_generic_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		char *query_relation_character_string,
		boolean attribute_is_number )
{
	char where[ 256 ];
	char *extract;
	char *full_attribute_name;

	if ( !dictionary
	||   !attribute_name
	||   !query_relation_character_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( extract =
		   /* --------------------------------------- */
		   /* Returns component of dictionary or null */
		   /* --------------------------------------- */
		   query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number ) ) )
	{
		return (char *)0;
	}

	full_attribute_name =
		/* ------------------------------------- */
		/* Returns attribute_name or heap memory */
		/* ------------------------------------- */
		attribute_full_attribute_name(
			table_name,
			attribute_name );

	if ( attribute_is_number )
	{
		sprintf(where,
			"%s %s %s",
			full_attribute_name,
			query_relation_character_string,
			extract );
	}
	else
	{
		sprintf(where,
			"%s %s '%s'",
			full_attribute_name,
			query_relation_character_string,
			extract );
	}

	return strdup( where );
}

char *query_from_string(
		char *folder_name,
		LIST *relation_mto1_isa_list,
		char *row_security_role_update_list_from )
{
	char from_string[ 1024 ];
	char *ptr = from_string;

	if ( !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "%s", folder_name );

	if ( list_length( relation_mto1_isa_list ) )
	{
		ptr += sprintf(
			ptr,
			",%s",
			list_display_delimited(
				relation_mto1_folder_name_list(
					relation_mto1_isa_list ),
				',' ) );
	}

	if ( row_security_role_update_list_from )
	{
		ptr += sprintf(
			ptr,
			",%s",
			row_security_role_update_list_from );
	}

	return strdup( from_string );
}

char *query_extract_key(
		char *attribute_name,
		char *prompt_lookup_from_prefix,
		char *prompt_lookup_to_prefix )
{
	static char key[ 128 ];

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !prompt_lookup_from_prefix
	&&   !prompt_lookup_to_prefix )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: both from and to prefixes are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( prompt_lookup_from_prefix )
	{
		snprintf(
			key,
			sizeof ( key ),
			"%s%s",
			prompt_lookup_from_prefix,
			attribute_name );
	}
	else
	{
		snprintf(
			key,
			sizeof ( key ),
			"%s%s",
			prompt_lookup_to_prefix,
			attribute_name );
	}

	return key;
}

char *query_dictionary_extract(
		DICTIONARY *dictionary,
		char *attribute_name,
		/* ------------------ */
		/* Mutually exclusive */
		/* ------------------ */
		char *prompt_lookup_from_prefix,
		/* ------------------ */
		/* Mutually exclusive */
		/* ------------------ */
		char *prompt_lookup_to_prefix,
		boolean attribute_is_number )
{
	char *get;

	if ( !dictionary ) return (char *)0;

	if ( !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	get =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			attribute_name,
			dictionary );

	if ( get )
	{
		if ( attribute_is_number )
		{
			/* --------------- */
			/* Returns number. */
			/* Trims , and $   */
			/* --------------- */
			attribute_trim_number_characters(
				get /* number */ );
		}

		return get;
	}

	get =
		dictionary_get(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_extract_key(
				attribute_name,
				prompt_lookup_from_prefix,
				prompt_lookup_to_prefix ),
			dictionary );

	if ( get )
	{
		if ( attribute_is_number )
		{
			/* --------------- */
			/* Returns number. */
			/* Trims , and $   */
			/* --------------- */
			attribute_trim_number_characters(
				get /* number */ );
		}
	}

	return get;
}

QUERY_ATTRIBUTE_LIST *query_attribute_list_new(
		char *application_name,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *dictionary,
		int relation_mto1_isa_list_length,
		char *row_security_role_update_list_where )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *table_name;

	QUERY_ATTRIBUTE_LIST *
		query_attribute_list =
			 query_attribute_list_calloc();

	if ( !dictionary_length( dictionary )
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		return query_attribute_list;
	}

	folder_attribute =
		list_first(
			folder_attribute_append_isa_list );

	table_name =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		query_attribute_list_table_name(
			application_name,
			relation_mto1_isa_list_length,
			row_security_role_update_list_where,
			folder_attribute->folder_name );

	query_attribute_list->query_date_time_between_list =
		query_date_time_between_list(
			folder_attribute_append_isa_list,
			dictionary,
			table_name );

	query_attribute_list->list = list_new();

	list_rewind( folder_attribute_append_isa_list );

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( query_date_time_between_exists(
			folder_attribute->attribute_name,
			query_attribute_list->query_date_time_between_list ) )
		{
			continue;
		}

		table_name =
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			query_attribute_list_table_name(
				application_name,
				relation_mto1_isa_list_length,
				row_security_role_update_list_where,
				folder_attribute->folder_name );

		list_set(
			query_attribute_list->list,
			query_attribute_new(
				dictionary,
				table_name,
				folder_attribute->attribute_name,
				folder_attribute->
					attribute->
					datatype_name ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	query_attribute_list->where =
		/* ------------------------------------ */
		/* Returns heap memory or null.		*/
		/* Note: it frees memory in lists.	*/
		/* ------------------------------------ */
		query_attribute_list_where(
			query_attribute_list->query_date_time_between_list,
			query_attribute_list->list );

	return query_attribute_list;
}

QUERY_ATTRIBUTE_LIST *query_attribute_list_calloc( void )
{
	QUERY_ATTRIBUTE_LIST *query_attribute_list;

	if ( ! ( query_attribute_list =
			calloc( 1, sizeof ( QUERY_ATTRIBUTE_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_attribute_list;
}

char *query_attribute_list_where(
		LIST *query_date_time_between_list,
		LIST *query_attribute_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;

	if ( list_rewind( query_date_time_between_list ) )
	{
		QUERY_DATE_TIME_BETWEEN *query_date_time_between;

		do {
			query_date_time_between =
				list_get(
					query_date_time_between_list );

			if ( !query_date_time_between->where )
			{
				fprintf(stderr,
				"ERROR in %s/%s()/%d: where is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_date_time_between->where );

		} while ( list_next( query_date_time_between_list ) );
	}

	if ( list_rewind( query_attribute_list ) )
	{
		QUERY_ATTRIBUTE *query_attribute;

		do {
			query_attribute =
				list_get(
					query_attribute_list );

			if ( !query_attribute->where )
			{
				char message[ 128 ];

				sprintf(message,
					"query_attribute->where is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_attribute->where );

		} while ( list_next( query_attribute_list ) );
	}

	if ( ptr == where )
		return (char *)0;
	else
		return strdup( where );
}

char *query_date_time_between_date_attribute_name(
		LIST *attribute_trimmed_list,
		char *time_attribute_name )
{
	char *trim_datatype;
	char date_attribute_name[ 128 ];

	if ( ! ( trim_datatype =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			attribute_name_trim_datatype(
				time_attribute_name,
				0 /* not attribute_is_date */,
				1 /* attribute_is_time */ ) ) )
	{
		return (char *)0;
	}

	*date_attribute_name = '\0';

	if ( list_string_exists(
		trim_datatype,
		attribute_trimmed_list ) )
	{
		sprintf(date_attribute_name,
			"%s_date",
			trim_datatype );
	}

	free( trim_datatype );

	if ( *date_attribute_name )
		return strdup( date_attribute_name );
	else
		return (char *)0;
}

char *query_attribute_from_data(
		DICTIONARY *dictionary,
		char *attribute_name,
		boolean attribute_is_number )
{
	char *extract;

	extract =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number );

	if ( !extract ) return (char *)0;

	return extract;
}

char *query_attribute_to_data(
		DICTIONARY *dictionary,
		char *attribute_name,
		boolean attribute_is_number,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time )
{
	char *extract;
	char to_data[ 128 ];

	*to_data = '\0';

	extract =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		query_dictionary_extract(
			dictionary,
			attribute_name,
			(char *)0 /* PROMPT_LOOKUP_FROM_PREFIX */,
			PROMPT_LOOKUP_TO_PREFIX,
			attribute_is_number );

	if ( !extract ) return (char *)0;

	if ( ( attribute_is_date_time
	||     attribute_is_current_date_time )
	&&   !string_character_exists(
			extract,
			' ' ) )
	{
		sprintf(to_data,
			"%s 23:23:59",
			extract );
	}

	if ( *to_data )
		return strdup( to_data );
	else
		return extract;
}

char *query_attribute_between_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time )
{
	char where[ 1024 ];
	char *from_data;
	char *to_data;

	*where = '\0';

	from_data =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number );

	if ( !from_data ) return (char *)0;

	to_data =
		query_attribute_to_data(
			dictionary,
			attribute_name,
			attribute_is_number,
			attribute_is_date_time,
			attribute_is_current_date_time );

	if ( !to_data ) return (char *)0;

	if ( attribute_is_number )
	{
		sprintf(where,
			"%s between %s and %s",
			/* ------------------------------------- */
			/* Returns attribute_name or heap memory */
			/* ------------------------------------- */
			attribute_full_attribute_name(
				table_name,
				attribute_name ),
			from_data,
			to_data );
	}
	else
	{
		sprintf(where,
			"%s between '%s' and '%s'",
			/* ------------------------------------- */
			/* Returns attribute_name or heap memory */
			/* ------------------------------------- */
			attribute_full_attribute_name(
				table_name,
				attribute_name ),
			from_data,
			to_data );
	}

	return strdup( where );
}

char *query_attribute_or_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name,
		boolean attribute_is_number )
{
	char where[ STRING_64K ];
	char *ptr = where;
	char *from_data;
	char delimiter;
	char piece_buffer[ 1024 ];
	char expression[ 2048 ];
	int i;
	boolean first_time = 1;

	from_data =
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		query_dictionary_extract(
			dictionary,
			attribute_name,
			PROMPT_LOOKUP_FROM_PREFIX,
			(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
			attribute_is_number );

	if ( !from_data ) return (char *)0;

	delimiter = string_delimiter( from_data );

	ptr += sprintf( ptr, "(" );

	for(	i = 0;
		piece( piece_buffer, delimiter, from_data, i );
		i++ )
	{
		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " or " );

		if ( attribute_is_number )
		{
			sprintf(expression,
				"%s = %s",
				/* ------------------------------------- */
				/* Returns attribute_name or heap memory */
				/* ------------------------------------- */
				attribute_full_attribute_name(
					table_name,
					attribute_name ),
				piece_buffer );
		}
		else
		{
			sprintf(expression,
				"%s = '%s'",
				/* ------------------------------------- */
				/* Returns attribute_name or heap memory */
				/* ------------------------------------- */
				attribute_full_attribute_name(
					table_name,
					attribute_name ),
				piece_buffer );
		}

		if (	strlen( where ) +
			strlen( expression ) >=
			STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf( ptr, "%s", expression  );
	}

	ptr += sprintf( ptr, ")" );

	return strdup( where );
}

LIST *query_date_time_between_list(
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *dictionary,
		char *table_name )
{
	LIST *attribute_trimmed_list;
	LIST *list;
	FOLDER_ATTRIBUTE *folder_attribute;
	QUERY_RELATION *query_relation;
	char *date_attribute_name;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (LIST *)0;

	attribute_trimmed_list = list_new();
	list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute->attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		query_relation =
			query_relation_new(
				dictionary,
				folder_attribute->attribute_name,
				folder_attribute->
					attribute->
					datatype_name );

		if ( !query_relation
		||   query_relation->query_relation_enum != query_between )
		{
			continue;
		}

		if ( attribute_is_date(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			list_set(
				attribute_trimmed_list,
				attribute_name_trim_datatype(
					folder_attribute->attribute_name,
					1 /* attribute_is_date */,
					0 /* not attribute_is_time */ ) );

			continue;
		}

		if ( !attribute_is_time(
			folder_attribute->
				attribute->
				datatype_name ) )
		{
			continue;
		}

		date_attribute_name =
			query_date_time_between_date_attribute_name(
				attribute_trimmed_list,
				folder_attribute->attribute_name
					/* time_attribute_name */ );

		if ( date_attribute_name )
		{
			list_set(
				list,
				query_date_time_between_new(
					dictionary,
					table_name,
					date_attribute_name,
					folder_attribute->attribute_name
						/* time_attribute_name */ ) );
		}
	} while ( list_next( folder_attribute_append_isa_list ) );

	return list;
}

QUERY_DATE_TIME_BETWEEN *query_date_time_between_new(
		DICTIONARY *dictionary,
		char *table_name,
		char *date_attribute_name,
		char *time_attribute_name )
{
	QUERY_DATE_TIME_BETWEEN *query_date_time_between;

	if ( !dictionary
	||   !date_attribute_name
	||   !time_attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_date_time_between = query_date_time_between_calloc();

	if ( ! ( query_date_time_between->date_from_data =
			query_date_time_between_date_from_data(
				dictionary,
				date_attribute_name,
				PROMPT_LOOKUP_FROM_PREFIX ) ) )
	{
		free( query_date_time_between );
		return (QUERY_DATE_TIME_BETWEEN *)0;
	}

	if ( ! ( query_date_time_between->date_to_data =
			query_date_time_between_date_to_data(
				dictionary,
				date_attribute_name,
				PROMPT_LOOKUP_TO_PREFIX ) ) )
	{
		free( query_date_time_between->date_from_data );
		free( query_date_time_between );
		return (QUERY_DATE_TIME_BETWEEN *)0;
	}

	if ( ! ( query_date_time_between->time_from_data =
			query_date_time_between_time_from_data(
				dictionary,
				time_attribute_name,
				PROMPT_LOOKUP_FROM_PREFIX ) ) )
	{
		free( query_date_time_between->date_from_data );
		free( query_date_time_between->date_to_data );
		free( query_date_time_between );
		return (QUERY_DATE_TIME_BETWEEN *)0;
	}

	if ( ! ( query_date_time_between->time_to_data =
			query_date_time_between_time_to_data(
				dictionary,
				time_attribute_name,
				PROMPT_LOOKUP_TO_PREFIX ) ) )
	{
		free( query_date_time_between->time_from_data );
		free( query_date_time_between->date_from_data );
		free( query_date_time_between->date_to_data );
		free( query_date_time_between );
		return (QUERY_DATE_TIME_BETWEEN *)0;
	}

	query_date_time_between->date_attribute_name =
		date_attribute_name;

	query_date_time_between->time_attribute_name =
		time_attribute_name;

	query_date_time_between->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_date_time_between_where(
			table_name,
			query_date_time_between->date_attribute_name,
			query_date_time_between->time_attribute_name,
			query_date_time_between->date_from_data,
			query_date_time_between->date_to_data,
			query_date_time_between->time_from_data,
			query_date_time_between->time_to_data );

	return query_date_time_between;
}

QUERY_DATE_TIME_BETWEEN *query_date_time_between_calloc( void )
{
	QUERY_DATE_TIME_BETWEEN *query_date_time_between;

	if ( ! ( query_date_time_between =
			calloc( 1, sizeof ( QUERY_DATE_TIME_BETWEEN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_date_time_between;
}

char *query_date_time_between_where(
		char *table_name,
		char *date_attribute_name,
		char *time_attribute_name,
		char *date_from_data,
		char *date_to_data,
		char *time_from_data,
		char *time_to_data )
{
	char where_clause[ 1024 ];
	char *where_ptr = where_clause;
	char *start_end_time;
	char *finish_begin_time;

	if ( !date_attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !time_attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: time_attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( string_strcmp( date_from_data, date_to_data ) != 0 )
	{
		start_end_time = "2359";
		finish_begin_time = "0000";
	}
	else
	{
		start_end_time = time_to_data;
		finish_begin_time = time_from_data;
	}

	where_ptr +=
		sprintf(
		   where_ptr,
		  " ( ( %s = '%s' and %s >= '%s' and %s <= '%s' )",
			/* ------------------------------------- */
			/* Returns attribute_name or heap memory */
			/* ------------------------------------- */
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
		 	date_from_data,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ),
		 	time_from_data,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ),
			start_end_time );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
		 	date_from_data,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
		 	date_to_data,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s > '%s' and %s < '%s' )",
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
		 	date_from_data,
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
			date_to_data );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s >= '%s' and %s <= '%s' ) )",
			attribute_full_attribute_name(
				table_name,
				date_attribute_name ),
		 	date_to_data,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ),
			finish_begin_time,
			attribute_full_attribute_name(
				table_name,
				time_attribute_name ),
		 	time_to_data );

	return strdup( where_clause );
}

QUERY_DROP_DOWN_RELATION *query_drop_down_relation_new(
		DICTIONARY *dictionary,
		char *many_table_name /* optional */,
		int dictionary_highest_index,
		RELATION_MTO1 *relation_mto1 )
{
	QUERY_DROP_DOWN_RELATION *query_drop_down_relation;

	if ( !relation_mto1
	||   !list_length( relation_mto1->relation_foreign_key_list )
	||   !relation_mto1->relation )
	{
		char message[ 128 ];

		sprintf(message, "relation_mto1 is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_relation = query_drop_down_relation_calloc();

	query_drop_down_relation->many_folder_name =
		relation_mto1->many_folder_name;

	query_drop_down_relation->one_folder_name =
		relation_mto1->one_folder_name;

	query_drop_down_relation->dictionary_key_list_highest_index =
		dictionary_key_list_highest_index(
			ATTRIBUTE_MULTI_KEY_DELIMITER,
			relation_mto1->relation_foreign_key_list,
			dictionary,
			dictionary_highest_index );

	if ( query_drop_down_relation->dictionary_key_list_highest_index < 0 )
	{
		free( query_drop_down_relation );
		return NULL;
	}

	if ( query_is_null_drop_down_boolean(
		dictionary,
		relation_mto1->relation_foreign_key_list,
		query_drop_down_relation->dictionary_key_list_highest_index ) )
	{
		query_drop_down_relation->query_is_null_drop_down =
			query_is_null_drop_down_new(
			    many_table_name,
			    relation_mto1->
				relation_foreign_key_list );
	}
	else
	if ( query_not_null_drop_down_boolean(
		dictionary,
		relation_mto1->relation_foreign_key_list,
		query_drop_down_relation->dictionary_key_list_highest_index ) )
	{
		query_drop_down_relation->query_not_null_drop_down =
			query_not_null_drop_down_new(
				many_table_name,
				relation_mto1->
					relation_foreign_key_list );
	}
	else
	if ( query_drop_down_relation->dictionary_key_list_highest_index >= 1 )
	{
		query_drop_down_relation->query_multi_select_drop_down =
			query_multi_select_drop_down_new(
				relation_mto1->many_folder_name,
				relation_mto1->one_folder_name,
				many_table_name,
				dictionary,
				relation_mto1->
					relation_foreign_key_list,
				query_drop_down_relation->
					dictionary_key_list_highest_index );
	}
	else
	{
		query_drop_down_relation->query_single_select_drop_down =
			query_single_select_drop_down_new(
				many_table_name,
				dictionary,
				relation_mto1->
					relation_foreign_key_list );
	}

	if ( !query_drop_down_relation->
		query_is_null_drop_down
	&&   !query_drop_down_relation->
		query_not_null_drop_down
	&&   !query_drop_down_relation->
		query_multi_select_drop_down
	&&   !query_drop_down_relation->
		query_single_select_drop_down )
	{
		free( query_drop_down_relation );
		query_drop_down_relation = NULL;
	}

	return query_drop_down_relation;
}

QUERY_DROP_DOWN_RELATION *query_drop_down_relation_calloc( void )
{
	QUERY_DROP_DOWN_RELATION *query_drop_down_relation;

	if ( ! ( query_drop_down_relation =
			calloc( 1,
				sizeof ( QUERY_DROP_DOWN_RELATION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_drop_down_relation;
}

QUERY_DROP_DOWN_LIST *query_drop_down_list_new(
		LIST *relation_mto1_to_one_list,
		DICTIONARY *dictionary,
		char *many_table_name )
{
	RELATION_MTO1 *relation_mto1;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;
	QUERY_DROP_DOWN_RELATION *query_drop_down_relation;

	query_drop_down_list = query_drop_down_list_calloc();

	if ( !list_rewind( relation_mto1_to_one_list ) )
		return query_drop_down_list;

	query_drop_down_list->dictionary_highest_index =
		dictionary_highest_index(
			dictionary );

	query_drop_down_list->query_drop_down_relation_list = list_new();

	do {
		relation_mto1 =
			list_get(
				relation_mto1_to_one_list );

		if ( !relation_mto1->one_folder )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_mto1->one_folder is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_length( relation_mto1->relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
			"relation_mto1->relation_foreign_key_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( relation_mto1->relation->relation_type_isa ) continue;

		query_drop_down_relation =
			query_drop_down_relation_new(
				dictionary,
				many_table_name,
				query_drop_down_list->dictionary_highest_index,
				relation_mto1 );

		if ( query_drop_down_relation )
		{
			if ( !query_drop_down_relation->
				query_is_null_drop_down
			&&   !query_drop_down_relation->
				query_not_null_drop_down
			&&   !query_drop_down_relation->
				query_multi_select_drop_down
			&&   !query_drop_down_relation->
				query_single_select_drop_down )
			{
				char message[ 1024 ];

				sprintf(message,
"query_drop_down_relation_new(%s) returned incomplete.\n"
"Each of is_null, not_null, multi_select, and single_select are empty.",
					relation_mto1->one_folder_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			list_set(
				query_drop_down_list->
					query_drop_down_relation_list,
				query_drop_down_relation );
		}

	} while ( list_next( relation_mto1_to_one_list ) );

	query_drop_down_list->query_drop_down_relation_where =
		/* -----------------------------------  */
		/* Returns heap memory or null		*/
		/* Note: it frees char *where		*/
		/* ------------------------------------ */
		query_drop_down_relation_where(
			query_drop_down_list->
				query_drop_down_relation_list );

	query_drop_down_list->query_drop_down_relation_attribute_name_list =
		query_drop_down_relation_attribute_name_list(
			query_drop_down_list->
				query_drop_down_relation_list );

	return query_drop_down_list;
}

QUERY_DROP_DOWN_LIST *query_drop_down_list_calloc( void )
{
	QUERY_DROP_DOWN_LIST *query_drop_down_list;

	if ( ! ( query_drop_down_list =
		  calloc(
			1,
			sizeof ( QUERY_DROP_DOWN_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_drop_down_list;
}

char *query_drop_down_relation_where(
		LIST *query_drop_down_relation_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;
	QUERY_DROP_DOWN_RELATION *query_drop_down_relation;

	if ( list_rewind( query_drop_down_relation_list ) )
	do {
		query_drop_down_relation =
			list_get(
				query_drop_down_relation_list );

		if ( query_drop_down_relation->query_is_null_drop_down )
		{
			if ( !query_drop_down_relation->
				query_is_null_drop_down->
				where )
			{
				char message[ 128 ];

				sprintf(message, "where is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if (	strlen( where ) +
				strlen( query_drop_down_relation->
					query_is_null_drop_down->
					where ) +
				5 >= STRING_WHERE_BUFFER )
			{
				char message[ 128 ];

				sprintf(message,
					STRING_OVERFLOW_TEMPLATE,
					STRING_WHERE_BUFFER );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_drop_down_relation->
					query_is_null_drop_down->
					where );

			free( query_drop_down_relation->
				query_is_null_drop_down->
				where );
		}
		else
		if ( query_drop_down_relation->query_not_null_drop_down )
		{
			if ( !query_drop_down_relation->
				query_not_null_drop_down->
				where )
			{
				char message[ 128 ];

				sprintf(message, "where is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if (	strlen( where ) +
				strlen( query_drop_down_relation->
					query_not_null_drop_down->
					where ) +
				5 >= STRING_WHERE_BUFFER )
			{
				char message[ 128 ];

				sprintf(message,
					STRING_OVERFLOW_TEMPLATE,
					STRING_WHERE_BUFFER );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_drop_down_relation->
					query_not_null_drop_down->
					where );

			free( query_drop_down_relation->
				query_not_null_drop_down->
				where );
		}
		else
		if ( query_drop_down_relation->query_multi_select_drop_down )
		{
			if ( !query_drop_down_relation->
				query_multi_select_drop_down->
				query_multi_drop_down_column_where )
			{
				char message[ 128 ];

				sprintf(message,
			"query_multi_drop_down_column_where is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if (	strlen( where ) +
				strlen( query_drop_down_relation->
					query_multi_select_drop_down->
					query_multi_drop_down_column_where ) +
				5 >= STRING_WHERE_BUFFER )
			{
				char message[ 128 ];

				sprintf(message,
					STRING_OVERFLOW_TEMPLATE,
					STRING_WHERE_BUFFER );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_drop_down_relation->
					query_multi_select_drop_down->
					query_multi_drop_down_column_where );

			free( query_drop_down_relation->
				query_multi_select_drop_down->
				query_multi_drop_down_column_where );
		}
		else
		if ( query_drop_down_relation->query_single_select_drop_down )
		{
			if ( !query_drop_down_relation->
				query_single_select_drop_down->
				where )
			{
				char message[ 128 ];

				sprintf(message, "where is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if (	strlen( where ) +
				strlen( query_drop_down_relation->
					query_single_select_drop_down->
					where ) +
				5 >= STRING_WHERE_BUFFER )
			{
				char message[ 128 ];

				sprintf(message,
					STRING_OVERFLOW_TEMPLATE,
					STRING_WHERE_BUFFER );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( ptr != where ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
				ptr,
				"%s",
				query_drop_down_relation->
					query_single_select_drop_down->
					where );

			free( query_drop_down_relation->
				query_single_select_drop_down->
				where );
		}

	} while ( list_next( query_drop_down_relation_list ) );

	if ( ptr == where )
		return (char *)0;
	else
		return strdup( where );
}

char *query_table_name(
		char *application_name,
		char *folder_name,
		int relation_mto1_isa_list_length,
		char *attribute_not_null )
{
	if ( !relation_mto1_isa_list_length
	&&   !attribute_not_null )
	{
		return (char *)0;
	}
	else
	{
		return
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				folder_name ) );
	}
}

QUERY_CELL *query_cell_parse(
		LIST *folder_attribute_list,
		enum date_convert_format_enum destination_enum,
		char *attribute_name,
		char *select_datum )
{
	FOLDER_ATTRIBUTE *folder_attribute = {0};
	int primary_key_index = {0};
	boolean is_date = {0};
	boolean is_date_time = {0};
	boolean is_current_date_time = {0};
	boolean is_number = {0};
	char *display_datum = {0};
	QUERY_CELL *query_cell;

	if ( !select_datum )
	{
		char message[ 128 ];

		sprintf(message, "select_datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( folder_attribute_list ) )
	{
		if ( ( folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_list ) ) )
		{
			if ( !folder_attribute->attribute )
			{
				char message[ 256 ];

				sprintf(message,
	"for folder,attribute=[%s,%s], folder_attribute->attribute is empty.",
					folder_attribute->folder_name,
					attribute_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			primary_key_index =
				folder_attribute->
					primary_key_index;

			is_date =
				attribute_is_date(
					folder_attribute->
						attribute->
						datatype_name );

			is_date_time =
				attribute_is_date_time(
					folder_attribute->
						attribute->
						datatype_name );

			is_current_date_time =
				attribute_is_current_date_time(
					folder_attribute->
						attribute->
						datatype_name );

			is_number =
				attribute_is_number(
					folder_attribute->
						attribute->
					datatype_name );

			display_datum =
				/* ----------------------------------- */
				/* Returns heap memory or select_datum */
				/* ----------------------------------- */
				query_cell_display_datum(
					destination_enum,
					select_datum,
					is_date,
					is_date_time,
					is_current_date_time,
					is_number );
		}
	}

	query_cell =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_cell_new(
			attribute_name,
			select_datum,
			primary_key_index,
			is_date,
			is_date_time,
			is_current_date_time,
			is_number,
			display_datum );

	return query_cell;
}

QUERY_CELL *query_cell_calloc( void )
{
	QUERY_CELL *query_cell;

	if ( ! ( query_cell = calloc( 1, sizeof ( QUERY_CELL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_cell;
}

QUERY_CELL *query_cell_seek(
		char *attribute_name,
		LIST *cell_list )
{
	QUERY_CELL *query_cell;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( cell_list ) ) return NULL;

	do {
		query_cell = list_get( cell_list );

		if ( string_strcmp(
			attribute_name,
			query_cell->attribute_name ) == 0 )
		{
			return query_cell;
		}

	} while ( list_next( cell_list ) );

	return NULL;
}

char *query_cell_display_datum(
		enum date_convert_format_enum destination_enum,
		char *select_datum,
		boolean attribute_is_date,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time,
		boolean attribute_is_number )
{
	char *display_datum;

	if ( !select_datum )
	{
		char message[ 128 ];

		sprintf(message, "select_datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_is_date
	||   attribute_is_date_time
	||   attribute_is_current_date_time )
	{
		DATE_CONVERT *date_convert =
			date_convert_new(
				date_convert_international
					/* source_enum */,
				destination_enum,
				select_datum
					/* source_date_string */ );

		if ( !date_convert )
		{
			char message[ 128 ];

			sprintf(message,
				"date_convert_new(%s) returned empty.",
				select_datum );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		display_datum =
			date_convert->
			   /* ----------------------------------------------- */
			   /* Returns heap memory, source_date_string or null */
			   /* ----------------------------------------------- */
			   return_date_string;
	}
	else
	if ( attribute_is_number )
	{
		display_datum =
			strdup(
				/* --------------------------- */
				/* Returns static memory or "" */
				/* --------------------------- */
				string_commas_number_string(
					select_datum ) );
	}
	else
	{
		display_datum = select_datum;
	}

	return display_datum;
}

QUERY_ROW *query_row_parse(
		const char sql_delimiter,
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		boolean input_save_boolean,
		char *input /* stack memory */ )
{
	LIST *cell_list;
	char *attribute_name;
	char piece_buffer[ STRING_64K ];
	int p = 0;

	if ( !list_rewind( query_select_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "query_select_name_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !input || !*input ) return NULL;

	cell_list = list_new();

	do {
		attribute_name =
			list_get(
				query_select_name_list );

		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		if ( !piece(
			piece_buffer,
			sql_delimiter,
			input,
			p++ ) )
		{
			break;
		}

		list_set(
			cell_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_cell_parse(
				folder_attribute_append_isa_list,
				destination_enum,
				attribute_name,
				strdup( piece_buffer )
					/* select_datum */ ) );

	} while ( list_next( query_select_name_list ) );

	return
	query_row_new(
		input_save_boolean,
		input,
		cell_list );
}

QUERY_ROW *query_row_new(
		boolean input_save_boolean,
		char *input /* stack memory or null */,
		LIST *cell_list )
{
	QUERY_ROW *query_row;

	if ( !list_length( cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "cell_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_row = query_row_calloc();

	if ( input_save_boolean ) query_row->input = strdup( input );

	query_row->cell_list = cell_list;

	return query_row;
}

QUERY_ROW *query_row_calloc( void )
{
	QUERY_ROW *query_row;

	if ( ! ( query_row = calloc( 1, sizeof ( QUERY_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_row;
}

QUERY_FETCH *query_fetch_new(
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		char *query_system_string,
		boolean input_save_boolean )
{
	QUERY_FETCH *query_fetch;
	char input[ STRING_INPUT_BUFFER ];
	FILE *input_pipe;

	if ( !list_length( query_select_name_list )
	||   !query_system_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_fetch = query_fetch_calloc();

	query_fetch->folder_name = folder_name;
	query_fetch->query_select_name_list = query_select_name_list;
	query_fetch->row_list = list_new();

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_input_pipe(
			query_system_string );

	while( string_input( input, input_pipe, STRING_INPUT_BUFFER ) )
	{
		list_set(
			query_fetch->row_list,
			query_row_parse(
				SQL_DELIMITER,
				folder_attribute_append_isa_list,
				destination_enum,
				query_select_name_list,
				input_save_boolean,
				input /* stack memory */ ) );
	}

	pclose( input_pipe );

	return query_fetch;
}

QUERY_FETCH *query_fetch_calloc( void )
{
	QUERY_FETCH *query_fetch;

	if ( ! ( query_fetch = calloc( 1, sizeof ( QUERY_FETCH ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_fetch;
}

FILE *query_fetch_input_pipe( char *query_system_string )
{
	if ( !query_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_input_pipe( query_system_string );
}

QUERY_DROP_DOWN_DATUM *query_drop_down_datum_new(
		char *many_table_name,
		char *foreign_key,
		char *datum )
{
	QUERY_DROP_DOWN_DATUM *query_drop_down_datum;

	if ( !foreign_key )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"foreign_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !datum || !*datum )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_datum = query_drop_down_datum_calloc();

	query_drop_down_datum->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_drop_down_datum_where(
			many_table_name,
			foreign_key,
			datum );

	return query_drop_down_datum;
}

QUERY_DROP_DOWN_DATUM *query_drop_down_datum_calloc( void )
{
	QUERY_DROP_DOWN_DATUM *query_drop_down_datum;

	if ( ! ( query_drop_down_datum =
			calloc( 1,
				sizeof ( QUERY_DROP_DOWN_DATUM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_drop_down_datum;
}

char *query_drop_down_datum_where(
		char *many_table_name,
		char *foreign_key,
		char *datum )
{
	char where[ 2048 ];

	sprintf(where,
		"%s = '%s'",
		/* ------------------------------------- */
		/* Returns attribute_name or heap memory */
		/* ------------------------------------- */
		attribute_full_attribute_name(
			many_table_name,
			foreign_key /* attribute_name */ ),
		datum );

	return strdup( where );
}

char *query_drop_down_datum_list_where(
		LIST *datum_list,
		char *conjunction )
{
	QUERY_DROP_DOWN_DATUM *query_drop_down_datum;
	char where[ 2048 ];
	char *ptr = where;
	boolean first_time = 1;

	if ( !conjunction
	||   !list_rewind( datum_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "(" );

	do {
		query_drop_down_datum = list_get( datum_list );

		if ( !query_drop_down_datum->where )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: query_drop_down_datum->where is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " %s ", conjunction );
		}

		ptr +=
			sprintf(ptr,
				"%s",
				query_drop_down_datum->where );

		free( query_drop_down_datum->where );

	} while ( list_next( datum_list ) );

	ptr += sprintf( ptr, ")" );

	return strdup( where );
}

QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down_new(
		char *many_table_name,
		DICTIONARY *dictionary,
		LIST *foreign_key_list )
{
	QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down;
	char *key;

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_single_select_drop_down = query_single_select_drop_down_calloc();
	query_single_select_drop_down->foreign_key_list = foreign_key_list;

	key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_drop_down_key(
			foreign_key_list,
			0 /* index */,
			ATTRIBUTE_MULTI_KEY_DELIMITER );

	query_single_select_drop_down->
		data_list_string =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				dictionary );

	if ( query_single_select_drop_down->data_list_string
	&&   *query_single_select_drop_down->data_list_string )
	{
		query_single_select_drop_down->
			query_drop_down_row =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				query_drop_down_row_new(
					many_table_name,
					foreign_key_list,
					query_single_select_drop_down->
						data_list_string,
					ATTRIBUTE_MULTI_KEY_DELIMITER );
	}
	else
	{
		free( query_single_select_drop_down );
		return NULL;
	}

	query_single_select_drop_down->where =
		/* -------------------------------------------------- */
		/* Returns component of query_single_select_drop_down */
		/* -------------------------------------------------- */
		query_single_select_drop_down_where(
			query_single_select_drop_down );

	return query_single_select_drop_down;
}

QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down_calloc( void )
{
	QUERY_SINGLE_SELECT_DROP_DOWN *query_single_select_drop_down;

	if ( ! ( query_single_select_drop_down =
			calloc(
				1,
				sizeof ( QUERY_SINGLE_SELECT_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_single_select_drop_down;
}

char *query_single_select_drop_down_where(
		QUERY_SINGLE_SELECT_DROP_DOWN *
			query_single_select_drop_down )
{
	if ( !query_single_select_drop_down
	||   !query_single_select_drop_down->
		query_drop_down_row
	||   !query_single_select_drop_down->
		query_drop_down_row->
		query_drop_down_datum_list_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: where is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	query_single_select_drop_down->
		query_drop_down_row->
		query_drop_down_datum_list_where;
}

QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down_new(
		char *many_table_name,
		LIST *foreign_key_list )
{
	QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down;

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_is_null_drop_down = query_is_null_drop_down_calloc();
	query_is_null_drop_down->foreign_key_list = foreign_key_list;

	query_is_null_drop_down->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_is_null_drop_down_where(
			many_table_name,
			foreign_key_list );

	return query_is_null_drop_down;
}

QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down_calloc( void )
{
	QUERY_IS_NULL_DROP_DOWN *query_is_null_drop_down;

	if ( ! ( query_is_null_drop_down =
			calloc( 1, sizeof ( QUERY_IS_NULL_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_is_null_drop_down;
}

char *query_is_null_drop_down_where(
		char *many_table_name,
		LIST *foreign_key_list )
{
	char where[ 1024 ];
	char *ptr = where;

	if ( !list_rewind( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s is null",
			/* ------------------------------------- */
			/* Returns attribute_name or heap memory */
			/* ------------------------------------- */
			attribute_full_attribute_name(
				many_table_name,
				(char *)list_get( foreign_key_list )
					/* attribute_name */ ) );

	} while ( list_next( foreign_key_list ) );

	return strdup( where );
}

boolean query_is_null_drop_down_boolean(
		DICTIONARY *dictionary,
		LIST *foreign_key_list,
		int highest_index )
{
	int index;

	if ( highest_index < 0 ) return 0;

	for (	index = 0;
		index <= highest_index;
		index++ )
	{
		if ( string_strcmp(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_drop_down_key(
					foreign_key_list,
					index,
					ATTRIBUTE_MULTI_KEY_DELIMITER ),
				dictionary ),
			QUERY_IS_NULL ) == 0 )
		{
			return 1;
		}
	}

	return 0;
}

QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down_new(
		char *many_table_name,
		LIST *foreign_key_list )
{
	QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down;

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_not_null_drop_down = query_not_null_drop_down_calloc();
	query_not_null_drop_down->foreign_key_list = foreign_key_list;

	query_not_null_drop_down->where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_not_null_drop_down_where(
			many_table_name,
			foreign_key_list );

	return query_not_null_drop_down;
}

QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down_calloc( void )
{
	QUERY_NOT_NULL_DROP_DOWN *query_not_null_drop_down;

	if ( ! ( query_not_null_drop_down =
			calloc( 1, sizeof ( QUERY_NOT_NULL_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_not_null_drop_down;
}

char *query_not_null_drop_down_where(
			char *many_table_name,
			LIST *foreign_key_list )
{
	char where[ 1024 ];
	char *ptr = where;

	if ( !list_rewind( foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s is not null",
			/* ------------------------------------- */
			/* Returns attribute_name or heap memory */
			/* ------------------------------------- */
			attribute_full_attribute_name(
				many_table_name,
				(char *)list_get( foreign_key_list )
					/* attribute_name */ ) );

	} while ( list_next( foreign_key_list ) );

	return strdup( where );
}

boolean query_not_null_drop_down_boolean(
			DICTIONARY *dictionary,
			LIST *foreign_key_list,
			int highest_index )
{
	int index;

	if ( highest_index < 0 ) return 0;

	for (	index = 0;
		index <= highest_index;
		index++ )
	{
		if ( string_strcmp(
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_drop_down_key(
					foreign_key_list,
					index,
					ATTRIBUTE_MULTI_KEY_DELIMITER ),
				dictionary ),
			QUERY_NOT_NULL ) == 0 )
		{
			return 1;
		}
	}

	return 0;
}

QUERY_DROP_DOWN_FETCH *query_drop_down_fetch_new(
		char *application_name,
		char *login_name,
		char *one_folder_name,
		LIST *one_folder_attribute_list,
		LIST *relation_mto1_to_one_list,
		DICTIONARY *drop_down_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *common_name_list )
{
	QUERY_DROP_DOWN_FETCH *query_drop_down_fetch;

	if ( !application_name
	||   !login_name
	||   !one_folder_name
	||   !list_length( one_folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_fetch = query_drop_down_fetch_calloc();

	query_drop_down_fetch->query_select_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_select_list(
			folder_attribute_primary_list(
				one_folder_name,
				one_folder_attribute_list ) );

	query_drop_down_fetch->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_drop_down_fetch->query_select_list,
			common_name_list );

	query_drop_down_fetch->query_select_name_list =
		query_select_name_list(
			query_drop_down_fetch->query_select_list );

	query_drop_down_fetch->
		query_drop_down_where =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_where_new(
				application_name,
				one_folder_name,
				one_folder_attribute_list,
				relation_mto1_to_one_list,
				(security_entity)
					? security_entity->where
					: (char *)0,
				drop_down_dictionary );

	query_drop_down_fetch->query_order_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_order_string(
			query_drop_down_fetch->query_select_name_list
				/* folder_attribute_primary_key_list */,
			(LIST *)0 /* folder_attribute_name_list */,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_drop_down_fetch->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_drop_down_fetch->query_select_list_string,
			one_folder_name /* from_string */,
			query_drop_down_fetch->
				query_drop_down_where->
				string,
			(char *)0 /* group_by_string */,
			query_drop_down_fetch->query_order_string,
			QUERY_DROP_DOWN_FETCH_MAX_ROWS );

	query_drop_down_fetch->destination_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	query_drop_down_fetch->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			one_folder_name,
			one_folder_attribute_list
				/* folder_attribute_append_isa_list */,
			query_drop_down_fetch->destination_enum,
			query_drop_down_fetch->query_select_name_list,
			query_drop_down_fetch->query_system_string,
			0 /* not input_save_boolean */ );

	query_drop_down_fetch->query_row_delimited_list =
		query_row_delimited_list(
			SQL_DELIMITER,
			query_drop_down_fetch->
				query_fetch->
				row_list );

	return query_drop_down_fetch;
}

QUERY_DROP_DOWN_FETCH *query_drop_down_fetch_calloc( void )
{
	QUERY_DROP_DOWN_FETCH *query_drop_down_fetch;

	if ( ! ( query_drop_down_fetch =
			calloc( 1, sizeof ( QUERY_DROP_DOWN_FETCH ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_drop_down_fetch;
}

QUERY_DROP_DOWN_WHERE *query_drop_down_where_new(
		char *application_name,
		char *one_folder_name,
		LIST *one_folder_attribute_list,
		LIST *relation_mto1_to_one_list,
		char *security_entity_where,
		DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN_WHERE *query_drop_down_where;

	if ( !application_name
	||   !one_folder_name
	||   !list_length( one_folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_where = query_drop_down_where_calloc();

	query_drop_down_where->query_table_name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_table_name(
			application_name,
			one_folder_name,
			0 /* relation_mto1_isa_list_length */,
			(char *)0 /* attribute_not_null */ );

	query_drop_down_where->
		query_drop_down_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_list_new(
				relation_mto1_to_one_list,
				dictionary,
				query_drop_down_where->
					query_table_name
					/* many_table_name */ );

	query_drop_down_where->
		query_attribute_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_attribute_list_new(
				application_name,
				one_folder_attribute_list
				    /* folder_attribute_append_isa_list */,
				dictionary,
				0 /* relation_mto1_isa_list_length */,
				(char *)0
				    /* row_security_role_update_list_where */ );

	query_drop_down_where->string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_drop_down_where_string(
			security_entity_where,
			query_drop_down_where->
				query_drop_down_list->
				query_drop_down_relation_where,
			query_drop_down_where->
				query_attribute_list->
				where );

	return query_drop_down_where;
}

QUERY_DROP_DOWN_WHERE *query_drop_down_where_calloc( void )
{
	QUERY_DROP_DOWN_WHERE *query_drop_down_where;

	if ( ! ( query_drop_down_where =
			calloc(	1,
				sizeof ( QUERY_DROP_DOWN_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_drop_down_where;
}

char *query_drop_down_where_string(
		char *security_entity_where,
		char *query_drop_down_relation_where,
		char *query_attribute_list_where )
{
	char string[ STRING_WHERE_BUFFER ];
	char *ptr = string;

	*string = '\0';

	if ( query_drop_down_relation_where )
	{
		if (	strlen( query_drop_down_relation_where ) >=
			STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_relation_where );
	}

	if ( query_attribute_list_where )
	{
		if (	strlen( string ) +
			strlen( query_attribute_list_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_attribute_list_where );
	}

	if ( security_entity_where )
	{
		if (	strlen( string ) +
			strlen( security_entity_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( ptr != string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			security_entity_where );
	}

	if ( ptr == string )
		return (char *)0;
	else
		return strdup( string );
}

QUERY_CHOOSE_ISA *query_choose_isa_new(
		char *application_name,
		char *login_name,
		char *one_isa_folder_name,
		LIST *folder_attribute_primary_list,
		char *security_entity_where )
{
	QUERY_CHOOSE_ISA *query_choose_isa;

	if ( !application_name
	||   !login_name
	||   !one_isa_folder_name
	||   !list_length( folder_attribute_primary_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_choose_isa = query_choose_isa_calloc();

	query_choose_isa->query_select_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_select_list(
			folder_attribute_primary_list );

	query_choose_isa->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_choose_isa->query_select_list,
			(LIST *)0 /* common_name_list */ );

	query_choose_isa->query_select_name_list =
		query_select_name_list(
			query_choose_isa->query_select_list );

	query_choose_isa->query_order_string =
		/* ----------------------------------------------- */
		/* Returns query_select_list_string or heap memory */
		/* ----------------------------------------------- */
		query_order_string(
			query_choose_isa->query_select_name_list
				/* folder_attribute_primary_key_list */,
			(LIST *)0 /* folder_attribute_name_list */,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_choose_isa->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_choose_isa->query_select_list_string,
			one_isa_folder_name /* from_string */,
			security_entity_where,
			(char *)0 /* group_by_string */,
			query_choose_isa->query_order_string,
			0 /* max_rows */ );

	query_choose_isa->destination_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	query_choose_isa->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			one_isa_folder_name,
			folder_attribute_primary_list
				/* folder_attribute_append_isa_list */,
			query_choose_isa->destination_enum,
			query_choose_isa->query_select_name_list,
			query_choose_isa->query_system_string,
			0 /* not input_save_boolean */ );

	query_choose_isa->query_row_delimited_list =
		query_row_delimited_list(
			SQL_DELIMITER,
			query_choose_isa->query_fetch->row_list );

	return query_choose_isa;
}

QUERY_CHOOSE_ISA *query_choose_isa_calloc( void )
{
	QUERY_CHOOSE_ISA *query_choose_isa;

	if ( ! ( query_choose_isa =
			calloc( 1, sizeof ( QUERY_CHOOSE_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return query_choose_isa;
}

boolean query_date_time_between_exists(
		char *attribute_name,
		LIST *query_date_time_between_list )
{
	QUERY_DATE_TIME_BETWEEN *query_date_time_between;

	if ( !attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( query_date_time_between_list ) ) return 0;

	do {
		query_date_time_between =
			list_get(
				query_date_time_between_list );

		if (	string_strcmp(
				query_date_time_between->
					date_attribute_name,
				attribute_name ) == 0
		||	string_strcmp(
				query_date_time_between->
					time_attribute_name,
				attribute_name ) == 0 )
		{
			return 1;
		}

	} while ( list_next( query_date_time_between_list ) );

	return 0;
}

char *query_date_time_between_date_from_data(
		DICTIONARY *dictionary,
		char *date_attribute_name,
		char *appaserver_from_starting_label )
{
	if ( !date_attribute_name
	||   !appaserver_from_starting_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (char *)0;

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	query_dictionary_extract(
		dictionary,
		date_attribute_name,
		appaserver_from_starting_label,
		(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
		0 /* not attribute_is_number */ );
}

char *query_date_time_between_date_to_data(
		DICTIONARY *dictionary,
		char *date_attribute_name,
		char *prompt_lookup_to_prefix )
{
	if ( !date_attribute_name
	||   !prompt_lookup_to_prefix )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( dictionary ) ) return (char *)0;

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	query_dictionary_extract(
		dictionary,
		date_attribute_name,
		(char *)0 /* PROMPT_LOOKUP_FROM_PREFIX */,
		prompt_lookup_to_prefix,
		0 /* not attribute_is_number */ );
}

char *query_date_time_between_time_from_data(
		DICTIONARY *dictionary,
		char *time_attribute_name,
		char *prompt_lookup_from_prefix )
{
	if ( !time_attribute_name
	||   !prompt_lookup_from_prefix )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (char *)0;

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	query_dictionary_extract(
		dictionary,
		time_attribute_name,
		prompt_lookup_from_prefix,
		(char *)0 /* PROMPT_LOOKUP_TO_PREFIX */,
		0 /* not attribute_is_number */ );
}

char *query_date_time_between_time_to_data(
		DICTIONARY *dictionary,
		char *time_attribute_name,
		char *prompt_lookup_to_prefix )
{
	if ( !time_attribute_name
	||   !prompt_lookup_to_prefix )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( dictionary ) ) return (char *)0;

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	query_dictionary_extract(
		dictionary,
		time_attribute_name,
		(char *)0 /* PROMPT_LOOKUP_FROM_PREFIX */,
		prompt_lookup_to_prefix,
		0 /* not attribute_is_number */ );
}

LIST *query_cell_primary_data_list( LIST *query_row_cell_list )
{
	QUERY_CELL *query_cell;
	LIST *primary_data_list;

	if ( !list_rewind( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "query_row_cell_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_data_list = list_new();

	do {
		query_cell = list_get( query_row_cell_list );

		if ( query_cell->primary_key_index )
		{
			list_set(
				primary_data_list,
				query_cell->select_datum );
		}

	} while ( list_next( query_row_cell_list ) );

	return primary_data_list;
}

char *query_data_where(
		char *folder_table_name,
		LIST *where_attribute_name_list,
		LIST *where_attribute_data_list,
		LIST *folder_attribute_list )
{
	char where[ 1024 ];
	char *ptr = where;
	char *attribute_name;
	char escaped_data[ 1024 ];
	FOLDER_ATTRIBUTE *folder_attribute = {0};

	if ( !list_length( where_attribute_name_list ) ) return (char *)0;

	if ( list_length( where_attribute_name_list ) !=
	     list_length( where_attribute_data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"list_length()=%d != list_length=%d.",
			list_length( where_attribute_name_list ),
			list_length( where_attribute_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_rewind( where_attribute_name_list );
	list_rewind( where_attribute_data_list );

	do {
		attribute_name =
			list_get_pointer(
				where_attribute_name_list );

		if ( list_length( folder_attribute_list ) )
		{
			folder_attribute =
				folder_attribute_seek(
					attribute_name,
					folder_attribute_list );

			if ( !folder_attribute->attribute )
			{
				char message[ 128 ];

				sprintf(message,
				"folder_attribute->attribute is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

		}

		timlib_strcpy(
			escaped_data,
			list_get( where_attribute_data_list ),
			1024 );

		escape_single_quotes( escaped_data );

		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		if ( folder_table_name )
		{
			if ( folder_attribute
			&&   attribute_is_date_time(
				folder_attribute->
					attribute->
					datatype_name )
			&&     folder_attribute->
					attribute->
					width == 16 )
			{
				ptr += sprintf( ptr,
						"substr(%s.%s,1,16) = '%s'",
						folder_table_name,
						attribute_name,
						escaped_data );
			}
			else
			{
				ptr += sprintf( ptr,
				 		"%s.%s = '%s'",
				 		folder_table_name,
				 		attribute_name,
				 		escaped_data );
			}
		}
		else
		{
			ptr += sprintf( ptr,
				 	"%s = '%s'",
				 	attribute_name,
				 	escaped_data );
		}

		if ( !list_next( where_attribute_data_list ) ) break;

	} while( list_next( where_attribute_name_list ) );

	return strdup( where );
}

LIST *query_row_delimited_list(
		char sql_delimiter,
		LIST *query_fetch_row_list )
{
	QUERY_ROW *query_row;
	LIST *delimited_list;

	if ( !sql_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "sql_delimiter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_fetch_row_list ) ) return (LIST *)0;

	delimited_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		list_set(
			delimited_list,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_cell_list_delimited_string(
				sql_delimiter,
				query_row->cell_list ) );

	} while ( list_next( query_fetch_row_list ) );

	return delimited_list;
}

char *query_cell_list_delimited_string(
		char sql_delimiter,
		LIST *query_row_cell_list )
{
	char delimited_string[ 65536 ];
	char *ptr = delimited_string;
	QUERY_CELL *query_cell;

	if ( !sql_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "sql_delimiter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_row_cell_list ) ) return (char *)0;

	do {
		query_cell = list_get( query_row_cell_list );

		if ( ptr != delimited_string )
			ptr += sprintf( ptr, "%c", sql_delimiter );

		ptr += sprintf(
			ptr,
			"%s",
			query_cell->select_datum );

	} while ( list_next( query_row_cell_list ) );

	return strdup( delimited_string );
}

LIST *query_primary_delimited_list(
		char *folder_table_name,
		LIST *primary_key_list,
		LIST *foreign_key_list,
		LIST *foreign_data_list )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		list_display_delimited(
			primary_key_list, ',' ),
		folder_table_name,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_data_where(
			(char *)0
				/* folder_name */,
			foreign_key_list
				/* where_attribute_name_list */,
			foreign_data_list
				/* where_attribute_data_list */,
			(LIST *)0
				/* folder_attribute_list */ ) );

	return list_pipe_fetch( system_string );
}

char *query_drop_down_key(
		LIST *foreign_key_list,
		int index,
		char multi_attribute_key_delimiter )
{
	static char key[ 128 ];
	char *delimited_string;

	if ( !list_length( foreign_key_list )
	||   !multi_attribute_key_delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delimited_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		list_delimited_string(
			foreign_key_list,
			multi_attribute_key_delimiter );

	if ( !index )
	{
		strcpy( key, delimited_string );
	}
	else
	{
		sprintf(key,
			"%s_%d",
			delimited_string,
			index );
	}

	free( delimited_string );

	return key;
}

char *query_cell_quote_comma_string(
		QUERY_CELL *query_cell,
		boolean first_cell_boolean )
{
	char quote_comma_string[ 4096 ];
	char *ptr = quote_comma_string;

	if ( !query_cell )
	{
		char message[ 128 ];

		sprintf(message, "query_cell is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !first_cell_boolean ) ptr += sprintf( ptr, "," );

	if ( !query_cell->display_datum ) return strdup( "" );

	if ( query_cell->attribute_is_number )
	{
		ptr +=
			sprintf(
				ptr,
				"%s",
				query_cell->select_datum );
	}
	else
	{
		ptr +=
			sprintf(
				ptr,
				"\"%s\"",
			 	query_cell->display_datum );
	}

	return strdup( quote_comma_string );
}

void query_cell_free( QUERY_CELL *query_cell )
{
	if ( !query_cell ) return;

	if (	query_cell->select_datum
	&&	query_cell->select_datum !=
		query_cell->display_datum )
	{
		free( query_cell->select_datum );
	}

	free( query_cell );
}

void query_row_spreadsheet_output(
		FILE *output_file,
		QUERY_ROW *query_row )
{
	char output_string[ STRING_128K ];
	char *ptr = output_string;
	QUERY_CELL *query_cell;
	char *quote_comma_string;

	if ( !output_file
	||   !query_row
	||   !list_rewind( query_row->cell_list ) )
	{
		return;
	}

	do {
		query_cell = list_get( query_row->cell_list );

		quote_comma_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_cell_quote_comma_string(
				query_cell,
				list_at_first( query_row->cell_list )
					/* first_cell_boolean */ );

		ptr +=
			sprintf(
				ptr,
				"%s",
				quote_comma_string );

		free( quote_comma_string );

	} while ( list_next( query_row->cell_list ) );

	fprintf( output_file, "%s\n", output_string );
}

void query_row_free( QUERY_ROW *query_row )
{
	QUERY_CELL *query_cell;

	if ( !query_row ) return;

	if ( list_rewind( query_row->cell_list ) )
	do {
		query_cell = list_get( query_row->cell_list );
		query_cell_free( query_cell );

	} while ( list_next( query_row->cell_list ) );

	free( query_row );
}

unsigned long query_spreadsheet_output(
		LIST *folder_attribute_append_isa_list,
		enum date_convert_format_enum destination_enum,
		LIST *query_select_name_list,
		char *query_system_string,
		char *output_filename )
{
	FILE *input_pipe;
	FILE *output_file;
	char input[ STRING_256K ];
	QUERY_ROW *query_row;
	unsigned long row_count = 0;
	char destination[ 1024 ];

	if ( !list_length( query_select_name_list ) ) return 0;

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_input_pipe(
			query_system_string );

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_spreadsheet_output_file(
			output_filename );

	fprintf(output_file,
		"%s\n",
		/* ------------------- */
		/* Returns destination */
		/* ------------------- */
		list_double_quote_comma_display(
			destination,
			query_select_name_list ) );
		
	while ( string_input( input, input_pipe, sizeof ( input ) ) )
	{
		query_row =
			query_row_parse(
				SQL_DELIMITER,
				folder_attribute_append_isa_list,
				destination_enum,
				query_select_name_list,
				0 /* not input_save_boolean */,
				input /* stack memory */ );

		if ( !query_row ) continue;

		query_row_spreadsheet_output(
			output_file,
			query_row );

		query_row_free( query_row );
		row_count++;
	}

	pclose( input_pipe );
	fclose( output_file );

	return row_count;
}

FILE *query_spreadsheet_output_file( char *output_filename )
{
	FILE *output_file;

	if ( !output_filename )
	{
		char message[ 128 ];

		sprintf(message, "output_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"fopen(%s) returned empty.",
			output_filename );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return output_file;
}

QUERY_SPREADSHEET *query_spreadsheet_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *no_display_name_list,
		LIST *role_attribute_exclude_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *folder_attribute_primary_key_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list )
{
	QUERY_SPREADSHEET *query_spreadsheet;

	query_spreadsheet = query_spreadsheet_calloc();

	query_spreadsheet->query_select_table_edit_list =
		query_select_table_edit_list(
			application_name,
			folder_name,
			folder_attribute_append_isa_list,
			no_display_name_list,
			role_attribute_exclude_name_list,
			list_length( relation_mto1_isa_list ),
			(char *)0 /* attribute_not_null */ );

	query_spreadsheet->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_spreadsheet->
				query_select_table_edit_list,
			(LIST *)0 /* common_name_list */ );

	query_spreadsheet->query_select_name_list =
		query_select_name_list(
			query_spreadsheet->
				query_select_table_edit_list );

	query_spreadsheet->query_from_string =
		/* -------------------- */
		/* Returns heap memory. */
		/* -------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(char *)0 /* row_security_role_update_list_from */ );	

	query_spreadsheet->query_table_edit_where =
		/* ---------------------------- */
		/* Safely returns.		*/
		/* Note: frees each where.	*/
		/* ---------------------------- */
		query_table_edit_where_new(
			application_name,
			folder_name,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			security_entity_where,
			query_dictionary,
			(char *)0
				/* row_security_role_update_list_where */ );

	query_spreadsheet->query_order_string =
		/* ----------------------------------------------- */
		/* Returns query_select_list_string or heap memory */
		/* ----------------------------------------------- */
		query_order_string(
			folder_attribute_primary_key_list,
			(LIST *)0 /* folder_attribute_name_list */,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_spreadsheet->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_spreadsheet->query_select_list_string,
			query_spreadsheet->query_from_string,
			query_spreadsheet->query_table_edit_where->string,
			(char *)0 /* group_by_string */,
			query_spreadsheet->query_order_string,
			QUERY_SPREADSHEET_MAX_ROWS );

	query_spreadsheet->destination_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	return query_spreadsheet;
}

QUERY_SPREADSHEET *query_spreadsheet_calloc( void )
{
	QUERY_SPREADSHEET *query_spreadsheet;

	if ( ! ( query_spreadsheet =
			calloc( 1,
				sizeof ( QUERY_SPREADSHEET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_spreadsheet;
}

QUERY_CHART *query_chart_calloc( void )
{
	QUERY_CHART *query_chart;

	if ( ! ( query_chart = calloc( 1, sizeof ( QUERY_CHART ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_chart;
}

char *query_chart_date_attribute_name(
			LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (char *)0;

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( attribute_is_date(
			folder_attribute->attribute->datatype_name )
		||   attribute_is_date_time(
			folder_attribute->attribute->datatype_name )
		||   attribute_is_current_date_time(
			folder_attribute->attribute->datatype_name ) )
		{
			return folder_attribute->attribute_name;
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	return (char *)0;
}

char *query_chart_time_attribute_name(
			LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (char *)0;

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( attribute_is_time(
			folder_attribute->attribute->datatype_name ) )
		{
			return folder_attribute->attribute_name;
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	return (char *)0;
}

LIST *query_chart_number_attribute_name_list(
		LIST *no_display_name_list,
		LIST *folder_attribute_append_isa_list )
{
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *number_attribute_name_list;

	if ( !list_rewind( folder_attribute_append_isa_list ) )
		return (LIST *)0;

	number_attribute_name_list = list_new();

	do {
		folder_attribute =
			list_get(
				folder_attribute_append_isa_list );

		if ( !folder_attribute->attribute )
		{
			char message[ 128 ];

			sprintf(message,
				"folder_attribute->attribute is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( list_string_exists(
			folder_attribute->attribute_name,
			no_display_name_list ) )
		{
			continue;
		}

		if ( attribute_is_number(
			folder_attribute->attribute->datatype_name ) )
		{
			list_set(
				number_attribute_name_list,
				folder_attribute->attribute_name );
		}

	} while ( list_next( folder_attribute_append_isa_list ) );

	return number_attribute_name_list;
}

LIST *query_chart_select_list(
		char *application_name,
		char *folder_name,
		int relation_mto1_isa_list_length,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *number_attribute_name_list )
{
	LIST *select_list;
	char *number_attribute_name;

	if ( !application_name
	||   !folder_name
	||   !date_attribute_name
	||   !list_rewind( number_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	select_list = list_new();

	list_set(
		select_list,
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_select_new(
			application_name,
			relation_mto1_isa_list_length,
			(char *)0 /* attribute_not_null */,
			folder_name,
			date_attribute_name ) );

	if ( time_attribute_name )
	{
		list_set(
			select_list,
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				(char *)0 /* attribute_not_null */,
				folder_name,
				time_attribute_name ) );
	}

	do {
		number_attribute_name =
			list_get(
				number_attribute_name_list );

		list_set(
			select_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				(char *)0 /* application_name */,
				0 /* relation_mto1_isa_list_length */,
				(char *)0 /* attribute_not_null */,
				(char *)0 /* folder_name */,
				number_attribute_name ) );

	} while ( list_next( number_attribute_name_list ) );

	return select_list;
}

QUERY_CHART *query_chart_new(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *no_display_name_list,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list )
{
	QUERY_CHART *query_chart;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_chart = query_chart_calloc();

	query_chart->date_attribute_name =
		/* ------------------------- */
		/* Returns component or null */
		/* ------------------------- */
		query_chart_date_attribute_name(
			folder_attribute_append_isa_list );

	if ( !query_chart->date_attribute_name ) return NULL;

	query_chart->time_attribute_name =
		/* ------------------------- */
		/* Returns component or null */
		/* ------------------------- */
		query_chart_time_attribute_name(
			folder_attribute_append_isa_list );

	query_chart->number_attribute_name_list =
		/* ---------------------------------- */
		/* Returns list of components or null */
		/* ---------------------------------- */
		query_chart_number_attribute_name_list(
			no_display_name_list,
			folder_attribute_append_isa_list );

	if ( !list_length( query_chart->number_attribute_name_list ) )
	{
		return NULL;
	}

	query_chart->select_list =
		query_chart_select_list(
			application_name,
			folder_name,
			list_length( relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */,
			query_chart->date_attribute_name,
			query_chart->time_attribute_name,
			query_chart->number_attribute_name_list );

	query_chart->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_chart->select_list,
		(LIST *)0 /* common_name_list */ );

	query_chart->query_select_name_list =
		query_select_name_list(
			query_chart->select_list );

	query_chart->query_from_string =
		/* -------------------------------------------- */
		/* Returns heap memory.				*/
		/* Note: folder_table_name() is not used here.	*/
		/* -------------------------------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(char *)0 /* row_security_role_update_list_from */ );	

	query_chart->query_table_edit_where =
		/* ---------------------------- */
		/* Safely returns.		*/
		/* Note: frees each where.	*/
		/* ---------------------------- */
		query_table_edit_where_new(
			application_name,
			folder_name,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			security_entity_where,
			query_dictionary,
			(char *)0
				/* row_security_role_update_list_where */ );

	query_chart->order_name_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_chart_order_name_list(
			query_chart->date_attribute_name,
			query_chart->time_attribute_name );

	query_chart->query_order_string =
		/* ----------------------------------------------- */
		/* Returns query_select_list_string or heap memory */
		/* ----------------------------------------------- */
		query_order_string(
			query_chart->order_name_list
				/* folder_attribute_primary_key_list */,
			(LIST *)0 /* folder_attribute_name_list */,
			(DICTIONARY *)0 /* sort_dictionary */ );

	query_chart->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_chart->query_select_list_string,
			query_chart->query_from_string,
			query_chart->query_table_edit_where->string,
			(char *)0 /* group_by_string */,
			query_chart->query_order_string,
			QUERY_CHART_MAX_ROWS );

	query_chart->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			folder_name,
			folder_attribute_append_isa_list,
			date_convert_international
				/* destination_enum */,
			query_chart->query_select_name_list,
			query_chart->query_system_string,
			0 /* not input_save_boolean */ );

	return query_chart;
}

LIST *query_chart_order_name_list(
		char *query_chart_date_attribute_name,
		char *query_chart_time_attribute_name )
{
	LIST *name_list;

	if ( !query_chart_date_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "query_chart_date_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	name_list = list_new();

	list_set(
		name_list,
		query_chart_date_attribute_name );

	if ( query_chart_time_attribute_name )
	{
		list_set(
			name_list,
			query_chart_time_attribute_name );
	}

	return name_list;
}

int query_row_list_days_range(
		char *date_attribute_name,
		LIST *row_list )
{
	QUERY_ROW *first_row;
	QUERY_ROW *last_row;
	QUERY_CELL *first_cell;
	QUERY_CELL *last_cell;

	if ( !date_attribute_name
	||   !list_length( row_list ) )
	{
		return 0;
	}

	first_row = list_first( row_list );
	last_row = list_last( row_list );

	if ( ! ( first_cell =
			query_cell_seek(
				date_attribute_name,
				first_row->cell_list ) ) )
	{
		return 0;
	}

	last_cell =
		query_cell_seek(
			date_attribute_name,
			last_row->cell_list );

	return
	date_days_between(
		first_cell->select_datum,
		last_cell->select_datum ) + 1;
}

void query_table_edit_where_free(
		QUERY_TABLE_EDIT_WHERE *query_table_edit_where )
{
	if ( !query_table_edit_where ) return;

	string_free(
		query_table_edit_where->
			query_drop_down_list->
			query_drop_down_relation_where );

	string_free(
		query_table_edit_where->
			query_isa_drop_down_list->
			where );

	string_free(
		query_table_edit_where->
			drop_down_where );

	string_free( query_table_edit_where->string );

	free( query_table_edit_where );
}

void query_chart_free( QUERY_CHART *query_chart )
{
	if ( !query_chart ) return;

	query_table_edit_where_free(
		query_chart->query_table_edit_where );

	free( query_chart );
}

QUERY_GROUP *query_group_new(
		char *application_name,
		char *folder_name,
		char *query_table_edit_where_string,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		LIST *relation_foreign_key_list )
{
	QUERY_GROUP *query_group;

	if ( !application_name
	||   !folder_name
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_group = query_group_calloc();

	query_group->select_list =
		query_group_select_list(
			application_name,
			folder_name,
			folder_attribute_float_name_list,
			group_count_row_count,
			relation_foreign_key_list,
			list_length( relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */ );

	query_group->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_group->select_list,
		(LIST *)0 /* common_name_list */ );

	query_group->query_select_name_list =
		query_select_name_list(
			query_group->select_list );

	query_group->query_from_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(char *)0 /* row_security_role_update_list_from */ );	

	query_group->query_group_by_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_group_by_string(
			relation_foreign_key_list );

	query_group->order_string =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		query_group_order_string();

	query_group->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_group->query_select_list_string,
			query_group->query_from_string,
			query_table_edit_where_string,
			query_group->query_group_by_string,
			query_group->order_string,
			0 /* max_rows */ );

	return query_group;
}

QUERY_GROUP *query_group_calloc( void )
{
	QUERY_GROUP *query_group;

	if ( ! ( query_group = calloc( 1, sizeof ( QUERY_GROUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_group;
}

char *query_group_by_string( LIST *foreign_key_list )
{
	if ( !list_length( foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key_list is empty." );

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
	list_display_delimited(
		foreign_key_list,
		',' );
}

LIST *query_group_select_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_float_name_list,
		unsigned long group_count_row_count,
		LIST *foreign_key_list,
		int relation_mto1_isa_list_length )
{
	LIST *select_list;
	char *float_attribute_name;
	char *foreign_key;

	if ( !application_name
	||   !folder_name
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

	select_list = list_new();

	do {
		foreign_key = list_get( foreign_key_list );

		list_set(
			select_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				(char *)0 /* attribute_not_null */,
				folder_name,
				foreign_key /* attribute_name */ ) );

	} while ( list_next( foreign_key_list ) );

	list_set(
		select_list,
		query_select_new(
			(char *)0 /* application_name */,
			0 /* relation_mto1_isa_list_length */,
			(char *)0 /* attribute_not_null */,
			(char *)0 /* folder_name */,
			"count(1)" /* attribute_name */ ) );


	list_set(
		select_list,
		query_select_new(
			(char *)0 /* application_name */,
			0 /* relation_mto1_isa_list_length */,
			(char *)0 /* attribute_not_null */,
			(char *)0 /* folder_name */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_group_percent_expression(
				group_count_row_count ) ) );

	if ( list_rewind( folder_attribute_float_name_list ) )
	do {
		float_attribute_name =
			list_get(
				folder_attribute_float_name_list );


		list_set(
			select_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				(char *)0 /* application_name */,
				0 /* relation_mto1_isa_list_length */,
				(char *)0 /* attribute_not_null */,
				(char *)0 /* folder_name */,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				query_group_average_expression(
					float_attribute_name ) ) );

	} while ( list_next( folder_attribute_float_name_list ) );

	return select_list;
}

char *query_group_order_string( void )
{
	return "count(1) desc";
}

char *query_group_percent_expression( unsigned long group_count_row_count )
{
	char expression[ 128 ];

	sprintf(expression,
		"(count(1) / %ld) * 100.0",
		group_count_row_count );

	return strdup( expression );
}

char *query_group_average_expression( char *float_attribute_name )
{
	char expression[ 128 ];

	sprintf(expression,
		"avg(%s)",
		float_attribute_name );

	return strdup( expression );
}

LIST *query_select_primary_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		char *additional_attribute_name,
		int relation_mto1_isa_list_length )
{
	LIST *query_select_list;
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !application_name
	||   !folder_name
	||   !list_rewind( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_select_list = list_new();

	do {
		folder_attribute =
			list_get( folder_attribute_append_isa_list );

		if ( strcmp(	folder_attribute->folder_name,
				folder_name ) != 0
		||   !folder_attribute->primary_key_index )
		{
			continue;
		}

		list_set(
			query_select_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				(char *)0 /* attribute_not_null */,
				folder_attribute->folder_name,
				folder_attribute->attribute_name ) );

	} while ( list_next( folder_attribute_append_isa_list ) );

	if ( additional_attribute_name )
	{
		list_set(
			query_select_list,
			query_select_new(
				(char *)0 /* application_name */,
				0 /* relation_mto1_isa_list_length */,
				(char *)0 /* attribute_not_null */,
				(char *)0 /* folder_name */,
				additional_attribute_name ) );
	}

	return query_select_list;
}

QUERY_PRIMARY_KEY *query_primary_key_fetch(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *security_entity_where,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		DICTIONARY *query_dictionary,
		char *additional_attribute_name )
{
	QUERY_PRIMARY_KEY *query_primary_key;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_primary_key = query_primary_key_calloc();

	query_primary_key->isa_list_length =
		query_primary_key_isa_list_length(
			relation_mto1_isa_list );

	query_primary_key->query_select_primary_list =
		query_select_primary_list(
			application_name,
			folder_name,
			folder_attribute_append_isa_list,
			additional_attribute_name,
			query_primary_key->isa_list_length
				/* relation_mto1_isa_list_length */ );

	if ( !list_length( query_primary_key->query_select_primary_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_select_primary_list(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_primary_key->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_primary_key->
				query_select_primary_list,
			(LIST *)0 /* common_name_list */ );

	query_primary_key->query_from_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(char *)0 /* row_security_role_update_list_from */ );

	query_primary_key->query_table_edit_where =
		/* ---------------------- */
		/* Safely returns.	  */
		/* Note: frees each where */
		/* ---------------------- */
		query_table_edit_where_new(
			application_name,
			folder_name,
			relation_mto1_list,
			relation_mto1_isa_list,
			folder_attribute_append_isa_list,
			security_entity_where,
			query_dictionary,
			(char *)0 /* row_security_role_update_list_where */ );

	query_primary_key->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_primary_key->query_select_list_string,
			query_primary_key->query_from_string,
			query_primary_key->query_table_edit_where->string,
			(char *)0 /* group_by_string */,
			additional_attribute_name /* query_order_string */,
			0 /* max_rows */ );

	query_primary_key->query_select_name_list =
		query_select_name_list(
			query_primary_key->query_select_primary_list );

	query_primary_key->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			folder_name,
			folder_attribute_append_isa_list,
			date_convert_international
				/* destination_enum */,
			query_primary_key->query_select_name_list,
			query_primary_key->query_system_string,
			0 /* not input_save_boolean */ );

	return query_primary_key;
}

QUERY_PRIMARY_KEY *query_primary_key_calloc( void )
{
	QUERY_PRIMARY_KEY *query_primary_key;

	if ( ! ( query_primary_key =
			calloc( 1,
				sizeof ( QUERY_PRIMARY_KEY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_primary_key;
}

LIST *query_date_time_between_attribute_name_list(
		LIST *query_date_time_between_list )
{
	LIST *attribute_name_list;
	QUERY_DATE_TIME_BETWEEN *query_date_time_between;

	attribute_name_list = list_new();

	if ( !list_rewind( query_date_time_between_list ) )
		return attribute_name_list;

	do {
		query_date_time_between =
			list_get(
				query_date_time_between_list );

		list_set(
			attribute_name_list,
			query_date_time_between->
				date_attribute_name );

		list_set(
			attribute_name_list,
			query_date_time_between->
				time_attribute_name );

	} while ( list_next( query_date_time_between_list ) );

	return attribute_name_list;
}

LIST *query_drop_down_relation_attribute_name_list(
		LIST *query_drop_down_relation_list )
{
	QUERY_DROP_DOWN_RELATION *query_drop_down_relation;
	LIST *attribute_name_list = list_new();

	if ( list_rewind( query_drop_down_relation_list ) )
	do {
		query_drop_down_relation =
			list_get(
				query_drop_down_relation_list );

		if ( query_drop_down_relation->
			query_is_null_drop_down )
		{
			list_set_list(
				attribute_name_list,
				query_drop_down_relation->
					query_is_null_drop_down->
					foreign_key_list );
		}
		else
		if ( query_drop_down_relation->
			query_not_null_drop_down )
		{
			list_set_list(
				attribute_name_list,
				query_drop_down_relation->
					query_not_null_drop_down->
					foreign_key_list );
		}
		else
		if ( query_drop_down_relation->
			query_multi_select_drop_down )
		{
			list_set_list(
				attribute_name_list,
				query_drop_down_relation->
					query_multi_select_drop_down->
					foreign_key_list );
		}
		else
		if ( query_drop_down_relation->
			query_single_select_drop_down )
		{
			list_set_list(
				attribute_name_list,
				query_drop_down_relation->
					query_single_select_drop_down->
					foreign_key_list );
		}
		else
		{
			char message[ 128 ];

			sprintf(message,
"Each of is_null, not_null, multi_select, and single_select are empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

	} while ( list_next( query_drop_down_relation_list ) );

	return attribute_name_list;
}

LIST *query_attribute_list_name_list(
		LIST *query_date_time_between_list,
		LIST *query_attribute_list )
{
	QUERY_ATTRIBUTE *query_attribute;
	LIST *attribute_name_list;

	attribute_name_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_date_time_between_attribute_name_list(
			query_date_time_between_list );

	if ( list_rewind( query_attribute_list ) )
	do {
		query_attribute =
			list_get(
				query_attribute_list );

		list_set(
			attribute_name_list,
			query_attribute->attribute_name );

	} while ( list_next( query_attribute_list ) );

	return attribute_name_list;
}

char *query_cell_where_string( LIST *query_row_cell_list )
{
	char where_string[ 1024 ];
	char *ptr = where_string;
	QUERY_CELL *query_cell;
	char destination[ 256 ];

	if ( list_rewind( query_row_cell_list ) )
	do {
		query_cell = list_get( query_row_cell_list );

		if ( !query_cell->select_datum )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell->select_datum is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !query_cell->attribute_name ) continue;

		if ( ptr != where_string ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s = '%s'",
			query_cell->
				attribute_name,
			string_escape_quote(
				destination,
				query_cell->select_datum ) );

	} while ( list_next( query_row_cell_list ) );

	if ( ptr == where_string )
		return NULL;
	else
		return strdup( where_string );
}

void query_cell_command_line_replace(
		char *command_line,
		LIST *query_cell_list )
{
	QUERY_CELL *query_cell;
	char destination[ 256 ];
	int instr;

	if ( !command_line
	||   !list_rewind( query_cell_list ) )
	{
		return;
	}

	instr = string_instr( " ", command_line, 1 );

	if ( instr == -1 ) return;

	command_line += instr;

	do {
		query_cell = list_get( query_cell_list );

		if ( !query_cell->attribute_name ) continue;

		string_with_space_search_replace(
			command_line
					/* source_destination */,
			query_cell->attribute_name
					/* search */,
			double_quotes_around(
				destination,
				query_cell->
					select_datum )
					/* replace */ );

	} while ( list_next( query_cell_list ) );
}

char *query_attribute_yes_no_where(
		DICTIONARY *dictionary,
		char *table_name,
		char *attribute_name )
{
	char where[ 128 ];
	char *get;
	char *full_attribute_name;

	if ( !dictionary
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( get = dictionary_get( attribute_name, dictionary ) ) )
		return (char *)0;

	full_attribute_name =
		/* ------------------------------------- */
		/* Returns heap memory or attribute_name */
		/* ------------------------------------- */
		attribute_full_attribute_name(
			table_name,
			attribute_name );

	if ( *get == 'y' )
	{
		sprintf(where,
			"%s = 'y'",
			full_attribute_name );
	}
	else
	{
		sprintf(where,
			"(%s = 'n' or %s is null)",
			full_attribute_name,
			full_attribute_name );
	}

	return strdup( where );
}

LIST *query_cell_attribute_data_list(
		LIST *attribute_name_list,
		LIST *query_row_cell_list )
{
	QUERY_CELL *query_cell;
	char *attribute_name;
	LIST *data_list = list_new();

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name = list_get( attribute_name_list );

		if ( ( query_cell =
				query_cell_seek(
					attribute_name,
					query_row_cell_list ) ) )
		{
			list_set( data_list, query_cell->select_datum );
		}

	} while ( list_next( attribute_name_list ) );

	if ( !list_length( data_list ) )
	{
		list_free( data_list );
		data_list = NULL;

	}

	return data_list;
}

LIST *query_cell_attribute_list(
		LIST *attribute_name_list,
		LIST *query_row_cell_list )
{
	QUERY_CELL *query_cell;
	QUERY_CELL *new_query_cell;
	char *attribute_name;
	LIST *list = list_new();

	if ( list_rewind( attribute_name_list ) )
	do {
		attribute_name = list_get( attribute_name_list );

		if ( ( query_cell =
				query_cell_seek(
					attribute_name,
					query_row_cell_list ) ) )
		{
			new_query_cell =
				query_cell_simple_new(
					query_cell->attribute_name,
					query_cell->select_datum );

			list_set( list, new_query_cell );
		}

	} while ( list_next( attribute_name_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

LIST *query_cell_data_list( LIST *query_row_cell_list )
{
	QUERY_CELL *query_cell;
	LIST *data_list = list_new();

	if ( list_rewind( query_row_cell_list ) )
	do {
		query_cell = list_get( query_row_cell_list );

		list_set( data_list, query_cell->select_datum );

	} while ( list_next( query_row_cell_list ) );

	if ( !list_length( data_list ) )
	{
		list_free( data_list );
		data_list = NULL;
	}

	return data_list;
}

char *query_cell_list_display( LIST *cell_list )
{
	QUERY_CELL *query_cell;
	char display[ 65536 ];
	char *ptr = display;

	*ptr = '\0';

	if ( list_rewind( cell_list ) )
	do {
		query_cell = list_get( cell_list );

		if ( ptr != display ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_cell_display( query_cell ) );

	} while ( list_next( cell_list ) );

	if ( !*display ) strcpy( display, "NULL" );

	return strdup( display );
}

char *query_cell_display( QUERY_CELL *query_cell )
{
	static char display[ 2048 ];

	snprintf(
		display,
		sizeof ( display ),
		"[%s=%s; Display=%s; Primary=%d]",
		(query_cell->attribute_name)
			? query_cell->attribute_name
			: "anonymous",
		query_cell->select_datum,
		(query_cell->display_datum)
			? query_cell->display_datum
			: "",
		query_cell->primary_key_index );

	return display;
}

char *query_attribute_list_table_name(
		char *application_name,
		int relation_mto1_isa_list_length,
		char *row_security_role_update_list_where,
		char *folder_name )
{
	if ( !relation_mto1_isa_list_length
	&&   !row_security_role_update_list_where )
	{
		return (char *)0;
	}
	else
	return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			folder_name );
}

QUERY_DROP_DOWN *query_drop_down_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *one_folder_name,
		char *related_column,
		LIST *one_folder_attribute_list,
		char *populate_drop_down_process_name,
		LIST *relation_mto1_to_one_list,
		DICTIONARY *drop_down_dictionary,
		SECURITY_ENTITY *security_entity,
		LIST *common_name_list )
{
	QUERY_DROP_DOWN *query_drop_down;

	if ( !application_name
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( one_folder_attribute_list ) )
	{
		char message[ 128 ];

		sprintf(message, "one_folder_attribute_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down = query_drop_down_calloc();

	if ( populate_drop_down_process_name )
	{
		query_drop_down->query_drop_down_process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_process_new(
				application_name,
				session_key,
				login_name,
				role_name,
				state,
				many_folder_name,
				one_folder_name,
				related_column,
				one_folder_attribute_list,
				populate_drop_down_process_name,
				relation_mto1_to_one_list,
				drop_down_dictionary,
				security_entity );

		query_drop_down->delimited_list =
			query_drop_down->
				query_drop_down_process->
				delimited_list;
	}
	else
	{
		query_drop_down->query_drop_down_fetch =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_fetch_new(
				application_name,
				login_name,
				one_folder_name,
				one_folder_attribute_list,
				relation_mto1_to_one_list,
				drop_down_dictionary,
				security_entity,
				common_name_list );

		query_drop_down->delimited_list =
			query_drop_down->
				query_drop_down_fetch->
				query_row_delimited_list;
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_drop_down_calloc( void )
{
	QUERY_DROP_DOWN *query_drop_down;

	if ( ! ( query_drop_down = calloc( 1, sizeof ( QUERY_DROP_DOWN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN_PROCESS *query_drop_down_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *one_folder_name,
		char *related_column,
		LIST *one_folder_attribute_list,
		char *populate_drop_down_process_name,
		LIST *relation_mto1_to_one_list,
		DICTIONARY *dictionary,
		SECURITY_ENTITY *security_entity )
{
	QUERY_DROP_DOWN_PROCESS *query_drop_down_process;

	if ( !application_name
	||   !one_folder_name
	||   !populate_drop_down_process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_process = query_drop_down_process_calloc();

	query_drop_down_process->query_drop_down_where =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_drop_down_where_new(
			application_name,
			one_folder_name,
			one_folder_attribute_list,
			relation_mto1_to_one_list,
			(security_entity)
				? security_entity->where
				: (char *)0,
			dictionary );

	query_drop_down_process->process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_fetch(
			populate_drop_down_process_name,
			(char *)0 /* document_root */,
			(char *)0 /* relative_source_directory */,
			1 /* check_executable_inside_filesystem */,
			appaserver_parameter_mount_point() );

	query_drop_down_process->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	query_drop_down_process->command_line =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_drop_down_process_command_line(
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			many_folder_name,
			related_column,
			populate_drop_down_process_name,
			dictionary,
			query_drop_down_process->
				query_drop_down_where->
				string,
			query_drop_down_process->process->command_line,
			query_drop_down_process->appaserver_error_filename );

	query_drop_down_process->delimited_list =
		list_pipe_fetch(
			query_drop_down_process->command_line );

	return query_drop_down_process;
}

QUERY_DROP_DOWN_PROCESS *query_drop_down_process_calloc( void )
{
	QUERY_DROP_DOWN_PROCESS *query_drop_down_process;

	if ( ! ( query_drop_down_process =
			calloc( 1,
				sizeof ( QUERY_DROP_DOWN_PROCESS ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_drop_down_process;
}

char *query_drop_down_process_command_line(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *many_folder_name,
		char *related_column,
		char *populate_drop_down_process_name,
		DICTIONARY *dictionary,
		char *where_string,
		char *process_command_line,
		char *appaserver_error_filename )
{
	char command_line[ STRING_64K ];
	char *delimited;
	char *escape_dollar;

	if ( !process_command_line )
	{
		char message[ 128 ];

		sprintf(message, "process_command_line is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		process_command_line,
		sizeof ( command_line ) );

	string_replace_command_line(
		command_line,
		application_name,
		PROCESS_APPLICATION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		session_key,
		PROCESS_SESSION_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_NAME_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		login_name,
		PROCESS_LOGIN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		role_name,
		PROCESS_ROLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		state,
		PROCESS_STATE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		many_folder_name,
		PROCESS_MANY_TABLE_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		many_folder_name,
		PROCESS_MANY_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		related_column,
		PROCESS_RELATED_COLUMN_PLACEHOLDER );

	string_replace_command_line(
		command_line,
		populate_drop_down_process_name,
		PROCESS_NAME_PLACEHOLDER );

	dictionary_replace_command_line(
		command_line,
		dictionary );

	delimited =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_display_delimited(
			dictionary,
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER );

	string_replace_command_line(
		command_line,
		delimited,
		PROCESS_DICTIONARY_PLACEHOLDER );

	free( delimited );

	string_replace_command_line(
		command_line,
		where_string,
		PROCESS_WHERE_PLACEHOLDER );

	escape_dollar =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		string_escape_dollar(
			command_line /* source */ );

	if ( appaserver_error_filename )
	{
		char destination[ STRING_65K ];

		snprintf(
			destination,
			sizeof ( destination ),
			"%s 2>>%s",
			escape_dollar,
			appaserver_error_filename );

		free( escape_dollar );

		return strdup( destination );
	}
	else
	{
		return escape_dollar;
	}
}

QUERY_DICTIONARY *query_dictionary_fetch(
		char *application_name,
		DICTIONARY *dictionary,
		char *folder_name,
		LIST *select_attribute_name_list,
		LIST *where_attribute_name_list )
{
	QUERY_DICTIONARY *query_dictionary;
	QUERY_DROP_DOWN_DATUM *query_drop_down_datum;
	char *attribute_name;
	char *get;

	if ( !application_name
	||   !dictionary_length( dictionary )
	||   !folder_name
	||   !list_length( select_attribute_name_list )
	||   !list_rewind( where_attribute_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_dictionary = query_dictionary_calloc();

	query_dictionary->select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_dictionary_select_string(
			select_attribute_name_list );

	query_dictionary->query_drop_down_datum_list = list_new();

	do {
		attribute_name = list_get( where_attribute_name_list );

		get =
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				attribute_name,
				dictionary );

		if ( !get )
		{
			char message[ 128 ];

			sprintf(message,
				"dictionary_get(%s) returned empty.",
				attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		query_drop_down_datum =
			query_drop_down_datum_new(
				(char *)0 /* many_table_name */,
				attribute_name,
				get /* datum */ );

		list_set(
			query_dictionary->query_drop_down_datum_list,
			query_drop_down_datum );

	} while ( list_next( where_attribute_name_list ) );

	query_dictionary->query_drop_down_datum_list_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_drop_down_datum_list_where(
			query_dictionary->query_drop_down_datum_list,
			"and" /* conjunction */ );

	query_dictionary->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_dictionary_system_string(
			query_dictionary->select_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				folder_name ),
			query_dictionary->query_drop_down_datum_list_where );

	query_dictionary->delimited_list =
		list_pipe_fetch(
			query_dictionary->system_string );

	return query_dictionary;
}

QUERY_DICTIONARY *query_dictionary_calloc( void )
{
	QUERY_DICTIONARY *query_dictionary;

	if ( ! ( query_dictionary =
			calloc( 1,
				sizeof ( QUERY_DICTIONARY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_dictionary;
}

char *query_dictionary_select_string(
		LIST *many_folder_primary_key_list )
{
	if ( !list_length( many_folder_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"many_folder_primary_key_list is empty." );

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
	list_display_delimited(
		many_folder_primary_key_list,
		',' );
}

char *query_dictionary_system_string(
		char *query_dictionary_select_string,
		char *folder_table_name,
		char *query_drop_down_datum_list_where )
{
	char system_string[ 1024 ];

	if ( !query_dictionary_select_string
	||   !folder_table_name
	||   !query_drop_down_datum_list_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"select.sh %s %s \"%s\"",
		query_dictionary_select_string,
		folder_table_name,
		query_drop_down_datum_list_where );

	return strdup( system_string );
}

LIST *query_cell_attribute_build_list(
		LIST *key_list,
		LIST *data_list )
{
	LIST *list;
	QUERY_CELL *query_cell;

	if ( !list_length( key_list )
	||   !list_length( data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( key_list ) != list_length( data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"list_length()=%d != list_length()=%d.",
			list_length( key_list ),
			list_length( data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	list_rewind( key_list );
	list_rewind( data_list );

	do {
		list_set(
			list,
			( query_cell = query_cell_calloc() ) );

		query_cell->attribute_name =
			list_get( key_list );

		query_cell->select_datum =
			list_get( data_list );

		list_next( data_list );

	} while ( list_next( key_list ) );

	return list;
}

boolean query_select_skip_boolean(
		char *folder_name,
		char *folder_attribute_folder_name,
		char *attribute_name,
		LIST *no_display_name_list,
		LIST *exclude_lookup_attribute_name_list,
		int primary_key_index,
		char *attribute_not_null )
{
	if ( !folder_name
	||   !folder_attribute_folder_name
	||   !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( attribute_not_null
	&&   strcmp(
		attribute_name,
		attribute_not_null ) == 0 )
	{
		return 0;
	}

	if ( list_string_exists(
		attribute_name,
		no_display_name_list )
	&&   !primary_key_index )
	{
		return 1;
	}

	if ( list_string_exists(
		attribute_name,
		exclude_lookup_attribute_name_list )
	&&   !primary_key_index )
	{
		return 1;
	}

	if ( strcmp(
		folder_attribute_folder_name,
		folder_name ) != 0
	&&   primary_key_index )
	{
		return 1;
	}

	return 0;
}

void query_select_need_but_not_get_boolean(
		boolean *need_but_not_get_boolean,
		char *attribute_not_null,
		char *attribute_name )
{
	if (!need_but_not_get_boolean
	||  !attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !attribute_not_null )
	{
		*need_but_not_get_boolean = 0;
		return;
	}

	if ( strcmp(
		attribute_not_null,
		attribute_name ) == 0 )
	{
		*need_but_not_get_boolean = 0;
	}
}

char *query_where_display(
		char *where_string,
		int max_length )
{
	char where_display[ STRING_WHERE_BUFFER ];

	if ( !where_string
	||   strcmp( where_string, "1 = 1" ) == 0 )
	{
		where_string = "Entire Table";
	}

	if ( strlen( where_string ) + 17 >= STRING_WHERE_BUFFER )
	{
		*where_display = '\0';
	}
	else
	{
		sprintf(where_display,
			"Where: %s",
			where_string );
	}

	if ( max_length
	&&   strlen( where_display ) >= (size_t)(max_length - 3) )
	{
		sprintf(where_display + (max_length - 3),
			"..." );
	}

	return strdup( where_display );
}

char *query_prompt_target_frame(
		char *datatype_name,
		pid_t process_id )
{
	static char frame[ 64 ];

	if ( datatype_name )
	{
		sprintf(frame,
			"%s_%d",
			datatype_name,
			process_id );
	}
	else
	{
		sprintf(frame,
			"frame_%d",
			process_id );
	}

	return frame;
}

DICTIONARY *query_fetch_dictionary(
		LIST *select_name_list,
		char *folder_table_name,
		LIST *primary_key_list,
		LIST *primary_data_list )
{
	LIST *query_drop_down_datum_list;
	char *select;
	QUERY_DROP_DOWN_DATUM *query_drop_down_datum;
	char *where;
	char *system_string;
	DICTIONARY *dictionary;
	char *primary_datum;

	if ( !list_length( select_name_list )
	||   !folder_table_name
	||   !list_rewind( primary_key_list )
	||   !list_rewind( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	list_length( primary_key_list ) !=
		list_length( primary_data_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align [%s] with [%s]",
			list_display( primary_key_list ),
			list_display( primary_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_drop_down_datum_list = list_new();

	select =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		appaserver_select(
			select_name_list
				/* folder_attribute_name_list */ );

	do {
		primary_datum = list_get( primary_data_list );

		if ( !*primary_datum )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"*primary_datum is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		query_drop_down_datum =
			query_drop_down_datum_new(
				(char *)0 /* many_table_name */,
				list_get( primary_key_list )
					/* foreign_key */,
				primary_datum );

		list_set(
			query_drop_down_datum_list,
			query_drop_down_datum );

		list_next( primary_data_list );

	} while ( list_next( primary_key_list ) );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_drop_down_datum_list_where(
			query_drop_down_datum_list,
			"and" /* conjunction */ );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select,
			folder_table_name,
			where );

	dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_name_list_fetch(
			system_string,
			select_name_list,
			SQL_DELIMITER );

	free( select );
	free( where );
	free( system_string );

	return dictionary;
}

char *query_multi_drop_down_column_where(
		LIST *query_multi_drop_down_column_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;
	boolean first_time = 1;
	QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column;
	char *datum_list_where;

	if ( !list_rewind( query_multi_drop_down_column_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_multi_drop_down_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "(" );

	do {
		query_multi_drop_down_column =
			list_get(
				query_multi_drop_down_column_list );

		if ( !list_length(
			query_multi_drop_down_column->
				query_drop_down_datum_list ) )
		{
			char message[ 128 ];

			sprintf(message,
	"query_multi_drop_down_column->query_drop_down_datum_list is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !first_time )
			ptr += sprintf( ptr, " and " );
		else
			first_time = 0;

		datum_list_where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_drop_down_datum_list_where(
				query_multi_drop_down_column->
					query_drop_down_datum_list,
					"or" /* conjunction */ );

		ptr += sprintf(
			ptr,
			"%s",
			datum_list_where );

		free( datum_list_where );

	} while ( list_next( query_multi_drop_down_column_list ) );

	ptr += sprintf( ptr, ")" );

	return strdup( where );
}

QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_new(
		char *many_folder_name,
		char *one_folder_name,
		char *many_table_name,
		LIST *query_multi_drop_down_column_list,
		char *foreign_key,
		char *datum )
{
	QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column;

	if ( !many_folder_name
	||   !one_folder_name
	||   !foreign_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !datum || !*datum )
	{
		char message[ 128 ];

		sprintf(message, "datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_multi_drop_down_column =
		query_multi_drop_down_column_seek(
			one_folder_name,
			foreign_key,
			query_multi_drop_down_column_list );

	if ( query_multi_drop_down_column )
	{
		list_set(
			query_multi_drop_down_column->
				query_drop_down_datum_list,
			query_drop_down_datum_new(
				many_table_name,
				foreign_key,
				datum ) );

		return NULL;
	}

	query_multi_drop_down_column = query_multi_drop_down_column_calloc();

	query_multi_drop_down_column->many_folder_name = many_folder_name;
	query_multi_drop_down_column->one_folder_name = one_folder_name;
	query_multi_drop_down_column->foreign_key = foreign_key;

	query_multi_drop_down_column->
		query_drop_down_datum_list =
			list_new();

	list_set(
		query_multi_drop_down_column->
			query_drop_down_datum_list,
		query_drop_down_datum_new(
			many_table_name,
			foreign_key,
			datum ) );

	return query_multi_drop_down_column;
}

QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_calloc( void )
{
	QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column;

	if ( ! ( query_multi_drop_down_column =
			calloc( 1,
				sizeof ( QUERY_MULTI_DROP_DOWN_COLUMN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_multi_drop_down_column;
}

QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column_seek(
		char *one_folder_name,
		char *foreign_key,
		LIST *query_multi_drop_down_column_list )
{
	QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column;

	if ( !one_folder_name
	||   !foreign_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_multi_drop_down_column_list ) )
		return NULL;

	do {
		query_multi_drop_down_column =
			list_get(
				query_multi_drop_down_column_list );

		if ( strcmp(
			query_multi_drop_down_column->
				one_folder_name,
			one_folder_name ) != 0 )
		{
			continue;
		}

		if ( strcmp(
			query_multi_drop_down_column->
				foreign_key,
			foreign_key ) != 0 )
		{
			continue;
		}

		return query_multi_drop_down_column;

	} while ( list_next( query_multi_drop_down_column_list ) );

	return NULL;
}

QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down_new(
		char *many_folder_name,
		char *one_folder_name,
		char *many_table_name /* optional */,
		DICTIONARY *dictionary,
		LIST *foreign_key_list,
		int highest_index )
{
	QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down;
	int index;
	char *foreign_key;
	char *drop_down_key;
	char *get;
	QUERY_MULTI_DROP_DOWN_COLUMN *query_multi_drop_down_column;

	if ( !many_folder_name
	||   !one_folder_name
	||   !list_length( foreign_key_list )
	||   highest_index < 1 )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_multi_select_drop_down = query_multi_select_drop_down_calloc();

	query_multi_select_drop_down->query_multi_drop_down_column_list =
		list_new();

	for (	index = 1;
		index <= highest_index;
		index++ )
	{
		list_rewind( foreign_key_list );

		do {
			foreign_key = list_get( foreign_key_list );

			drop_down_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				query_multi_select_drop_down_key(
					foreign_key,
					index );

			get =
				dictionary_get(
					drop_down_key,
					dictionary );

			if ( get )
			{
				query_multi_drop_down_column =
				/* -------------------------------------- */
				/* Returns null if exists in list already */
				/* -------------------------------------- */
				query_multi_drop_down_column_new(
					many_folder_name,
					one_folder_name,
					many_table_name,
					query_multi_select_drop_down->
					    query_multi_drop_down_column_list,
					foreign_key,
					get /* datum */ );

				if ( query_multi_drop_down_column )
				{
					list_set(
					    query_multi_select_drop_down->
					      query_multi_drop_down_column_list,
					    query_multi_drop_down_column );
				}
			}

		} while ( list_next( foreign_key_list ) );
	}

	if ( !list_length(
		query_multi_select_drop_down->
			query_multi_drop_down_column_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_multi_drop_down_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_multi_select_drop_down->
		query_multi_drop_down_column_where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_multi_drop_down_column_where(
				query_multi_select_drop_down->
					query_multi_drop_down_column_list );

	return query_multi_select_drop_down;
}

QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down_calloc( void )
{
	QUERY_MULTI_SELECT_DROP_DOWN *query_multi_select_drop_down;

	if ( ! ( query_multi_select_drop_down =
			calloc( 1,
				sizeof ( QUERY_MULTI_SELECT_DROP_DOWN ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_multi_select_drop_down;
}

char *query_multi_select_drop_down_key(
		char *foreign_key,
		int index )
{
	static char key[ 128 ];

	if ( !foreign_key )
	{
		char message[ 128 ];

		sprintf(message, "foreign_key is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(key,
		"%s_%d",
		foreign_key,
		index );

	return key;
}

QUERY_ROW *query_row_seek(
		LIST *row_list,
		char *date_string )
{
	QUERY_ROW *query_row = {0};
	QUERY_CELL *query_cell;

	if ( list_rewind( row_list ) )
	do {
		query_row = list_get( row_list );

		if ( ! ( query_cell =
				/* --------------------------------- */
				/* Returns the first element or null */
				/* --------------------------------- */
				list_first(
					query_row->cell_list ) ) )
		{
			return NULL;
		}

		if ( strcmp( query_cell->select_datum, date_string ) == 0 )
		{
			return query_row;
		}

	} while ( list_next( row_list ) );

	return NULL;
}

QUERY_CELL *query_cell_simple_new(
		char *attribute_name,
		char *select_datum )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	query_cell_new(
		attribute_name,
		select_datum,
		0 /* not primary_key_index */,
		0 /* attribute_is_date */,
		0 /* attribute_is_date_time */,
		0 /* attribute_is_current_date_time */,
		0 /* attribute_is_number */,
		(char *)0 /* display_datum */ );
}

QUERY_CELL *query_cell_new(
		char *attribute_name,
		char *select_datum,
		int primary_key_index,
		boolean attribute_is_date,
		boolean attribute_is_date_time,
		boolean attribute_is_current_date_time,
		boolean attribute_is_number,
		char *display_datum )
{
	QUERY_CELL *query_cell;

	if ( !select_datum )
	{
		char message[ 128 ];

		sprintf(message, "select_datum is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_cell = query_cell_calloc();

	query_cell->attribute_name = attribute_name;
	query_cell->primary_key_index = primary_key_index;
	query_cell->attribute_is_date = attribute_is_date;
	query_cell->attribute_is_date_time = attribute_is_date_time;

	query_cell->attribute_is_current_date_time =
		attribute_is_current_date_time;

	query_cell->attribute_is_number = attribute_is_number;
	query_cell->select_datum = select_datum;
	query_cell->display_datum = display_datum;

	return query_cell;
}

int query_primary_key_isa_list_length( LIST *relation_mto1_isa_list )
{
	return
	list_length( relation_mto1_isa_list );
}

void query_fetch_html_display(
		char *folder_name,
		LIST *query_select_name_list,
		LIST *query_fetch_row_list )
{
	char *system_string;
	FILE *output_pipe;
	QUERY_ROW *query_row;

	if ( !folder_name
	||   !list_length( query_select_name_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

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
		query_fetch_display_system_string(
			folder_name,
			query_select_name_list );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		fprintf(
			output_pipe,
			"%s\n",
			query_row->input );

	} while ( list_next( query_fetch_row_list ) );

	pclose( output_pipe );
}

char *query_fetch_display_system_string(
		char *folder_name,
		LIST *query_select_name_list )
{
	char system_string[ STRING_65K ];
	char destination[ STRING_64K ];
	char *string;

	string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		list_to_string(
			query_select_name_list,
			',' /* delimiter */ );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"html_table.e \"^^%s\" \"%s\" '%c'",
		/* ------------------- */
		/* Returns destination */
		/* ------------------- */
		string_initial_capital(
			destination,
			folder_name ) /* title */,
		string /* heading_list */,
		SQL_DELIMITER );

	return strdup( system_string );
}

char *query_table_edit_where_drop_down_where(
		char *query_drop_down_relation_where,
		char *query_isa_drop_down_list_where )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;

	*ptr = '\0';

	if (	string_strlen( query_drop_down_relation_where ) +
		string_strlen( query_isa_drop_down_list_where ) +
		5 >= STRING_WHERE_BUFFER )
	{
		char message[ 128 ];

		sprintf(message,
			STRING_OVERFLOW_TEMPLATE,
			STRING_WHERE_BUFFER );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( query_drop_down_relation_where )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_relation_where );
	}

	if ( query_isa_drop_down_list_where )
	{
		if ( ptr != where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_isa_drop_down_list_where );
	}

	if ( *where )
		return strdup( where );
	else
		return NULL;
}

QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list_new(
		LIST *relation_mto1_isa_list,
		DICTIONARY *dictionary )
{
	QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list;
	RELATION_MTO1 *relation_mto1;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;

	query_isa_drop_down_list = query_isa_drop_down_list_calloc();
	query_isa_drop_down_list->list = list_new();

	if ( list_rewind( relation_mto1_isa_list ) )
	do {
		relation_mto1 = list_get( relation_mto1_isa_list );

		query_drop_down_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_drop_down_list_new(
				relation_mto1->relation_mto1_list
					/* relation_mto1_to_one_list */,
				dictionary,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				appaserver_table_name(
					relation_mto1->
						one_folder_name ) );

		if ( query_drop_down_list->
			query_drop_down_relation_where )
		{
			list_set(
				query_isa_drop_down_list->list,
				query_drop_down_list );
		}

	} while ( list_next( relation_mto1_isa_list ) );

	if ( list_length( query_isa_drop_down_list->list ) )
	{
		query_isa_drop_down_list->where =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			query_isa_drop_down_list_where(
				query_isa_drop_down_list->list
					/* query_drop_down_list_list */ );
	}
	else
	{
		list_free( query_isa_drop_down_list->list );
		query_isa_drop_down_list->list = NULL;
	}

	return query_isa_drop_down_list;
}

QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list_calloc( void )
{
	QUERY_ISA_DROP_DOWN_LIST *query_isa_drop_down_list;

	if ( ! ( query_isa_drop_down_list =
			calloc( 1,
				sizeof ( QUERY_ISA_DROP_DOWN_LIST ) ) ) )
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

	return query_isa_drop_down_list;
}

char *query_isa_drop_down_list_where( LIST *query_drop_down_list_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr = where;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;

	if ( list_rewind( query_drop_down_list_list ) )
	do {
		query_drop_down_list = list_get( query_drop_down_list_list );

		if (	strlen( where ) +
			strlen( query_drop_down_list->
					query_drop_down_relation_where ) +
			5 >= STRING_WHERE_BUFFER )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_WHERE_BUFFER );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !list_at_first( query_drop_down_list_list ) )
			ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_drop_down_list->
				query_drop_down_relation_where );

	} while ( list_next( query_drop_down_list_list ) );

	return strdup( where );
}

char *query_row_display( QUERY_ROW *query_row )
{
	if ( !query_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_cell_list_display( query_row->cell_list );
}

boolean query_row_list_set_viewonly_boolean(
		LIST *query_fetch_row_list,
		LIST *relation_one2m_list )
{
	QUERY_ROW *query_row;
	boolean viewonly_boolean = 0;

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		if ( query_row_set_viewonly_boolean(
			query_row,
			relation_one2m_list ) )
		{
			viewonly_boolean = 1;
		}

	} while ( list_next( query_fetch_row_list ) );

	return viewonly_boolean;
}

boolean query_row_set_viewonly_boolean(
		QUERY_ROW *query_row,
		LIST *relation_one2m_list )
{
	RELATION_ONE2M *relation_one2m;
	LIST *query_cell_list;
	char *where_string;
	char *system_string;
	boolean viewonly_boolean = 0;

	if ( !query_row )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"query_row is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( relation_one2m_list ) )
	do {
		relation_one2m =
			list_get(
				relation_one2m_list );

		query_cell_list =
			/* ------------------------------- */
			/* Returns query_cell_list or null */
			/* ------------------------------- */
			query_cell_attribute_list(
				relation_one2m->
					one_folder_primary_key_list
						/* attribute_name_list */,
				query_row->cell_list );

		if ( list_length( query_cell_list ) )
		{
			if ( query_cell_list_set_attribute_name(
				relation_one2m->relation_foreign_key_list
					/* attribute_name_list */,
				query_cell_list /* in/out */ ) )
			{
				where_string =
					/* --------------------------- */
					/* Returns heap memory or null */
					/* --------------------------- */
					query_cell_where_string(
						query_cell_list );

				system_string =
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					appaserver_system_string(
						"count(1)" /* select */,
						/* --------------------- */
						/* Returns static memory */
						/* --------------------- */
						appaserver_table_name(
							relation_one2m->
							many_folder_name ),
						where_string );

				query_row->viewonly_boolean =
					(boolean)string_atoi(
						string_pipe_fetch(
							system_string ) );

				if ( query_row->viewonly_boolean )
					viewonly_boolean = 1;

				free( where_string );
				free( system_string );
			}
		}

	} while ( list_next( relation_one2m_list ) );

	return viewonly_boolean;
}

boolean query_cell_list_set_attribute_name(
		LIST *attribute_name_list,
		LIST *query_cell_list /* in/out */ )
{
	QUERY_CELL *query_cell;
	boolean return_boolean = 0;

	if (	list_length( attribute_name_list ) !=
		list_length( query_cell_list ) )
	{
		return return_boolean;
	}

	list_rewind( attribute_name_list );

	if ( list_rewind( query_cell_list ) )
	do {
		query_cell = list_get( query_cell_list );

		query_cell->attribute_name =
			(char *)list_get( attribute_name_list );

		list_next( attribute_name_list );

		return_boolean = 1;

	} while ( list_next( query_cell_list ) );

	return return_boolean;
}

unsigned long query_row_count( char *folder_name )
{
	char *system_string;

	if ( !folder_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_row_count_system_string(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_table_name(
				folder_name ) );

	return
	strtoul(
		string_pipe_fetch(
			system_string ),
		NULL /* endptr */,
		10 /* base */ );
}

char *query_row_count_system_string( char *table_name )
{
	static char system_string[ 128 ];

	if ( !table_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"table_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh 'count(1)' %s",
		table_name );

	return system_string;
}

