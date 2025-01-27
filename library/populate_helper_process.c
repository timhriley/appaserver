/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/populate_helper_process.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include "populate_helper_process.h"
#include "timlib.h"
#include "appaserver_link.h"

POPULATE_HELPER_PROCESS *populate_helper_process_calloc( void )
{
	POPULATE_HELPER_PROCESS *populate_helper_process;

	if ( ! ( populate_helper_process =
			calloc( 1, sizeof ( POPULATE_HELPER_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return populate_helper_process;
}

POPULATE_HELPER_PROCESS *populate_helper_process_new(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id )
{
	POPULATE_HELPER_PROCESS *populate_helper_process =
		populate_helper_process_calloc();

	populate_helper_process->name = name;

	populate_helper_process->data_directory =
		data_directory;

	populate_helper_process->application_name =
		application_name;

	populate_helper_process->process_id = process_id;

	populate_helper_process->output_filename =
		populate_helper_process_output_filename(
			name,
			data_directory,
			application_name,
			process_id );

	populate_helper_process->prompt =
		populate_helper_process_prompt(
			name,
			data_directory,
			application_name,
			process_id );

	return populate_helper_process;
}

char *populate_helper_process_output_filename(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id )
{
	char *output_filename;
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
	 		data_directory,
			name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	output_filename = appaserver_link->appaserver_link_output->filename;

	return output_filename;
}

char *populate_helper_process_prompt(
		char *name,
		char *data_directory,
		char *application_name,
		pid_t process_id )
{
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
	 		data_directory,
			name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	return appaserver_link->appaserver_link_prompt->filename;
}

