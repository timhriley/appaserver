/* $APPASERVER_HOME/src_appaserver/output_transmit.c		*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the transmit radio	*/
/* button in some of the lookup forms.				*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "role.h"
#include "dictionary_appaserver.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *role_name;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *post_dictionary;
	LIST *row_dictionary_list = {0};
	QUERY *query;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *output_filename;
	char *ftp_filename;
	pid_t process_id = getpid();
	DICTIONARY_APPASERVER *dictionary_appaserver;
	APPASERVER_LINK_FILE *appaserver_link_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* state = argv[ 6 ]; */

	decode_html_post(
		decoded_dictionary_string,
		argv[ 7 ] );

	post_dictionary = 
		dictionary_index_string2dictionary( 
			decoded_dictionary_string );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_name_list */,
				(LIST *)0 /* attribute_date_name_list */,
				(LIST *)0 /* operation_name_list */,
				(char *)0 /* login_name */ ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: dictionary_appaserver_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	query =
		query_simple_new(
			dictionary_appaserver->query_dictionary,
			login_name,
			folder_name,
			role_name,
			dictionary_appaserver->
				ignore_select_attribute_name_list );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_simple_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	row_dictionary_list =
		query_dictionary_list(
			query->query_select_display,
			query->query_select_name_list,
			query->query_from,
			query->query_where,
			query->query_order,
			query->query_date_convert );

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			folder_name /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

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

	ftp_filename =
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

	dictionary_list_output_to_file(
		output_filename, 
		row_dictionary_list,
		attribute_name_list,
		(char *)0 /* heading_display */ );

	printf( "<h1>Spreadsheet Transmission<br></h1>\n" );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );
	
	printf( "<BR><p>Search criteria: %s<hr>\n",
		query_display_where(
			query->query_output->query_output_where,
			folder_name ) );

	printf( "<p>Selected %d records.</p>\n",
		list_length( row_dictionary_list ) );

	appaserver_library_output_ftp_prompt(
		ftp_filename,
		TRANSMIT_PROMPT,
		(char *)0 /* target */,
		(char *)0 /* application_type */ );

	document_close();
	return 0;
}

