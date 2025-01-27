/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_filename.c			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "application.h"
#include "appaserver_error.h"
#include "vertical_new_filename.h"

VERTICAL_NEW_FILENAME *vertical_new_filename_calloc( void )
{
	VERTICAL_NEW_FILENAME *
		vertical_new_filename;

	if ( ! ( vertical_new_filename =
			calloc(
			   1,
			   sizeof ( VERTICAL_NEW_FILENAME ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_filename;
}

VERTICAL_NEW_FILENAME *vertical_new_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		const char *filename_stem )
{
	VERTICAL_NEW_FILENAME *vertical_new_filename;

	if ( !application_name
	||   !session_key
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

	vertical_new_filename = vertical_new_filename_calloc();

	vertical_new_filename->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			(char *)filename_stem,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" /* extension */ );

	vertical_new_filename->output_filename =
		vertical_new_filename->
			appaserver_link->
			appaserver_link_output->
			filename;

	vertical_new_filename->prompt_filename =
		vertical_new_filename->
			appaserver_link->
			appaserver_link_prompt->
			filename;

	return vertical_new_filename;
}

