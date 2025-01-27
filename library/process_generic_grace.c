/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_grace.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "sql.h"
#include "query.h"
#include "dictionary.h"
#include "attribute.h"
#include "appaserver.h"
#include "appaserver_user.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "accumulate_boolean.h"
#include "aggregate_level.h"
#include "document.h"
#include "folder.h"
#include "environ.h"
#include "units.h"
#include "grace.h"
#include "date_convert.h"
#include "process_generic_grace.h"

PROCESS_GENERIC_GRACE *process_generic_grace_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			char *document_root,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_GRACE *process_generic_grace;

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

	process_generic_grace = process_generic_grace_calloc();

	process_generic_grace->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_grace->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_grace->
				process_generic_process_set_name );

	process_generic_grace->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_grace->
				process_generic_process_name,
			process_generic_grace->
				process_generic_process_set_name,
			PROCESS_GENERIC_GRACE_MEDIUM_STRING,
			document_root,
			post_dictionary );

	process_generic_grace->point_list =
		process_generic_grace_point_list(
			SQL_DELIMITER,
			process_generic_grace->
				process_generic->
				system_string,
			process_generic_grace->
				process_generic->	
				process_generic_input->
				date_piece,
			process_generic_grace->
				process_generic->
				process_generic_input->
				time_piece,
			process_generic_grace->
				process_generic->
				process_generic_input->
				value_piece );

	process_generic_grace->horizontal_range =
		process_generic_grace_horizontal_range(
			process_generic_grace->point_list );

	process_generic_grace->grace_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_generic_new(
			application_name,
			document_root,
			process_generic_grace->
				process_generic->
				value_attribute_name,
			process_generic_grace->process_generic->title,
			process_generic_grace->process_generic->sub_title,
			process_generic_grace->point_list,
			process_generic_grace->horizontal_range );

	process_generic_grace->grace_window =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_window_new(
			process_generic_grace->
				grace_generic->
				grace_setup->
				grace_filename,
			process_generic_grace->
				process_generic->
				sub_title,
			process_generic_grace->
				process_generic_process_name
				/* datatype_name */,
			getpid() /* process_id */ );

	process_generic_grace->grace_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grace_prompt_new(
			process_generic_grace->
				grace_generic->
				grace_setup->
				grace_filename,
			process_generic_grace->
				process_generic_process_name
				/* datatype_name */,
			getpid() /* process_id */ );

	return process_generic_grace;
}

PROCESS_GENERIC_GRACE *process_generic_grace_calloc( void )
{
	PROCESS_GENERIC_GRACE *process_generic_grace;

	if ( ! ( process_generic_grace =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_GRACE ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_grace;
}

LIST *process_generic_grace_point_list(
			char delimiter,
			char *system_string,
			int date_piece,
			int time_piece,
			int value_piece )
{
	LIST *point_list;
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

	point_list = list_new();

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe( system_string );

	while ( string_input( delimited_row, input_pipe, 1024 ) )
	{
		list_set(
			point_list,
			grace_point_delimited_parse(
				delimiter,
				date_piece,
				time_piece,
				value_piece,
				delimited_row ) );
	}

	pclose( input_pipe );
	return point_list;
}

int process_generic_grace_horizontal_range( LIST *grace_point_list )
{
	double lowest = DBL_MAX;
	double highest = 0.0;
	GRACE_POINT *grace_point;

	if ( !list_rewind( grace_point_list ) ) return 0;

	do {
		grace_point = list_get( grace_point_list );

		if ( 	grace_point->horizontal_double < lowest )
		{
			lowest = grace_point->horizontal_double;
		}

		if ( 	grace_point->horizontal_double > highest )
		{
			highest = grace_point->horizontal_double;
		}

	} while ( list_next( grace_point_list ) );

	return (int)highest - (int)lowest + 1;
}

