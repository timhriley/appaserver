/* $APPASERVER_HOME/library/populate_helper_process.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include "populate_helper_process.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

POPULATE_HELPER_PROCESS *populate_helper_process_calloc( void )
{
	POPULATE_HELPER_PROCESS *populate_helper_process;

	if ( ! ( populate_helper_process =
			calloc( 1, sizeof( POPULATE_HELPER_PROCESS ) ) ) )
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
			char *document_root_directory,
			char *application_name,
			pid_t process_id )
{
	POPULATE_HELPER_PROCESS *populate_helper_process =
		populate_helper_process_calloc();

	populate_helper_process->name = name;

	populate_helper_process->document_root_directory =
		document_root_directory;

	populate_helper_process->application_name =
		application_name;

	populate_helper_process->process_id = process;

	populate_helper_process->output_filename =
		populate_helper_process_output_filename(
			name,
			document_root_directory,
			application_name,
			process_id );

	populate_helper_process->prompt =
		populate_helper_process_prompt(
			name,
			application_name,
			process_id );

	return populate_helper_process;
}

char *populate_helper_process_output_filename(
			char *name,
			char *document_root_directory,
			char *application_name,
			pid_t process_id )
{
	char *output_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"html" );

	output_filename =
		appaserver_link_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	return output_filename;

}

char *populate_helper_process_prompt(
				char *name,
				char *application_name,
				pid_t process_id )
{
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		(char *)0 /* document_root_directory */,
			name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"html" );

	return appaserver_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );
}

