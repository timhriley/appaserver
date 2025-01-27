/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_google.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <float.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "query.h"
#include "dictionary.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "google_chart.h"
#include "process_generic_google.h"

PROCESS_GENERIC_GOOGLE *process_generic_google_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			char *data_directory,
			DICTIONARY *post_dictionary /* in/out */ )
{
	PROCESS_GENERIC_GOOGLE *process_generic_google;

	if ( !application_name
	||   !login_name
	||   !process_name
	||   !process_set_name
	||   !data_directory
	||   !dictionary_length( post_dictionary ) )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	process_generic_google = process_generic_google_calloc();

	process_generic_google->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_google->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_google->
				process_generic_process_set_name );


	process_generic_google->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_google->
				process_generic_process_name,
			process_generic_google->
				process_generic_process_set_name,
			PROCESS_GENERIC_GOOGLE_MEDIUM_STRING,
			data_directory,
			post_dictionary );

	process_generic_google->point_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_google_point_list(
			SQL_DELIMITER,
			process_generic_google->
				process_generic->
				system_string,
			process_generic_google->
				process_generic->	
				process_generic_input->
				date_piece,
			process_generic_google->
				process_generic->
				process_generic_input->
				time_piece,
			process_generic_google->
				process_generic->
				process_generic_input->
				value_piece );

	process_generic_google->google_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_generic_new(
			application_name,
			data_directory,
			process_generic_google->
				process_generic->
				value_attribute_name,
			process_generic_google->point_list );

	process_generic_google->google_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		google_window_new(
			application_name,
			process_generic_google->
				google_generic->
				google_filename->
				http_prompt_filename,
			process_generic_google->
				process_generic->
				sub_title,
			getpid() /* process_id */ );

	return process_generic_google;
}

PROCESS_GENERIC_GOOGLE *process_generic_google_calloc( void )
{
	PROCESS_GENERIC_GOOGLE *process_generic_google;

	if ( ! ( process_generic_google =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_GOOGLE ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_google;
}


LIST *process_generic_google_point_list(
			char delimiter,
			char *system_string,
			int date_piece,
			int time_piece,
			int value_piece )
{
	FILE *input_pipe;
	char delimited_row[ 1024 ];
	LIST *point_list;

	if ( !delimiter
	||   !system_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	point_list = list_new();

	input_pipe =
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	while ( string_input( delimited_row, input_pipe, 1024 ) )
	{
		list_set(
			point_list,
			google_chart_point_delimited_parse(
				SQL_DELIMITER,
				date_piece,
				time_piece,
				value_piece,
				delimited_row ) );
	}

	pclose( input_pipe );
	return point_list;
}

