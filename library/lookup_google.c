/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_google.c				*/
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
#include "document.h"
#include "appaserver_error.h"
#include "chart.h"
#include "lookup_google.h"

LOOKUP_GOOGLE *lookup_google_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_GOOGLE *lookup_google;
	char *number_attribute_name;
	LOOKUP_GOOGLE_GRAPH *lookup_google_graph;
	int chart_number = 0;
	char *title;
	char *filename_key;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
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

	lookup_google = lookup_google_calloc();

	lookup_google->chart_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		chart_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	if ( !lookup_google->chart_input->row_list_length )
		return lookup_google;

	lookup_google->lookup_google_graph_list = list_new();

	if ( list_rewind(
			lookup_google->
				chart_input->
				query_chart->
				number_attribute_name_list ) )
	do {
		number_attribute_name =
			list_get(
				lookup_google->
					chart_input->
					query_chart->
					number_attribute_name_list );

		title =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			chart_title(
				folder_name,
				(char *)0 /* number_attribute_name */,
				aggregate_statistic_none,
				aggregate_level_none,
				lookup_google->
					chart_input->
					query_chart->
					query_table_edit_where->
					string /* where_string */ );

		filename_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			chart_filename_key(
				GOOGLE_FILENAME_STEM,
				number_attribute_name,
				aggregate_statistic_none,
				aggregate_level_none );

		lookup_google_graph =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			lookup_google_graph_new(
				application_name,
				session_key,
				data_directory,
				lookup_google->
					chart_input->
					query_chart->
					date_attribute_name,
				lookup_google->
					chart_input->
					query_chart->
					time_attribute_name,
				lookup_google->
					chart_input->
					query_chart->
					query_fetch->
					row_list,
				number_attribute_name,
				title,
				filename_key,
				++chart_number );

		list_set(
			lookup_google->lookup_google_graph_list,
			lookup_google_graph );

	} while ( list_next(
			lookup_google->
				chart_input->
				query_chart->
				number_attribute_name_list ) );

	if ( lookup_google->chart_input->chart_aggregate_list )
	{
		lookup_google->lookup_google_aggregate =
			lookup_google_aggregate_new(
				application_name,
				session_key,
				folder_name,
				data_directory,
				lookup_google->
					chart_input->
					query_chart,
				lookup_google->
					chart_input->
					chart_aggregate_list->
					chart_aggregate_attribute_list,
				chart_number );

		list_set_list(
			lookup_google->lookup_google_graph_list,
			lookup_google->lookup_google_aggregate->
				google_graph_list );
	}

	lookup_google->google_filename =
		lookup_google_filename(
			lookup_google->lookup_google_graph_list );

	lookup_google->google_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_window_new(
			lookup_google->google_filename,
			lookup_google->
				chart_input->
				query_chart->
				query_table_edit_where->
				string,
			getpid() /* process_id */ );

	return lookup_google;
}

LOOKUP_GOOGLE *lookup_google_calloc( void )
{
	LOOKUP_GOOGLE *lookup_google;

	if ( ! ( lookup_google = calloc( 1, sizeof ( LOOKUP_GOOGLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_google;
}

LOOKUP_GOOGLE_GRAPH *lookup_google_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		char *number_attribute_name,
		char *chart_title,
		char *chart_filename_key,
		int chart_number )
{
	LOOKUP_GOOGLE_GRAPH *lookup_google_graph;

	if ( !application_name
	||   !session_key
	||   !data_directory
	||   !date_attribute_name
	||   !list_length( query_fetch_row_list )
	||   !number_attribute_name
	||   !chart_title
	||   !chart_filename_key
	||   !chart_number )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_google_graph = lookup_google_graph_calloc();

	if ( ! ( lookup_google_graph->google_graph =
			google_graph_new(
				application_name,
				session_key,
				data_directory,
				date_attribute_name,
				time_attribute_name,
				number_attribute_name,
				query_fetch_row_list,
				chart_title,
				chart_filename_key,
				chart_number,
				"ColumnChart"
					/* google_graph_function */ ) ) )
	{
		char message[ 128 ];

		sprintf(message, "google_lookup_new() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_google_graph->google_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_prompt_new(
			lookup_google_graph->
				google_graph->
				google_filename,
			chart_filename_key,
			getpid() /* process_id */ );

	return lookup_google_graph;
}

LOOKUP_GOOGLE_GRAPH *lookup_google_graph_calloc( void )
{
	LOOKUP_GOOGLE_GRAPH *lookup_google_graph;

	if ( ! ( lookup_google_graph =
			calloc( 1,
				sizeof ( LOOKUP_GOOGLE_GRAPH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_google_graph;
}

GOOGLE_FILENAME *lookup_google_filename( LIST *lookup_google_graph_list )
{
	LOOKUP_GOOGLE_GRAPH *lookup_google_graph;

	if ( list_length( lookup_google_graph_list ) == 1 )
	{
		lookup_google_graph =
			list_first(
				lookup_google_graph_list );

		return
		lookup_google_graph->
			google_graph->
			google_filename;
	}
	else
	{
		return NULL;
	}
}

LOOKUP_GOOGLE_AGGREGATE *lookup_google_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		LIST *chart_aggregate_attribute_list,
		int chart_number )
{
	LOOKUP_GOOGLE_AGGREGATE *lookup_google_aggregate;
	CHART_AGGREGATE_ATTRIBUTE *chart_aggregate_attribute;
	char *title;
	char *filename_key;
	LOOKUP_GOOGLE_GRAPH *lookup_google_graph;

	lookup_google_aggregate = lookup_google_aggregate_calloc();

	lookup_google_aggregate->google_graph_list = list_new();

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
					(char *)0 /* number_attribute_name */,
					average /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */,
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						week_average_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					(char *)0 /* number_attribute_name */,
					sum /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */,
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					weekly /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						week_sum_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

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
					(char *)0 /* number_attribute_name */,
					average /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */,
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						month_average_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

			title =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				chart_title(
					folder_name,
					(char *)0 /* number_attribute_name */,
					sum /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */,
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					monthly /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						month_sum_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

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
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					average /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						annual_average_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

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
					query_chart->
						query_table_edit_where->
						string );

			filename_key =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				chart_filename_key(
					GOOGLE_FILENAME_STEM,
					chart_aggregate_attribute->
						number_attribute_name,
					sum /* enum aggregate_statistic */,
					annually /* enum aggregate_level */ );

			lookup_google_graph =
				lookup_google_graph_new(
					application_name,
					session_key,
					data_directory,
					query_chart->
						date_attribute_name,
					(char *)0 /* time_attribute_name */,
					chart_aggregate_attribute->
						annual_sum_row_list,
					chart_aggregate_attribute->
						number_attribute_name,
					title,
					filename_key,
					++chart_number );

			list_set(
				lookup_google_aggregate->google_graph_list,
				lookup_google_graph );

		} /* if annual_average_row_list */

	} while ( list_next( chart_aggregate_attribute_list ) );

	return lookup_google_aggregate;
}

LOOKUP_GOOGLE_AGGREGATE *lookup_google_aggregate_calloc( void )
{
	LOOKUP_GOOGLE_AGGREGATE *lookup_google_aggregate;

	if ( ! ( lookup_google_aggregate =
			calloc( 1,
				sizeof ( LOOKUP_GOOGLE_AGGREGATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_google_aggregate;
}

