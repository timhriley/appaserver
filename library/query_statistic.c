/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/query_statistic.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "query_statistic.h"

QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute_new(
		char sql_delimiter,
		char *attribute_name,
		int value_piece,
		boolean attribute_is_primary_first,
		boolean attribute_is_number,
		boolean attribute_is_date )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( !sql_delimiter
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

	if ( !attribute_is_primary_first
	&&   !attribute_is_number
	&&   !attribute_is_date )
	{
		char message[ 128 ];

		sprintf(message,
"Each of attribute_is_primary_first, attribute_is_number, attribute_is_date are  empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_statistic_fetch_attribute =
		query_statistic_fetch_attribute_calloc();

	query_statistic_fetch_attribute->attribute_name =
		attribute_name;

	query_statistic_fetch_attribute->attribute_is_primary_first =
		attribute_is_primary_first;

	query_statistic_fetch_attribute->attribute_is_number =
		attribute_is_number;

	query_statistic_fetch_attribute->attribute_is_date =
		attribute_is_date;

	if ( attribute_is_number )
	{
		query_statistic_fetch_attribute->statistic =
			statistic_new(
				value_piece,
				-1 /* weight_piece */,
				sql_delimiter );
	}
	else
	if ( attribute_is_date )
	{
		query_statistic_fetch_attribute->statistic_date =
			statistic_date_new(
				value_piece,
				sql_delimiter );
	}

	return query_statistic_fetch_attribute;
}

QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute_calloc( void )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( ! ( query_statistic_fetch_attribute =
			calloc( 1,
				sizeof ( QUERY_STATISTIC_FETCH_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_statistic_fetch_attribute;
}

void query_statistic_fetch_attribute_accumulate(
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		char *input )
{
	if ( !query_statistic_fetch_attribute
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( query_statistic_fetch_attribute->attribute_is_number )
	{
		statistic_accumulate(
			query_statistic_fetch_attribute->
				statistic /* in/out */,
			query_statistic_fetch_attribute->
				statistic->
				statistic_number_array /* in/out */,
			input );
	}
	else
	if ( query_statistic_fetch_attribute->attribute_is_date )
	{
		statistic_date_accumulate(
			query_statistic_fetch_attribute->
				statistic_date /* in/out */,
			input );
	}

	query_statistic_fetch_attribute->row_count++;
}

QUERY_STATISTIC_FETCH *query_statistic_fetch_new(
		char sql_delimiter,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list,
		char *query_system_string )
{
	QUERY_STATISTIC_FETCH *query_statistic_fetch;
	FILE *input_pipe;
	char input[ 1024 ];
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( !sql_delimiter
	||   !primary_key_first
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

	query_statistic_fetch = query_statistic_fetch_calloc();

	query_statistic_fetch->attribute_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_fetch_attribute_list(
			sql_delimiter,
			primary_key_first,
			attribute_number_name_list,
			attribute_date_name_list );

	input_pipe =
		query_statistic_fetch_input_pipe(
			query_system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_rewind( query_statistic_fetch->attribute_list );

		do {
			query_statistic_fetch_attribute =
				list_get(
					query_statistic_fetch->attribute_list );

			query_statistic_fetch_attribute_accumulate(
				query_statistic_fetch_attribute,
				input );

		} while ( list_next( query_statistic_fetch->attribute_list ) );
	}

	pclose( input_pipe );

	query_statistic_fetch_attribute_calculate_set(
		query_statistic_fetch->attribute_list );

	query_statistic_fetch->numeric_attribute_count =
		query_statistic_fetch_numeric_attribute_count(
			list_length( attribute_number_name_list )
				/* attribute_number_name_list_length */ );

	if ( query_statistic_fetch->numeric_attribute_count == 1 )
	{
		query_statistic_fetch->outlier_low_heading_string =
			/* ------------------------------------------- */
			/* Returns component of attribute_list or null */
			/* ------------------------------------------- */
			query_statistic_fetch_outlier_low_heading_string(
				query_statistic_fetch->attribute_list );

		query_statistic_fetch->outlier_high_heading_string =
			/* ------------------------------------------- */
			/* Returns component of attribute_list or null */
			/* ------------------------------------------- */
			query_statistic_fetch_outlier_high_heading_string(
				query_statistic_fetch->attribute_list );
	}

	return query_statistic_fetch;
}

QUERY_STATISTIC_FETCH *query_statistic_fetch_calloc( void )
{
	QUERY_STATISTIC_FETCH *query_statistic_fetch;

	if ( ! ( query_statistic_fetch =
			calloc( 1,
				sizeof ( QUERY_STATISTIC_FETCH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_statistic_fetch;
}

FILE *query_statistic_fetch_input_pipe(
		char *query_system_string )
{
	if ( !query_system_string )
	{
		char message[ 128 ];

		sprintf(message, "query_system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return popen( query_system_string, "r" );
}

LIST *query_statistic_fetch_attribute_list(
		char sql_delimiter,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list )
{
	LIST *attribute_list;
	int value_piece = 0;
	char *attribute_name;

	if ( !sql_delimiter
	||   !primary_key_first )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	attribute_list = list_new();

	list_set(
		attribute_list,
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_fetch_attribute_new(
			sql_delimiter,
			primary_key_first /* attribute_name */,
			value_piece++,
			1 /* attribute_is_primary_first */,
			query_statistic_fetch_first_number_boolean(
				primary_key_first,
				attribute_number_name_list )
					/* attribute_is_number */,
			query_statistic_fetch_first_date_boolean(
				primary_key_first,
				attribute_date_name_list )
					/* attribute_is_date */ ) );

	if ( list_rewind( attribute_number_name_list ) )
	do {
		attribute_name =
			list_get(
				attribute_number_name_list );

		if ( strcmp( attribute_name, primary_key_first ) == 0 )
			continue;

		list_set(
			attribute_list,
			query_statistic_fetch_attribute_new(
				sql_delimiter,
				attribute_name,
				value_piece++,
				0 /* not attribute_is_primary_first */,
				1 /* attribute_is_number */,
				0 /* not attribute_is_date */ ) );

	} while ( list_next( attribute_number_name_list ) );

	if ( list_rewind( attribute_date_name_list ) )
	do {
		attribute_name =
			list_get(
				attribute_date_name_list );

		if ( strcmp( attribute_name, primary_key_first ) == 0 )
			continue;

		list_set(
			attribute_list,
			query_statistic_fetch_attribute_new(
				sql_delimiter,
				attribute_name,
				value_piece++,
				0 /* not attribute_is_primary_first */,
				0 /* not attribute_is_number */,
				1 /* attribute_is_date */ ) );

	} while ( list_next( attribute_date_name_list ) );

	return attribute_list;
}

QUERY_STATISTIC *query_statistic_new(
		char *application_name,
		char *folder_name,
		/* --------------------- */
		/* Static memory or null */
		/* --------------------- */
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list )
{
	QUERY_STATISTIC *query_statistic;

	if ( !application_name
	||   !folder_name
	||   !list_length( folder_attribute_append_isa_list )
	||   !primary_key_first )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_statistic = query_statistic_calloc();

	query_statistic->select_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_statistic_select_list(
			application_name,
			folder_name,
			list_length( relation_mto1_isa_list )
				/* relation_mto1_isa_list_length */,
			primary_key_first,
			attribute_number_name_list,
			attribute_date_name_list );

	query_statistic->query_select_list_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_select_list_string(
			query_statistic->select_list );

	query_statistic->query_select_name_list =
		query_select_name_list(
			query_statistic->select_list );

	query_statistic->query_from_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_from_string(
			folder_name,
			relation_mto1_isa_list,
			(char *)0 /* row_security_role_update_list_from */ );	

	query_statistic->query_table_edit_where =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
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

	query_statistic->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			query_statistic->query_select_list_string,
			query_statistic->query_from_string,
			query_statistic->query_table_edit_where->string,
			(char *)0 /* group_by_string */,
			(char *)0 /* order_by_string */,
			0 /* max_rows */ );

	return query_statistic;
}

QUERY_STATISTIC *query_statistic_calloc( void )
{
	QUERY_STATISTIC *query_statistic;

	if ( ! ( query_statistic = calloc( 1, sizeof ( QUERY_STATISTIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_statistic;
}

LIST *query_statistic_select_list(
		char *application_name,
		char *folder_name,
		int relation_mto1_isa_list_length,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list )
{
	LIST *select_list;
	char *attribute_name;

	if ( !application_name
	||   !folder_name
	||   !primary_key_first )
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
		query_select_new(
			application_name,
			relation_mto1_isa_list_length,
			(char *)0 /* attribute_not_null */,
			folder_name,
			primary_key_first /* attribute_name */ ) );

	if ( list_rewind( attribute_number_name_list ) )
	do {
		attribute_name =
			list_get(
				attribute_number_name_list );

		list_set(
			select_list,
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				(char *)0 /* attribute_not_null */,
				folder_name,
				attribute_name ) );

	} while ( list_next( attribute_number_name_list ) );

	if ( list_rewind( attribute_date_name_list ) )
	do {
		attribute_name =
			list_get(
				attribute_date_name_list );

		list_set(
			select_list,
			query_select_new(
				application_name,
				relation_mto1_isa_list_length,
				(char *)0 /* attribute_not_null */,
				folder_name,
				attribute_name ) );

	} while ( list_next( attribute_date_name_list ) );

	return select_list;
}

void query_statistic_fetch_attribute_calculate_set( LIST *attribute_list )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get( attribute_list );

		if ( attribute->statistic )
		{
			attribute->statistic->statistic_calculate =
			    /* -------------- */
			    /* Safely returns */
			    /* -------------- */
			    statistic_calculate_new(
				attribute->
					statistic->
					statistic_number_array,
				attribute->statistic->weight_count,
				attribute->statistic->sum,
				attribute->statistic->row_count,
				attribute->statistic->value_count,
				attribute->statistic->non_zero_count );

			statistic_number_array_sort(
				attribute->
					statistic->
					statistic_number_array /* in/out */ );

			attribute->statistic->statistic_quartile =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				statistic_quartile_new(
					attribute->
						statistic->
						statistic_number_array );
		}
	} while ( list_next( attribute_list ) );
}

int query_statistic_fetch_numeric_attribute_count(
		int attribute_number_name_list_length )
{
	return attribute_number_name_list_length;
}

char *query_statistic_fetch_outlier_low_heading_string(
		LIST *attribute_list )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( list_rewind( attribute_list ) )
	do {
		query_statistic_fetch_attribute =
			list_get(
				attribute_list );

		if ( !query_statistic_fetch_attribute->statistic
		||   !query_statistic_fetch_attribute->
			statistic->
			statistic_quartile )
		{
			continue;
		}

		if ( query_statistic_fetch_attribute->
			statistic->
			statistic_quartile->
			outlier_low_heading_string )
		{
			return
			query_statistic_fetch_attribute->
				statistic->
				statistic_quartile->
				outlier_low_heading_string;
		}

	} while ( list_next( attribute_list ) );

	return NULL;
}

char *query_statistic_fetch_outlier_high_heading_string(
		LIST *attribute_list )
{
	QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute;

	if ( list_rewind( attribute_list ) )
	do {
		query_statistic_fetch_attribute =
			list_get(
				attribute_list );

		if ( !query_statistic_fetch_attribute->statistic
		||   !query_statistic_fetch_attribute->
			statistic->
			statistic_quartile )
		{
			continue;
		}

		if ( query_statistic_fetch_attribute->
			statistic->
			statistic_quartile->
			outlier_high_heading_string )
		{
			return
			query_statistic_fetch_attribute->
				statistic->
				statistic_quartile->
				outlier_high_heading_string;
		}

	} while ( list_next( attribute_list ) );

	return NULL;
}

boolean query_statistic_fetch_first_number_boolean(
		char *primary_key_first,
		LIST *attribute_number_name_list )
{
	return
	list_string_exists(
		primary_key_first /* string */,
		attribute_number_name_list );
}

boolean query_statistic_fetch_first_date_boolean(
		char *primary_key_first,
		LIST *attribute_date_name_list )
{
	return
	list_string_exists(
		primary_key_first /* string */,
		attribute_date_name_list );
}

