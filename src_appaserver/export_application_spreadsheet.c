/* --------------------------------------------------- 	*/
/* src_appaserver/export_application_spreadsheet.c     	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "folder.h"
#include "application.h"
#include "list.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "query.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void remove_files(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name );

/* Returns link_prompt */
/* ------------------- */
char *output_zip_file(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name );

void export_output_spreadsheet_folder(	char *output_filename,
					char *application_name,
					char *folder_name );

LIST *subtract_exclude_application_export(
			char *application_name,
			LIST *folder_name_list );

int main( int argc, char **argv )
{
	char *application_name;
	char sys_string[ 1024 ];
	char system_folders_yn;
	DOCUMENT *document;
	LIST *folder_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *folder_name;
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *link_prompt;
	char *date_string;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s ignored system_folders_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	system_folders_yn = *argv[ 2 ];

	date_string = pipe2string( "now.sh ymd" );

	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	document = document_new( "", application_name );

	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	/* Do the work */
	/* ----------- */
	sprintf( sys_string,
		 "application_folder_list.sh %s %c", 
		 application_name, system_folders_yn );
	folder_name_list = pipe2list( sys_string );

	/* Subtract those with the exclude flag set. */
	/* ----------------------------------------- */
	folder_name_list =
		subtract_exclude_application_export(
			application_name,
			folder_name_list );

	if ( !list_rewind( folder_name_list ) )
	{
		printf(
		"<h3>Warning: there is nothing available to output.</h3>\n" );
		document_close();
		exit( 0 );
	}

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

		appaserver_link->filename_stem = folder_name;

		output_filename =
			appaserver_link_output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		export_output_spreadsheet_folder(
			output_filename,
			application_name,
			folder_name );

	} while( list_next( folder_name_list ) );

	link_prompt =
		output_zip_file(
			folder_name_list,
			appaserver_parameter_file->
		 		document_root,
			date_string,
			application_name );

	remove_files(	folder_name_list,
			appaserver_parameter_file->
				document_root,
			date_string,
			application_name );

	appaserver_library_output_ftp_prompt(
				link_prompt,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );

	document_close();

	return 0;
}

void remove_files(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name )
{
	APPASERVER_LINK *appaserver_link;
	char sys_string[ 1024 ];
	char *output_filename;
	char *folder_name;

	if ( !list_rewind( folder_name_list ) ) return;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			application_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" );

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

		appaserver_link->filename_stem = folder_name;

		output_filename =
			appaserver_link_output_filename(
				appaserver_link->document_root_directory,
				appaserver_link_output_tail_half(
					appaserver_link->application_name,
					appaserver_link->filename_stem,
					appaserver_link->begin_date_string,
					appaserver_link->end_date_string,
					appaserver_link->process_id,
					appaserver_link->session_key,
					appaserver_link->extension ) );

		sprintf( sys_string, "rm %s", output_filename );
		if ( system( sys_string ) ){};

	} while( list_next( folder_name_list ) );
}

/* Returns link_prompt */
/* ------------------- */
char *output_zip_file(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name )
{
	APPASERVER_LINK *appaserver_link;
	char *source_directory;
	char *folder_name;
	char sys_string[ 65536 ];
	char *ptr = sys_string;
	char *output_filename;
	char *link_prompt;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			application_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"zip" );

	output_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	link_prompt =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->prepend_http,
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	source_directory =
		appaserver_link_working_directory(
			document_root_directory,
			appaserver_link->application_name );

	if ( chdir( source_directory ) != 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot chdir(%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 source_directory );
		exit( 1 );
	}

	ptr += sprintf(	ptr,
			"zip -rq %s",
			output_filename );

	if ( !list_rewind( folder_name_list ) ) return "";

	appaserver_link->extension = "csv";

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

		appaserver_link->filename_stem = folder_name;

		output_filename =
			appaserver_link_output_abbreviated_filename(
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension );

		ptr += sprintf( ptr, " %s", output_filename );

	} while( list_next( folder_name_list ) );

	if ( system( sys_string ) ){};

	return link_prompt;

}

LIST *subtract_exclude_application_export(
			char *application_name,
			LIST *folder_name_list )
{
	char sys_string[ 1024 ];
	char *where;

	where = "exclude_application_export_yn = 'y'";

	sprintf( sys_string,
		 "get_folder_data	folder=folder			"
		 "			select=folder			"
		 "			application=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return list_subtract_list( folder_name_list, pipe2list( sys_string ) );
}

void export_output_spreadsheet_folder(
			char *output_filename,
			char *application_name,
			char *folder_name )
{
	LIST *dictionary_list = {0};
	QUERY *query;

	query =
		query_simple_new(
			(DICTIONARY *)0 /* query_dictionary */,
			(char *)0 /* login_name */,
			folder_name,
			(char *)0 /* role_name */,
			(LIST *)0 /* ignore_select_attribute_name_list */ );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	dictionary_list =
		query_dictionary_list(
			query->query_select_display,
			query->query_select_name_list,
			query->query_from,
			query->query_where,
			query->query_order,
			0 /* max_rows */,
			(QUERY_DATE_CONVERT *)0 );

	dictionary_list_output_to_file(
		output_filename, 
			dictionary_list,
			attribute_name_list,
			(char *)0 /* heading_display */ );
}
