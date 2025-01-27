/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_whisker.c			*/
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
#include "float.h"
#include "query.h"
#include "dictionary.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "process_generic_whisker.h"

PROCESS_GENERIC_WHISKER *process_generic_whisker_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			char *document_root,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_WHISKER *process_generic_whisker;

	if ( !application_name
	||   !login_name
	||   !document_root
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

	process_generic_whisker = process_generic_whisker_calloc();

	process_generic_whisker->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_whisker->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_whisker->
				process_generic_process_set_name );

	dictionary_set(
		post_dictionary,
		PROCESS_GENERIC_WHISKER_YN,
		"y" );

	process_generic_whisker->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_whisker->
				process_generic_process_name,
			process_generic_whisker->
				process_generic_process_set_name,
			PROCESS_GENERIC_GRACE_MEDIUM_STRING,
			document_root,
			post_dictionary );

	process_generic_whisker->point_list =
		process_generic_whisker_point_list(
			SQL_DELIMITER,
			process_generic_whisker->
				process_generic->
				system_string,
			process_generic_whisker->
				process_generic->	
				process_generic_input->
				date_piece,
			process_generic_whisker->
				process_generic->
				process_generic_input->
				time_piece,
			process_generic_whisker->
				process_generic->
				process_generic_input->
				value_piece /* average_value_piece */,
			process_generic_whisker->
				process_generic->
				process_generic_input->
				value_piece + 1 /* low_value_piece */,
			process_generic_whisker->
				process_generic->
				process_generic_input->
				value_piece + 2 /* high_value_piece */ );

	process_generic_whisker->horizontal_range =
		process_generic_whisker_horizontal_range(
			process_generic_whisker->point_list );

	process_generic_whisker->vertical_range =
		process_generic_whisker_vertical_range(
			process_generic_whisker->point_list );

	process_generic_whisker->grace_whisker =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_whisker_new(
			application_name,
			document_root,
			process_generic_whisker->
				process_generic->
				value_attribute_name,
			process_generic_whisker->
				process_generic->
				title,
			process_generic_whisker->
				process_generic->
				sub_title,
			process_generic_whisker->point_list,
			process_generic_whisker->horizontal_range,
			process_generic_whisker->vertical_range );

	process_generic_whisker->grace_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_window_new(
			process_generic_whisker->
				grace_whisker->
				grace_setup->
				grace_filename,
			process_generic_whisker->
				process_generic->
				sub_title,
			process_generic_whisker->
				process_generic->
				value_attribute_name
					/* datatype_name */,
			getpid() /* process_id */ );

	process_generic_whisker->grace_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_prompt_new(
			process_generic_whisker->
				grace_whisker->
				grace_setup->
				grace_filename,
			process_generic_whisker->
				process_generic->
				value_attribute_name
					/* datatype_name */,
			getpid() /* process_id */ );

	return process_generic_whisker;
}

PROCESS_GENERIC_WHISKER *process_generic_whisker_calloc( void )
{
	PROCESS_GENERIC_WHISKER *process_generic_whisker;

	if ( ! ( process_generic_whisker =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_WHISKER ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_whisker;
}

char *process_generic_whisker_title( char *process_name )
{
	static char title[ 128 ];

	return
	string_initial_capital(
		title /* destination */,
		process_name );
}

LIST *process_generic_whisker_point_list(
		char delimiter,
		char *system_string,
		int date_piece,
		int time_piece,
		int average_value_piece,
		int low_value_piece,
		int high_value_piece )
{
	LIST *whisker_point_list;
	FILE *input_pipe;
	char delimited_row[ 1024 ];

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

	whisker_point_list = list_new();

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe( system_string );

	while ( string_input( delimited_row, input_pipe, 1024 ) )
	{
		list_set(
			whisker_point_list,
			grace_whisker_point_delimited_parse(
				delimiter,
				date_piece,
				time_piece,
				average_value_piece,
				low_value_piece,
				high_value_piece,
				delimited_row ) );
	}

	pclose( input_pipe );
	return whisker_point_list;
}

int process_generic_whisker_horizontal_range( LIST *whisker_point_list )
{
	double lowest = FLOAT_MAXIMUM_DOUBLE;
	double highest = FLOAT_MINIMUM_DOUBLE;
	GRACE_WHISKER_POINT *point;

	if ( !list_rewind( whisker_point_list ) ) return 0;

	do {
		point = list_get( whisker_point_list );

		if ( point->horizontal_double < lowest )
			lowest = point->horizontal_double;
		else
		if ( point->horizontal_double > highest )
			highest = point->horizontal_double;

	} while ( list_next( whisker_point_list ) );

	return
	float_round_int( highest ) -
	float_round_int( lowest );
}

double process_generic_whisker_vertical_range( LIST *whisker_point_list )
{
	double lowest = FLOAT_MAXIMUM_DOUBLE;
	double highest = FLOAT_MINIMUM_DOUBLE;
	GRACE_WHISKER_POINT *point;

	if ( !list_rewind( whisker_point_list ) ) return 0.0;

	do {
		point = list_get( whisker_point_list );

		if ( point->low_value < lowest )
			lowest = point->low_value;
		else
		if ( point->high_value > highest )
			highest = point->high_value;

	} while ( list_next( whisker_point_list ) );

	return highest - lowest;
}

