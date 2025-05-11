/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/chart.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver.h"
#include "application.h"
#include "document.h"
#include "relation_mto1.h"
#include "folder_attribute.h"
#include "appaserver_error.h"
#include "spool.h"
#include "sql.h"
#include "piece.h"
#include "date_convert.h"
#include "chart.h"

CHART_INPUT *chart_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string )
{
	CHART_INPUT *chart_input;

	if ( !application_name
	||   !login_name
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

	chart_input = chart_input_calloc();

	chart_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	chart_input->role_attribute_exclude_lookup_name_list =
		role_attribute_exclude_lookup_name_list(
			ROLE_PERMISSION_LOOKUP,
			ROLE_PERMISSION_UPDATE,
			chart_input->role->role_attribute_exclude_list );

	chart_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			chart_input->
				role_attribute_exclude_lookup_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	chart_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			chart_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	chart_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			chart_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	chart_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			chart_input->folder->folder_attribute_list,
			chart_input->relation_mto1_isa_list );

	chart_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			chart_input->folder_attribute_append_isa_list );

	chart_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			chart_input->folder_attribute_append_isa_list );

	chart_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	chart_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			chart_input->
				folder_row_level_restriction->
				non_owner_forbid,
			chart_input->role->override_row_restrictions );

	chart_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			dictionary_string );

	chart_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_lookup_new(
			chart_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			chart_input->
				relation_mto1_list,
			chart_input->
				folder_attribute_name_list,
			chart_input->
				folder_attribute_date_name_list,
			chart_input->
				folder_attribute_append_isa_list );

	chart_input->query_chart =
		query_chart_new(
			application_name,
			login_name,
			folder_name,
			chart_input->
				security_entity->
				where,
			chart_input->
				dictionary_separate->
				non_prefixed_dictionary
					/* query_dictionary */,
			chart_input->
				dictionary_separate->
				no_display_name_list,
			chart_input->
				folder_attribute_append_isa_list,
			chart_input->
				relation_mto1_list,
			chart_input->
				relation_mto1_isa_list );

	if ( !chart_input->query_chart )
	{
		char message[ 128 ];

		sprintf(message, "Insufficient input to generate a chart." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	chart_input->row_list_length =
		chart_input_row_list_length(
			chart_input->
				query_chart->
				query_fetch->
				row_list );

	if ( !chart_input->row_list_length ) return chart_input;

	chart_input->query_row_list_days_range =
		query_row_list_days_range(
			chart_input->
				query_chart->
				date_attribute_name,
			chart_input->
				query_chart->
				query_fetch->
				row_list );

	if ( chart_input->query_row_list_days_range >= 180 )
	{
		chart_input->chart_aggregate_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_list_new(
				chart_input->
					query_chart->
					date_attribute_name,
				chart_input->
					query_chart->
					time_attribute_name,
				chart_input->
					query_chart->
					query_select_name_list,
				chart_input->
					query_chart->
					query_fetch->
					row_list,
				chart_input->query_row_list_days_range );
	}

	return chart_input;
}

CHART_INPUT *chart_input_calloc( void )
{
	CHART_INPUT *chart_input;

	if ( ! ( chart_input = calloc( 1, sizeof ( CHART_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return chart_input;
}

int chart_input_row_list_length( LIST *query_fetch_row_list )
{
	return
	list_length( query_fetch_row_list );
}

LIST *chart_aggregate_fetch_list(
		LIST *query_fetch_row_list,
		char *attribute_name,
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level )
{
	LIST *fetch_list;
	char *system_string;
	char *output_string;
	SPOOL *spool;
	QUERY_ROW *query_row;
	LIST *list;
	char *input_string;
	CHART_AGGREGATE_FETCH *chart_aggregate_fetch;

	if ( !list_rewind( query_fetch_row_list )
	||   !attribute_name
	||   !chart_aggregate_list_end_date )
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
		chart_aggregate_fetch_system_string(
			chart_aggregate_list_end_date,
			aggregate_statistic,
			aggregate_level );

	spool =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spool_new(
			system_string,
			0 /* not capture_stderr_boolean */ );

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

		output_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_aggregate_fetch_output_string(
				attribute_name,
				query_row->cell_list );

		fprintf(spool->output_pipe,
			"%s\n",
			output_string );

	} while ( list_next( query_fetch_row_list ) );

	pclose( spool->output_pipe );

	list = spool_list( spool->output_filename );

	fetch_list = list_new();

	if ( list_rewind( list ) )
	do {
		input_string = list_get( list );

		chart_aggregate_fetch =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_fetch_parse(
				input_string );

		list_set( fetch_list, chart_aggregate_fetch );

	} while ( list_next( list ) );

	return fetch_list;
}

CHART_AGGREGATE_FETCH *chart_aggregate_fetch_parse(
		char *input_string )
{
	char date_string[ 128 ];
	char value_string[ 128 ];

	if ( !input_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( date_string, SQL_DELIMITER, input_string, 0 );
	piece( value_string, SQL_DELIMITER, input_string, 1 );

	if ( !*value_string )
	{
		char message[ 128 ];

		sprintf(message,
			"delimiter=[%c] is empty in [%s].",
			SQL_DELIMITER,
			input_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	chart_aggregate_fetch_new(
		strdup( date_string ),
		strdup( value_string ) );
}

CHART_AGGREGATE_FETCH *chart_aggregate_fetch_new(
		char *date_string,
		char *value_string )
{
	CHART_AGGREGATE_FETCH *chart_aggregate_fetch;

	if ( !date_string
	||   !value_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	chart_aggregate_fetch = chart_aggregate_fetch_calloc();

	chart_aggregate_fetch->date_string = date_string;
	chart_aggregate_fetch->value_string = value_string;

	return chart_aggregate_fetch;
}

CHART_AGGREGATE_FETCH *chart_aggregate_fetch_calloc( void )
{
	CHART_AGGREGATE_FETCH *chart_aggregate_fetch;

	if ( ! ( chart_aggregate_fetch =
			calloc( 1,
				sizeof ( CHART_AGGREGATE_FETCH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return chart_aggregate_fetch;
}

char *chart_aggregate_fetch_system_string(
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level )
{
	char system_string[ 1024 ];

	if ( !chart_aggregate_list_end_date )
	{
		char message[ 128 ];

		sprintf(message, "chart_aggregate_list_end_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(system_string,
		"real_time2aggregate_value.e %s 0 -1 1 '%c' %s n '%s'",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		aggregate_statistic_string( aggregate_statistic ),
		SQL_DELIMITER,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		aggregate_level_string( aggregate_level ),
		chart_aggregate_list_end_date );

	return strdup( system_string );
}

char *chart_aggregate_fetch_output_string(
		char *attribute_name,
		LIST *cell_list )
{
	QUERY_CELL *first_query_cell;
	char *date_string;
	QUERY_CELL *query_cell;
	char *value_string;
	static char output_string[ 128 ];

	if ( !attribute_name
	||   !list_length( cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	first_query_cell = list_first( cell_list );

	date_string = first_query_cell->select_datum;

	if ( ! ( query_cell =
			query_cell_seek(
				attribute_name,
				cell_list ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"query_cell_seek(%s) returned empty.",
			attribute_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	value_string = query_cell->select_datum;

	sprintf(output_string,
		"%s%c%s",
		date_string,
		SQL_DELIMITER,
		value_string );

	return output_string;
}

CHART_AGGREGATE *chart_aggregate_new(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level )
{
	CHART_AGGREGATE *chart_aggregate;
	LIST *fetch_list;

	if ( !date_attribute_name
	||   !number_attribute_name
	||   !chart_aggregate_list_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	chart_aggregate = chart_aggregate_calloc();

	fetch_list =
		chart_aggregate_fetch_list(
			query_fetch_row_list,
			number_attribute_name,
			chart_aggregate_list_end_date,
			aggregate_statistic,
			aggregate_level );

	chart_aggregate->query_row_list =
		chart_aggregate_query_row_list(
			date_attribute_name,
			number_attribute_name,
			fetch_list );

	return chart_aggregate;
}

CHART_AGGREGATE *chart_aggregate_calloc( void )
{
	CHART_AGGREGATE *chart_aggregate;

	if ( ! ( chart_aggregate = calloc( 1, sizeof ( CHART_AGGREGATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return chart_aggregate;
}

LIST *chart_aggregate_query_row_list(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *chart_aggregate_fetch_list )
{
	CHART_AGGREGATE_FETCH *chart_aggregate_fetch;
	QUERY_ROW *query_row;
	LIST *query_row_list;
	LIST *query_cell_list;

	query_row_list = list_new();

	if ( list_rewind( chart_aggregate_fetch_list ) )
	do {
		chart_aggregate_fetch =
			list_get(
				chart_aggregate_fetch_list );

		query_cell_list =
			chart_aggregate_query_cell_list(
				date_attribute_name,
				number_attribute_name,
				chart_aggregate_fetch->date_string,
				chart_aggregate_fetch->value_string );

		query_row =
			query_row_new(
				0 /* not input_save_boolean */,
				(char *)0 /* input */,
				query_cell_list );

		list_set( query_row_list, query_row );

	} while ( list_next( chart_aggregate_fetch_list ) );

	return query_row_list;
}

LIST *chart_aggregate_query_cell_list(
		char *date_attribute_name,
		char *number_attribute_name,
		char *date_string,
		char *value_string )
{
	LIST *query_cell_list;

	if ( !date_attribute_name
	||   !number_attribute_name
	||   !date_string
	||   !value_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_cell_list = list_new();

	list_set(
		query_cell_list,
		query_cell_new(
			date_attribute_name,
			date_string /* select_datum */,
			0 /* primary_key_index */,
			1 /* attribute_is_date */,
			0 /* not attribute_is_date_time */,
			0 /* not attribute_is_current_date_time */,
			0 /* not attribute_is_number */,
			(char *)0 /* display_datum */ ) );

	list_set(
		query_cell_list,
		query_cell_new(
			number_attribute_name,
			value_string /* select_datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_date */,
			0 /* not attribute_is_date_time */,
			0 /* not attribute_is_current_date_time */,
			1 /* attribute_is_number */,
			(char *)0 /* display_datum */ ) );

	return query_cell_list;
}

CHART_AGGREGATE_LIST *chart_aggregate_list_new(
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_select_name_list,
		LIST *query_fetch_row_list,
		int query_row_list_days_range )
{
	CHART_AGGREGATE_LIST *chart_aggregate_list;
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;
	char *number_attribute_name;

	if ( !date_attribute_name
	||   !list_rewind( query_select_name_list )
	||   !list_length( query_fetch_row_list )
	||   !query_row_list_days_range )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	chart_aggregate_list = chart_aggregate_list_calloc();

	chart_aggregate_list->end_date =
		/* ------------------------------------ */
		/* Returns a component of the parameter */
		/* ------------------------------------ */
		chart_aggregate_list_end_date(
			query_fetch_row_list );

	/* Skip date */
	/* --------- */
	list_next( query_select_name_list );

	/* Skip time */
	/* --------- */
	if ( time_attribute_name ) list_next( query_select_name_list );

	chart_aggregate_list->chart_aggregate_attribute_list = list_new();

	do {
		number_attribute_name =
			list_get(
				query_select_name_list );

		chart_aggregate_attribute =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_attribute_new(
				date_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				query_row_list_days_range,
				chart_aggregate_list->end_date );

		list_set(
			chart_aggregate_list->chart_aggregate_attribute_list,
			chart_aggregate_attribute );

	} while ( list_next( query_select_name_list ) );

	return chart_aggregate_list;
}

CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute_new(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		int query_row_list_days_range,
		char *chart_aggregate_list_end_date )
{
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;
	CHART_AGGREGATE *chart_aggregate;

	if ( !date_attribute_name
	||   !number_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !query_row_list_days_range
	||   !chart_aggregate_list_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	chart_aggregate_attribute = chart_aggregate_attribute_calloc();

	chart_aggregate_attribute->number_attribute_name =
		number_attribute_name;

	if ( query_row_list_days_range > 1825 )
	{
		chart_aggregate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_new(
				date_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				chart_aggregate_list_end_date,
				average /* enum aggregate_statistic */,
				annually /* enum aggregate_level */ );

		chart_aggregate_attribute->annual_average_row_list =
			chart_aggregate->query_row_list;

		chart_aggregate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_new(
				date_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				chart_aggregate_list_end_date,
				sum /* enum aggregate_statistic */,
				annually /* enum aggregate_level */ );

		chart_aggregate_attribute->annual_sum_row_list =
			chart_aggregate->query_row_list;
	}

	if ( query_row_list_days_range > 730 )
	{
		chart_aggregate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_new(
				date_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				chart_aggregate_list_end_date,
				average /* enum aggregate_statistic */,
				monthly /* enum aggregate_level */ );

		chart_aggregate_attribute->month_average_row_list =
			chart_aggregate->query_row_list;

		chart_aggregate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			chart_aggregate_new(
				date_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				chart_aggregate_list_end_date,
				sum /* enum aggregate_statistic */,
				monthly /* enum aggregate_level */ );

		chart_aggregate_attribute->month_sum_row_list =
			chart_aggregate->query_row_list;
	}

	chart_aggregate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		chart_aggregate_new(
			date_attribute_name,
			number_attribute_name,
			query_fetch_row_list,
			chart_aggregate_list_end_date,
			average /* enum aggregate_statistic */,
			weekly /* enum aggregate_level */ );

	chart_aggregate_attribute->week_average_row_list =
		chart_aggregate->query_row_list;

	chart_aggregate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		chart_aggregate_new(
			date_attribute_name,
			number_attribute_name,
			query_fetch_row_list,
			chart_aggregate_list_end_date,
			sum /* enum aggregate_statistic */,
			weekly /* enum aggregate_level */ );

	chart_aggregate_attribute->week_sum_row_list =
		chart_aggregate->query_row_list;

	return chart_aggregate_attribute;
}

CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute_calloc( void )
{
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;

	if ( ! ( chart_aggregate_attribute =
			calloc( 1,
				sizeof ( CHART_AGGREGATE_ATTRIBUTE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return chart_aggregate_attribute;
}

CHART_AGGREGATE_LIST *chart_aggregate_list_calloc( void )
{
	CHART_AGGREGATE_LIST *chart_aggregate_list;

	if ( ! ( chart_aggregate_list =
			calloc( 1,
				sizeof ( CHART_AGGREGATE_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return chart_aggregate_list;
}

char *chart_aggregate_list_end_date( LIST *query_fetch_row_list )
{
	QUERY_ROW *query_row;
	QUERY_CELL *query_cell;

	if ( ! ( query_row =
			list_last(
				query_fetch_row_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "list_last() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( query_cell =
			list_first(
				query_row->cell_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "list_first() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !date_convert_is_valid_international(
		query_cell->select_datum ) )
	{
		char message[ 128 ];

		sprintf(message,
		"date_convert_is_valid_international(%s) returned empty.",
			query_cell->select_datum );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return query_cell->select_datum;
}

char *chart_title(
		char *folder_name,
		char *number_attribute_name,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level,
		char *where_string )
{
	char title[ 65536 ];

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

	if ( !number_attribute_name )
	{
		strcpy( title, folder_name );
	}
	else
	{
		sprintf(title,
			"%s -- %s",
			folder_name,
			number_attribute_name );
	}

	if ( aggregate_statistic != aggregate_statistic_none )
	{
		sprintf(title + strlen( title ),
			" %s %s",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			aggregate_level_string(
				aggregate_level ),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			aggregate_statistic_string(
				aggregate_statistic ) );
	}

	string_initial_capital( title, title );

	if ( where_string )
	{
		if ( strlen( where_string ) + strlen( title ) < ( 65536 - 2 ) )
		{
			sprintf(title + strlen( title ),
				": %s",
				where_string );
		}
	}

	return strdup( title );
}

char *chart_filename_key(
		char *filename_stem,
		char *number_attribute_name,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level )
{
	static char filename_key[ 128 ];

	if ( !filename_stem
	||   !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		sprintf(filename_key,
			"%s_%s",
			filename_stem,
			number_attribute_name );
	}
	else
	{
		sprintf(filename_key,
			"%s_%s_%s_%s",
			filename_stem,
			number_attribute_name,
			aggregate_level_string( aggregate_level ),
			aggregate_statistic_string( aggregate_statistic ) );
	}

	return filename_key;
}

char *chart_window_sub_title_display( char *sub_title )
{
	char display[ STRING_WHERE_BUFFER ];

	if ( !sub_title
	||   strcmp( sub_title, "1 = 1" ) == 0 )
	{
		sub_title = "Entire Table";
	}

	if ( strlen( sub_title ) >= STRING_WHERE_BUFFER - 32 )
	{
		*display = '\0';
	}
	else
	{
		snprintf(
			display,
			sizeof ( display ),
			"<h2>Where: %s</h2>",
			sub_title );
	}

	return strdup( display );
}

char *chart_window_html(
	char *document_body_onload_open_tag,
	char *screen_title,
	char *sub_title_display )
{
	char html[ STRING_WHERE_BUFFER ];

	if ( !document_body_onload_open_tag
	||   !screen_title
	||   !sub_title_display )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	strlen( document_body_onload_open_tag ) +
		strlen( screen_title ) +
		strlen( sub_title_display ) +
		3 >= STRING_WHERE_BUFFER )
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

	snprintf(
		html,
		sizeof ( html ),
		"%s\n%s\n%s",
		document_body_onload_open_tag,
		screen_title,
		sub_title_display );

	return strdup( html );
}

char *chart_window_onload_string(
		char *prompt_filename,
		char *target )
{
	static char onload_string[ 256 ];

	if ( !prompt_filename
	||   !target )
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
		onload_string,
		sizeof ( onload_string ),
		APPASERVER_WINDOW_OPEN_TEMPLATE,
		prompt_filename,
		target );

	return onload_string;
}

