/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_statistic.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "process_generic_statistic.h"

PROCESS_GENERIC_STATISTIC *process_generic_statistic_new(
		char *application_name,
		char *login_name,
		char *process_name,
		char *process_set_name,
		DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_STATISTIC *process_generic_statistic;
	FILE *input_pipe;
	char input[ 1024 ];

	if ( !application_name
	||   !login_name
	||   !process_name
	||   !process_set_name
	||   !post_dictionary )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_statistic = process_generic_statistic_calloc();

	process_generic_statistic->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_statistic->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_statistic->
				process_generic_process_set_name );

	process_generic_statistic->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_statistic->
				process_generic_process_name,
			process_generic_statistic->
				process_generic_process_set_name,
			(char *)0 /* output_medium_string */,
			(char *)0 /* document_root */,
			post_dictionary );

	process_generic_statistic->statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statistic_new(
			process_generic_statistic->
				process_generic->
				process_generic_input->
				value_piece,
			-1 /* weight_piece */,
			SQL_DELIMITER );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			process_generic_statistic->
				process_generic->
				system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		statistic_accumulate(
			process_generic_statistic->
				statistic /* in/out */,
			process_generic_statistic->
				statistic->
				statistic_number_array /* in/out */,
			input );
	}

	pclose( input_pipe );

	process_generic_statistic->statistic->statistic_calculate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statistic_calculate_new(
			process_generic_statistic->
				statistic->
				statistic_number_array /* in/out */,
			process_generic_statistic->
				statistic->
				weight_count,
			process_generic_statistic->
				statistic->
				sum,
			process_generic_statistic->
				statistic->
				row_count,
			process_generic_statistic->
				statistic->
				value_count,
			process_generic_statistic->
				statistic->
				non_zero_count );

	statistic_number_array_sort(
		process_generic_statistic->
			statistic->
			statistic_number_array /* in/out */ );

	process_generic_statistic->statistic->statistic_quartile =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statistic_quartile_new(
			process_generic_statistic->
				statistic->
				statistic_number_array );

	process_generic_statistic->html_table =
		process_generic_statistic_html_table(
			process_generic_statistic->
				process_generic->
				process_generic_value->
				value_attribute_name,
			process_generic_statistic->
				process_generic->
				sub_title,
			process_generic_statistic->
				statistic );

	return process_generic_statistic;
}

PROCESS_GENERIC_STATISTIC *process_generic_statistic_calloc( void )
{
	PROCESS_GENERIC_STATISTIC *process_generic_statistic;

	if ( ! ( process_generic_statistic =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_STATISTIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_statistic;
}

HTML_TABLE *process_generic_statistic_html_table(
		char *value_attribute_name,
		char *sub_title,
		STATISTIC *statistic )
{
	HTML_TABLE *html_table;
	HTML_ROW *html_row;
	char destination[ 128 ];

	if ( !value_attribute_name
	||   !sub_title
	||   !statistic )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		html_table_new(
			(char *)0 /* title */,
			sub_title,
			(char *)0 /* sub_sub_title */ );

	html_table->column_list =
		statistic_table_column_list(
			(char *)0 /* outlier_low_heading_string */,
			(char *)0 /* outlier_high_heading_string */ );

	html_table->row_list = list_new();

	list_set(
		html_table->row_list,
		( html_row = html_row_calloc() ) );

	html_row->cell_list =
		statistic_table_cell_list(
			statistic,
			string_initial_capital(
				destination,
				value_attribute_name )
					/* label */,
			0 /* not outlier_boolean */ );

	return html_table;
}

