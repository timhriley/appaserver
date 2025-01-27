/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_histogram.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "sql.h"
#include "dictionary.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "output_medium.h"
#include "process_generic_histogram.h"

PROCESS_GENERIC_HISTOGRAM *process_generic_histogram_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *process_set_name,
			DICTIONARY *post_dictionary )
{
	PROCESS_GENERIC_HISTOGRAM *process_generic_histogram;

	if ( !application_name
	||   !login_name
	||   !process_name
	||   !process_set_name
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

	process_generic_histogram = process_generic_histogram_calloc();

	process_generic_histogram->process_generic_process_set_name =
		/* ---------------------------------- */
		/* Returns process_set_name or "null" */
		/* ---------------------------------- */
		process_generic_process_set_name(
			process_set_name );

	process_generic_histogram->process_generic_process_name =
	/* ------------------------------------------------------------- */
	/* Returns process_name, component of post_dictionary, or "null" */
	/* ------------------------------------------------------------- */
		process_generic_process_name(
			process_name,
			post_dictionary,
			process_generic_histogram->
				process_generic_process_set_name );

	process_generic_histogram->process_generic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		process_generic_new(
			application_name,
			login_name,
			process_generic_histogram->
				process_generic_process_name,
			process_generic_histogram->
				process_generic_process_set_name,
			OUTPUT_MEDIUM_STDOUT_STRING,
			(char *)0 /* document_root */,
			post_dictionary );

	process_generic_histogram->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		process_generic_histogram_system_string(
			SQL_DELIMITER,
			application_name,
			process_generic_histogram->
				process_generic->
				value_attribute_name,
			process_generic_histogram->
				process_generic->
				system_string,
			process_generic_histogram->
				process_generic->
				title,
			process_generic_histogram->
				process_generic->
				sub_title,
			process_generic_histogram->
				process_generic->
				process_generic_input->
				value_piece );

	return process_generic_histogram;
}

PROCESS_GENERIC_HISTOGRAM *process_generic_histogram_calloc( void )
{
	PROCESS_GENERIC_HISTOGRAM *process_generic_histogram;

	if ( ! ( process_generic_histogram =
			calloc( 1,
				sizeof ( PROCESS_GENERIC_HISTOGRAM ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return process_generic_histogram;
}

char *process_generic_histogram_system_string(
			char delimiter,
			char *application_name,
			char *value_attribute_name,
			char *system_string,
			char *title,
			char *sub_title,
			int value_piece )
{
	char histogram_system_string[ 1024 ];

	if ( !delimiter
	||   !application_name
	||   !value_attribute_name
	||   !system_string
	||   !title
	||   !sub_title )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(histogram_system_string,
		"%s |"
		"grace_histogram.e %s \"%s\" \"%s\" %s '%c' %d",
		system_string,
		application_name,
		title,
		sub_title,
		value_attribute_name,
		delimiter,
		value_piece );

	return strdup( histogram_system_string );
}

