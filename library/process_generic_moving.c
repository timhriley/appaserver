/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_moving.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "dictionary.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "aggregate_statistic.h"
#include "moving_statistic.h"
#include "process_generic.h"
#include "process_generic_grace.h"
#include "process_generic_moving.h"

PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet_new(
			char *application_name,
			char *data_directory,
			char *process_generic_sub_title,
			LIST *process_generic_point_list )
{
	PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet;

	if ( !application_name
	||   !data_directory
	||   !process_generic_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_moving_spreadsheet =
		process_generic_moving_spreadsheet_calloc();

	process_generic_moving_spreadsheet->spreadsheet =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spreadsheet_new(
			application_name,
			data_directory,
			getpid() /* process_id */ );

	list_free(
		process_generic_moving_spreadsheet->
			spreadsheet->
			heading_list );

	process_generic_moving_spreadsheet->spreadsheet->heading_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_moving_spreadsheet_heading_list();

	list_free(
		process_generic_moving_spreadsheet->
			spreadsheet->
			row_list );

	process_generic_moving_spreadsheet->spreadsheet->row_list =
		process_generic_moving_spreadsheet_row_list(
			process_generic_point_list );

	return process_generic_moving_spreadsheet;
}

PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet_calloc(
			void )
{
	PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet;

	if ( ! ( process_generic_moving_spreadsheet =
		   calloc( 1,
			   sizeof ( PROCESS_GENERIC_MOVING_SPREADSHEET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_moving_spreadsheet;
}

LIST *process_generic_moving_spreadsheet_heading_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		spreadsheet_heading_new( "date" ) );

	list_set(
		list,
		spreadsheet_heading_new( "value" ) );

	return list;
}

LIST *process_generic_moving_spreadsheet_row_list(
			LIST *process_generic_point_list )
{
	PROCESS_GENERIC_POINT *process_generic_point;
	SPREADSHEET_ROW *spreadsheet_row;
	LIST *list;

	if ( !list_rewind( process_generic_point_list ) ) return (LIST *)0;

	list = list_new();

	do {
		process_generic_point =
			list_get(
				process_generic_point_list );

		list_set(
			list,
			( spreadsheet_row = spreadsheet_row_new() ) );

		list_set(
			spreadsheet_row->cell_list,
			spreadsheet_cell_new(
				process_generic_point->
					date_string /* data */,
				1 /* string_boolean */ ) );

		list_set(
			spreadsheet_row->cell_list,
			spreadsheet_cell_new(
				process_generic_point->
					value_string /* data */,
				0 /* not string_boolean */ ) );

	} while ( list_next( process_generic_point_list ) );

	return list;
}

PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google_new(
			char *application_name,
			char *data_directory,
			char *value_attribute_name,
			char *process_generic_sub_title,
			LIST *process_generic_point_list )
{
	PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google;

	if ( !application_name
	||   !data_directory
	||   !value_attribute_name
	||   !process_generic_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_moving_google =
		process_generic_moving_google_calloc();

	process_generic_moving_google->point_list =
		process_generic_moving_google_point_list(
			process_generic_point_list );

	process_generic_moving_google->google_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_generic_new(
			application_name,
			data_directory,
			value_attribute_name,
			process_generic_moving_google->point_list
				/* google_point_list */ );

	process_generic_moving_google->google_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_window_new(
			application_name,
			process_generic_moving_google->
				google_generic->
				google_filename->
				http_prompt_filename,
		process_generic_sub_title,
		getpid() /* process_id */ );

	return process_generic_moving_google;
}

PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google_calloc( void )
{
	PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google;

	if ( ! ( process_generic_moving_google =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_MOVING_GOOGLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_moving_google;
}

LIST *process_generic_moving_google_point_list(
			LIST *process_generic_point_list )
{
	PROCESS_GENERIC_POINT *process_generic_point;
	LIST *list;
	char *new_date_function_string;
	char destination[ 128 ];
	GOOGLE_VALUE *google_value;
	GOOGLE_CHART_POINT *google_chart_point;

	if ( !list_rewind( process_generic_point_list ) ) return (LIST *)0;

	list = list_new();

	do {
		process_generic_point =
			list_get(
				process_generic_point_list );

		new_date_function_string =
			/* ------------------- */
			/* Returns destination */
			/* ------------------- */
			google_chart_new_date_function_string(
				destination,
				process_generic_point->date_string,
				(char *)0 /* time_string */ );

		google_value =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			google_value_new(
				process_generic_point->value_string );

		google_chart_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			google_chart_point_new(
				new_date_function_string,
				google_value->display );

		list_set(
			list,
			google_chart_point );

		google_value_free( google_value );

	} while ( list_next( process_generic_point_list ) );

	return list;
}

PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_title,
		char *process_generic_sub_title,
		LIST *process_generic_point_list )
{
	PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace;

	if ( !application_name
	||   !data_directory
	||   !value_attribute_name
	||   !process_generic_title
	||   !process_generic_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_moving_grace = process_generic_moving_grace_calloc();

	process_generic_moving_grace->point_list =
		process_generic_moving_grace_point_list(
			process_generic_point_list );

	process_generic_moving_grace->
		process_generic_grace_horizontal_range =
			process_generic_grace_horizontal_range(
				process_generic_moving_grace->point_list );

	process_generic_moving_grace->grace_generic =
		/* ------------------------------------- */
		/* Safely returns.			 */
		/* Interface with PROCESS_GENERIC_GRACE. */
		/* ------------------------------------- */
		grace_generic_new(
			application_name,
			data_directory,
			value_attribute_name,
			process_generic_title,
			process_generic_sub_title,
			process_generic_moving_grace->point_list,
			process_generic_moving_grace->
				process_generic_grace_horizontal_range );

	process_generic_moving_grace->grace_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_window_new(
			process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_filename,
			process_generic_sub_title,
			value_attribute_name /* datatype_name */,
			getpid() /* process_id */ );

	process_generic_moving_grace->grace_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_prompt_new(
			process_generic_moving_grace->
				grace_generic->
				grace_setup->
				grace_filename,
			value_attribute_name /* datatype_name */,
			getpid() /* process_id */ );

	return process_generic_moving_grace;
}

PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace_calloc( void )
{
	PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace;

	if ( ! ( process_generic_moving_grace =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_MOVING_GRACE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_moving_grace;
}

LIST *process_generic_moving_grace_point_list(
			LIST *process_generic_point_list )
{
	LIST *list;
	PROCESS_GENERIC_POINT *process_generic_point;
	double horizontal_double;
	GRACE_POINT *grace_point;

	if ( !list_rewind( process_generic_point_list ) ) return (LIST *)0;

	list = list_new();

	do {
		process_generic_point =
			list_get(
				process_generic_point_list );

		if ( process_generic_point->value_null_boolean )
			continue;

		horizontal_double =
			grace_graph_horizontal_double(
				process_generic_point->date_string,
				(char *)0 /* grace_point_time_string */ );

		grace_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			grace_point_new(
				horizontal_double,
				process_generic_point->value_double
					/* vertical_double */ );

		list_set( list, grace_point );

	} while ( list_next( process_generic_point_list ) );

	return list;
}

PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table_new(
			char *sub_title,
			LIST *process_generic_point_list )
{
	PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table;

	if ( !sub_title )
	{
		char message[ 128 ];

		sprintf(message, "sub_title is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_moving_table = process_generic_moving_table_calloc();

	process_generic_moving_table->html_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		html_table_new(
			(char *)0 /* title */,
			sub_title,
			(char *)0 /* sub_sub_title */ );

	process_generic_moving_table->html_table->heading_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_moving_table_heading_list();

	process_generic_moving_table->html_table->row_list =
		process_generic_moving_table_row_list(
			process_generic_point_list );

	return process_generic_moving_table;
}

PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table_calloc( void )
{
	PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table;

	if ( ! ( process_generic_moving_table =
		   calloc( 1,
			   sizeof ( PROCESS_GENERIC_MOVING_TABLE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_moving_table;
}

LIST *process_generic_moving_table_heading_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		html_heading_new(
			"date",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_heading_new(
			"value",
			1 /* right_justify_boolean */ ) );

	return list;
}

LIST *process_generic_moving_table_row_list(
			LIST *process_generic_point_list )
{
	LIST *list = list_new();
	PROCESS_GENERIC_POINT *process_generic_point;
	HTML_ROW *html_row;

	if ( !list_rewind( process_generic_point_list ) ) return (LIST *)0;

	do {
		process_generic_point =
			list_get(
				process_generic_point_list );

		list_set(
			list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			( html_row = html_row_new() ) );

		list_set(
			html_row->cell_list,
			html_cell_new(
				process_generic_point->
					date_string /* data */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );

		list_set(
			html_row->cell_list,
			html_cell_new(
				process_generic_point->
					value_string /* data */,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );

	} while ( list_next( process_generic_point_list ) );

	return list;
}

PROCESS_GENERIC_MOVING *process_generic_moving_new(
			DICTIONARY *post_dictionary /* in/out */,
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			int moving_statistic_days,
			char *output_medium_string,
			char *data_directory )
{
	PROCESS_GENERIC_MOVING *process_generic_moving;

	if ( !dictionary_length( post_dictionary )
	||   !application_name
	||   !login_name
	||   !output_medium_string
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

	process_generic_moving = process_generic_moving_calloc();

	process_generic_moving->statistic_days =
		process_generic_moving_statistic_days(
			MOVING_STATISTIC_DEFAULT_DAYS,
			moving_statistic_days );

	process_generic_moving->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_moving->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_moving->
				process_generic_process_set_name );

	dictionary_set(
		post_dictionary,
		"aggregate_level",
		"daily" );

	process_generic_moving->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_moving->
				process_generic_process_name,
			process_generic_moving->
				process_generic_process_set_name,
		output_medium_string,
		data_directory,
		post_dictionary );

	process_generic_moving->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_generic_moving_system_string(
			SQL_DELIMITER,
			process_generic_moving->statistic_days,
			process_generic_moving->
				process_generic->
				system_string,
			process_generic_moving->
				process_generic->	
				process_generic_input->
				date_piece,
			process_generic_moving->
				process_generic->
				process_generic_input->
				value_piece,
			process_generic_moving->
				process_generic->
				process_generic_input->
				begin_date_string,
			process_generic_moving->
				process_generic->
				process_generic_input->
				end_date_string,
			process_generic_moving->
				process_generic->
				process_generic_input->
				aggregate_statistic );

	process_generic_moving->process_generic_point_list =
		process_generic_point_list(
			SQL_DELIMITER,
			-1 /* primary_key_list_length */,
			0 /* date_piece */,
			-1 /* time_piece */,
			1 /* value_piece */,
			-1 /* accumulate_piece */,
			process_generic_moving->system_string );

	if ( process_generic_moving->
		process_generic->
		process_generic_input->
		output_medium == table )
	{
		process_generic_moving->
			process_generic_moving_table =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_generic_moving_table_new(
					process_generic_moving->
						process_generic->
						sub_title,
					process_generic_moving->
						process_generic_point_list );
	}
	else
	if ( process_generic_moving->
			process_generic->
			process_generic_input->
			output_medium == gracechart )
	{
		process_generic_moving->
			process_generic_moving_grace =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_generic_moving_grace_new(
					application_name,
					data_directory,
					process_generic_moving->
						process_generic->
						value_attribute_name,
					process_generic_moving->
						process_generic->
						title,
					process_generic_moving->
						process_generic->
						sub_title,
					process_generic_moving->
						process_generic_point_list );
	}
	else
	if ( process_generic_moving->
			process_generic->
			process_generic_input->
			output_medium == spreadsheet )
	{
		process_generic_moving->
			process_generic_moving_spreadsheet =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_generic_moving_spreadsheet_new(
					application_name,
					data_directory,
					process_generic_moving->
						process_generic->
						sub_title,
					process_generic_moving->
						process_generic_point_list );
	}
	else
	if ( process_generic_moving->
			process_generic->
			process_generic_input->
			output_medium == googlechart )
	{
		process_generic_moving->
			process_generic_moving_google =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_generic_moving_google_new(
					application_name,
					data_directory,
					process_generic_moving->
						process_generic->
						value_attribute_name,
					process_generic_moving->
						process_generic->
						sub_title,
					process_generic_moving->
						process_generic_point_list );
	}

	return process_generic_moving;
}

PROCESS_GENERIC_MOVING *process_generic_moving_calloc( void )
{
	PROCESS_GENERIC_MOVING *process_generic_moving;

	if ( ! ( process_generic_moving =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_MOVING ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_moving;
}

int process_generic_moving_statistic_days(
			int moving_statistic_default_days,
			int moving_statistic_days )
{
	if ( moving_statistic_days )
		return moving_statistic_days;
	else
		return moving_statistic_default_days;
}

char *process_generic_moving_system_string(
			char delimiter,
			int moving_statistic_days,
			char *system_string,
			int date_piece,
			int value_piece,
			char *begin_date_string,
			char *end_date_string,
			enum aggregate_statistic aggregate_statistic )
{
	char moving_system_string[ 4096 ];

	if ( !delimiter
	||   !system_string
	||   !begin_date_string
	||   !end_date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(moving_system_string,
		"%s |"
		"%s '%c' %d,-1,%d daily %s 0000 %s 2359 0 1 |"
		"%s %d %d '%c' %s %s",
		system_string,
		"pad_missing_times.e",
		delimiter,
		date_piece,
		value_piece,
		begin_date_string,
		end_date_string,
		"moving_statistic.e",
		moving_statistic_days,
		value_piece,
		delimiter,
		aggregate_statistic_string(
			aggregate_statistic ),
		begin_date_string );

	return strdup( moving_system_string );
}

