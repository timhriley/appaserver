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
#include "appaserver_link_file.h"

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
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *output_filename;
	char *link_prompt;
	char *date_string;

	application_name = environ_get_application_name( argv[ 0 ] );

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

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
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
			application_get_relative_source_directory(
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

		appaserver_link_file->filename_stem = folder_name;

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

} /* main() */

void remove_files(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name )
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	char sys_string[ 1024 ];
	char *output_filename;
	char *folder_name;

	if ( !list_rewind( folder_name_list ) ) return;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			application_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
			"csv" );

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

/*
		sprintf(output_filename, 
		 	OUTPUT_FILE_TEMPLATE,
		 	appaserver_mount_point,
		 	application_name, 
		 	folder_name,
		 	process_id );
*/

		appaserver_link_file->filename_stem = folder_name;

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

		sprintf( sys_string, "rm %s", output_filename );
		system( sys_string );

	} while( list_next( folder_name_list ) );

} /* remove_files() */

/* Returns link_prompt */
/* ------------------- */
char *output_zip_file(	LIST *folder_name_list,
			char *document_root_directory,
			char *date_string,
			char *application_name )
{
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *source_directory;
	char *folder_name;
	char sys_string[ 65536 ];
	char *ptr = sys_string;
	char *output_filename;
	char *link_prompt;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			application_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			date_string /* session */,
			"zip" );

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

	link_prompt =
		appaserver_link_get_link_prompt(
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

	source_directory =
		appaserver_link_get_source_directory(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name );

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

	appaserver_link_file->extension = "csv";

	do {
		folder_name = list_get( folder_name_list );

		if ( strcmp( folder_name, "appaserver_sessions" ) == 0 )
			continue;

		appaserver_link_file->filename_stem = folder_name;

		output_filename =
			appaserver_link_get_abbreviated_output_filename(
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		ptr += sprintf( ptr, " %s", output_filename );

	} while( list_next( folder_name_list ) );

	system( sys_string );

	return link_prompt;

} /* output_zip_file() */

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

} /* subtract_exclude_application_export() */

void export_output_spreadsheet_folder(	char *output_filename,
					char *application_name,
					char *folder_name )
{
	FOLDER *folder;
	LIST *row_dictionary_list = {0};
	QUERY *query;
	LIST *attribute_name_list;
	LIST *mto1_isa_related_folder_list;

	folder = folder_new_folder(
				application_name,
				(char *)0 /* session */,
				folder_name );

	mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			(char *)0 /* session */,
			folder_name,
			(char *)0 /* role_name */,
			1 /* isa_flag */,
			related_folder_recursive_all,
			0 /* dont override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	folder->attribute_list =
		attribute_get_attribute_list(
		application_name,
		folder_name,
		(char *)0 /* attribute_name */,
		mto1_isa_related_folder_list,
		(char *)0 /* role_name */ );

	attribute_name_list =
		attribute_get_lookup_allowed_attribute_name_list(
			folder->attribute_list );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			application_name,
			(char *)0 /* session */,
			folder->folder_name,
			0 /* no override_row_restrictions */,
			(char *)0 /* role_name */,
			(LIST *)0 /* mto1_related_folder_list */ );

	query = query_edit_table_new(
			application_name,
			(char *)0 /* login_name */,
			folder->folder_name,
			(DICTIONARY *)0 /* query_dictionary */,
			(ROLE *)0 );

	row_dictionary_list =
		query_row_dictionary_list(
				query->folder->application_name,
				query->query_output->select_clause,
				query->query_output->from_clause,
				query->query_output->where_clause,
				query->query_output->order_clause,
				query->max_rows,
				folder->attribute_list
					/* append_isa_attribute_list */,
				query->login_name );

	dictionary_list_output_to_file(	output_filename, 
					row_dictionary_list,
					attribute_name_list,
					(char *)0 /* heading_display */ );

} /* export_output_spreadsheet_folder() */
