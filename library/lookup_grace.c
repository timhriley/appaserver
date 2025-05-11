/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_grace.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "String.h"
#include "appaserver.h"
#include "application.h"
#include "appaserver_error.h"
#include "chart.h"
#include "lookup_grace.h"

LOOKUP_GRACE *lookup_grace_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_GRACE *lookup_grace;
	char *number_attribute_name;
	char *title;
	char *filename_key;
	LOOKUP_GRACE_GRAPH *lookup_grace_graph;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !dictionary_string
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_grace = lookup_grace_calloc();

	lookup_grace->chart_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		chart_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	if ( !lookup_grace->chart_input->row_list_length ) return NULL;

	lookup_grace->lookup_grace_graph_list = list_new();

	if ( list_rewind(
		lookup_grace->
			chart_input->
			query_chart->
			number_attribute_name_list ) )
	do {
		number_attribute_name =
			list_get(
				lookup_grace->
					chart_input->
					query_chart->
					number_attribute_name_list );

		title =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			chart_title(
				folder_name,
				number_attribute_name,
				aggregate_statistic_none
					/* enum aggregate_statistic */,
				aggregate_level_none
					/* enum aggregate_level */,
				(char *)0 /* where_string */ );

		filename_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_filename_key(
				GRACE_FILENAME_STEM,
				number_attribute_name,
				aggregate_statistic_none
					/* enum aggregate_statistic */,
				aggregate_level_none
					/* enum aggregate_level */ );

		lookup_grace_graph =
			lookup_grace_graph_new(
				application_name,
				session_key,
				data_directory,
				lookup_grace->
					chart_input->
					query_chart->
					query_table_edit_where->
					string /* sub_title */,
				lookup_grace->
					chart_input->
					query_chart->
					date_attribute_name,
				lookup_grace->
					chart_input->
					query_chart->
					time_attribute_name,
				lookup_grace->
					chart_input->
					query_chart->
					query_fetch->
					row_list,
				lookup_grace->
					chart_input->
					query_row_list_days_range
					/* horizontal_range */,
				title,
				number_attribute_name
					/* grace_datatype_name */,
				filename_key );

		list_set(
			lookup_grace->lookup_grace_graph_list,
			lookup_grace_graph );

	} while ( list_next( 
			lookup_grace->
				chart_input->
				query_chart->
				number_attribute_name_list ) );

	if ( lookup_grace->chart_input->chart_aggregate_list )
	{
		lookup_grace->lookup_grace_aggregate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_grace_aggregate_new(
				application_name,
				session_key,
				folder_name,
				data_directory,
				lookup_grace->
					chart_input->
					query_chart,
				lookup_grace->
					chart_input->
					query_row_list_days_range,
				lookup_grace->
					chart_input->
					chart_aggregate_list->
					chart_aggregate_attribute_list );

		list_set_list(
			lookup_grace->lookup_grace_graph_list,
			lookup_grace->
				lookup_grace_aggregate->
				grace_graph_list );
	}

	lookup_grace->grace_filename =
		lookup_grace_filename(
			lookup_grace->lookup_grace_graph_list );

	lookup_grace->grace_window =
		grace_window_new(
			lookup_grace->grace_filename,
			lookup_grace->
				chart_input->
				query_chart->
				query_table_edit_where->
				string /* sub_title */,
			(char *)0 /* grace_datatype_name */,
			getpid() /* process_id */ );

	return lookup_grace;
}

LOOKUP_GRACE *lookup_grace_calloc( void )
{
	LOOKUP_GRACE *lookup_grace;

	if ( ! ( lookup_grace = calloc( 1, sizeof ( LOOKUP_GRACE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_grace;
}

LOOKUP_GRACE_GRAPH *lookup_grace_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *sub_title,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		int horizontal_range_integer,
		char *chart_title,
		char *grace_datatype_name,
		char *filename_key )
{
	LOOKUP_GRACE_GRAPH *lookup_grace_graph;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !date_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !horizontal_range_integer
	||   !chart_title
	||   !grace_datatype_name
	||   !filename_key )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_grace_graph = lookup_grace_graph_calloc();

	lookup_grace_graph->grace_unitless =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_unitless_new(
			application_name,
			session_key,
			data_directory,
			sub_title,
			date_attribute_name,
			time_attribute_name,
			query_fetch_row_list,
			horizontal_range_integer,
			chart_title,
			grace_datatype_name,
			filename_key );

	lookup_grace_graph->grace_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_prompt_new(
			lookup_grace_graph->
				grace_unitless->
				grace_setup->
				grace_filename,
			filename_key,
			getpid() /* process_id */ );

	return lookup_grace_graph;
}

LOOKUP_GRACE_GRAPH *lookup_grace_graph_calloc( void )
{
	LOOKUP_GRACE_GRAPH *lookup_grace_graph;

	if ( ! ( lookup_grace_graph =
			calloc( 1,
				sizeof ( LOOKUP_GRACE_GRAPH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_grace_graph;
}

GRACE_FILENAME *lookup_grace_filename( LIST *lookup_grace_graph_list )
{
	if ( list_length( lookup_grace_graph_list ) == 1 )
	{
		LOOKUP_GRACE_GRAPH *lookup_grace_graph =
			list_first(
				lookup_grace_graph_list );

		return
		lookup_grace_graph->
			grace_unitless->
			grace_setup->
			grace_filename;
	}
	else
	{
		return NULL;
	}
}

LOOKUP_GRACE_AGGREGATE *lookup_grace_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		int query_row_list_days_range,
		LIST *chart_aggregate_attribute_list )
{
	LOOKUP_GRACE_AGGREGATE *lookup_grace_aggregate;
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;
	char *title;
	char *filename_key;
	LOOKUP_GRACE_GRAPH *lookup_grace_graph;

	lookup_grace_aggregate = lookup_grace_aggregate_calloc();

	lookup_grace_aggregate->grace_graph_list = list_new();

	if ( list_rewind( chart_aggregate_attribute_list ) )
	do {
		chart_aggregate_attribute =
			list_get(
				chart_aggregate_attribute_list );

		if ( list_length(
				chart_aggregate_attribute->
					week_average_row_list ) )
		{
			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						week_average_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						week_sum_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

		} /* if week_average_row_list */

		if ( list_length(
				chart_aggregate_attribute->
					month_average_row_list ) )
		{
			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						month_average_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						month_sum_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

		} /* if month_average_row_list */

		if ( list_length(
				chart_aggregate_attribute->
					annual_average_row_list ) )
		{
			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					annually /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						annual_average_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					annually /* enum aggregate_level */,
					(char *)0 /* where_string */ );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GRACE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_grace_graph =
				lookup_grace_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						query_table_edit_where->
						string /* sub_title */,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						annual_sum_row_list,
					query_row_list_days_range
						/* horizontal_range */,
					title,
					chart_aggregate_attribute->
						number_attribute_name
						/* grace_datatype_name */,
					filename_key );

			list_set(
				lookup_grace_aggregate->grace_graph_list,
				lookup_grace_graph );

		} /* if annual_average_row_list */

	} while ( list_next( chart_aggregate_attribute_list ) );

	return lookup_grace_aggregate;
}

LOOKUP_GRACE_AGGREGATE *lookup_grace_aggregate_calloc( void )
{
	LOOKUP_GRACE_AGGREGATE *lookup_grace_aggregate;

	if ( ! ( lookup_grace_aggregate =
			calloc( 1,
				sizeof ( LOOKUP_GRACE_AGGREGATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_grace_aggregate;
}

