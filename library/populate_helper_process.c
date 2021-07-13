/* library/populate_helper_process.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include "populate_helper_process.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

POPULATE_HELPER_PROCESS *populate_helper_process_new( char *key )
{
	POPULATE_HELPER_PROCESS *populate_helper_process;

	populate_helper_process =
		(POPULATE_HELPER_PROCESS *)
			calloc( 1, sizeof( POPULATE_HELPER_PROCESS ) );
	populate_helper_process->key = key;
	return populate_helper_process;
}

char *populate_helper_process_get_output_filename(
				char *key,
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
			key /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"html" );

	output_filename =
		appaserver_link_get_output_filename(
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

/*
	sprintf(output_filename,
		HELPER_PROCESS_OUTPUT_FILE_TEMPLATE,
		appaserver_mount_point,
		application_name,
		key,
		process_id );
*/

	return output_filename;

}

char *populate_helper_process_get_ftp_file_prompt(
				char *key,
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
			key /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"html" );

	return appaserver_link_get_link_prompt(
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

