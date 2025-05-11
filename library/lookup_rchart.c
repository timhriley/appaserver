/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_rchart.c				*/
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
#include "lookup_rchart.h"

LOOKUP_RCHART_AGGREGATE *lookup_rchart_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		LIST *chart_aggregate_attribute_list )
{
	LOOKUP_RCHART_AGGREGATE *lookup_rchart_aggregate;
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;
	char *title;
	char *filename_key;
	LOOKUP_RCHART_CHART *lookup_rchart_chart;

	if ( !application_name
	||   !session_key
	||   !folder_name
	||   !data_directory
	||   !query_chart )
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

	lookup_rchart_aggregate = lookup_rchart_aggregate_calloc();

	lookup_rchart_aggregate->rchart_chart_list = list_new();

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						week_average_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						week_sum_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						month_average_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						month_sum_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						annual_average_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

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
					RCHART_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_rchart_chart =
				lookup_rchart_chart_new(
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
						number_attribute_name,
					chart_aggregate_attribute->
						annual_sum_row_list,
					title,
					filename_key );

			list_set(
				lookup_rchart_aggregate->rchart_chart_list,
				lookup_rchart_chart );

		} /* if annual_average_row_list */

	} while ( list_next( chart_aggregate_attribute_list ) );

	return lookup_rchart_aggregate;
}

LOOKUP_RCHART_AGGREGATE *lookup_rchart_aggregate_calloc( void )
{
	LOOKUP_RCHART_AGGREGATE *lookup_rchart_aggregate;

	if ( ! ( lookup_rchart_aggregate =
			calloc( 1,
				sizeof ( LOOKUP_RCHART_AGGREGATE ) ) ) )
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

	return lookup_rchart_aggregate;
}

LOOKUP_RCHART_CHART *lookup_rchart_chart_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *sub_title,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_title,
		char *chart_filename_key )
{
	LOOKUP_RCHART_CHART *lookup_rchart_chart;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !sub_title
	||   !date_attribute_name
	||   !number_attribute_name
	||   !query_fetch_row_list
	||   !chart_title
	||   !chart_filename_key )
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

	lookup_rchart_chart = lookup_rchart_chart_calloc();

	lookup_rchart_chart->rchart =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rchart_new(
			application_name,
			session_key,
			data_directory,
			date_attribute_name,
			time_attribute_name,
			number_attribute_name,
			query_fetch_row_list,
			sub_title /* query_table_edit_where_string */,
			chart_title,
			chart_filename_key );

	lookup_rchart_chart->rchart_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rchart_prompt_new(
			lookup_rchart_chart->rchart->rchart_filename,
			chart_filename_key,
			getpid() /* process_id */ );

	return lookup_rchart_chart;
}

LOOKUP_RCHART_CHART *lookup_rchart_chart_calloc( void )
{
	LOOKUP_RCHART_CHART *lookup_rchart_chart;

	if ( ! ( lookup_rchart_chart =
			calloc( 1,
				sizeof ( LOOKUP_RCHART_CHART ) ) ) )
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

	return lookup_rchart_chart;
}

LOOKUP_RCHART *lookup_rchart_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_RCHART *lookup_rchart;
	char *number_attribute_name;
	char *title;
	char *filename_key;
	LOOKUP_RCHART_CHART *lookup_rchart_chart;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !dictionary_string
	||   !data_directory )
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

	lookup_rchart = lookup_rchart_calloc();

	lookup_rchart->chart_input =
		chart_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	if ( !lookup_rchart->chart_input->row_list_length ) return NULL;

	lookup_rchart->lookup_rchart_chart_list = list_new();

	if ( list_rewind(
		lookup_rchart->
			chart_input->
			query_chart->
			number_attribute_name_list ) )
	do {
		number_attribute_name =
			list_get(
				lookup_rchart->
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
				RCHART_FILENAME_STEM,
				number_attribute_name,
				aggregate_statistic_none
					/* enum aggregate_statistic */,
				aggregate_level_none
					/* enum aggregate_level */ );

		lookup_rchart_chart =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_rchart_chart_new(
				application_name,
				session_key,
				data_directory,
				lookup_rchart->
					chart_input->
					query_chart->
					query_table_edit_where->
					string /* sub_title */,
				lookup_rchart->
					chart_input->
					query_chart->
					date_attribute_name,
				lookup_rchart->
					chart_input->
					query_chart->
					time_attribute_name,
				number_attribute_name,
				lookup_rchart->
					chart_input->
					query_chart->
					query_fetch->
					row_list,
				title,
				filename_key );

		list_set(
			lookup_rchart->lookup_rchart_chart_list,
			lookup_rchart_chart );

	} while ( list_next( 
			lookup_rchart->
				chart_input->
				query_chart->
				number_attribute_name_list ) );

	if ( lookup_rchart->chart_input->chart_aggregate_list )
	{
		lookup_rchart->lookup_rchart_aggregate =
			lookup_rchart_aggregate_new(
				application_name,
				session_key,
				folder_name,
				data_directory,
				lookup_rchart->chart_input->query_chart,
				lookup_rchart->
					chart_input->
					chart_aggregate_list->
					chart_aggregate_attribute_list );

		list_set_list(
			lookup_rchart->lookup_rchart_chart_list,
			lookup_rchart->
				lookup_rchart_aggregate->
				rchart_chart_list );
	}

	lookup_rchart->rchart_filename =
		lookup_rchart_filename(
			lookup_rchart->lookup_rchart_chart_list );

	lookup_rchart->rchart_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		rchart_window_new(
			lookup_rchart->rchart_filename,
			lookup_rchart->
				chart_input->
				query_chart->
				query_table_edit_where->
				string /* sub_title */,
			(char *)0 /* filename_key */,
			getpid() /* process_id */ );

	return lookup_rchart;
}

LOOKUP_RCHART *lookup_rchart_calloc( void )
{
	LOOKUP_RCHART *lookup_rchart;

	if ( ! ( lookup_rchart = calloc( 1, sizeof ( LOOKUP_RCHART ) ) ) )
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

	return lookup_rchart;
}

RCHART_FILENAME *lookup_rchart_filename( LIST *lookup_rchart_chart_list )
{
	if ( list_length( lookup_rchart_chart_list ) == 1 )
	{
		LOOKUP_RCHART_CHART *lookup_rchart_chart =
			list_first(
				lookup_rchart_chart_list );

		return
		lookup_rchart_chart->
			rchart->
			rchart_filename;
	}
	else
	{
		return NULL;
	}
}
