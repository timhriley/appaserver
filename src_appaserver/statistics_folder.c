/* $APPASERVER_HOME/src_appaserver/statistics_folder.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "document.h"
#include "application.h"
#include "dictionary.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "decode_html_post.h"
#include "role.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *folder_name;
	char *role_name;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char *dictionary_string;
	DICTIONARY *original_post_dictionary;
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	QUERY *query;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored ignored folder role dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	dictionary_string = argv[ 6 ];

	decode_html_post(
		decoded_dictionary_string, 
		dictionary_string );

	original_post_dictionary = 
		dictionary_index_string2dictionary( 
			decoded_dictionary_string );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
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

	query =
		query_simple_new(
			dictionary_appaserver->query_dictionary,
			login_name,
			folder_name,
			role_name,
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

	document = document_new( "", application_name );
	document->output_content_type = 1;

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

	query_attribute_statistics_list =
		query_attribute_statistics_list_new(
			document->application_name,
			query->query_from );

	query_attribute_statistics_list->list =
		query_attribute_statistics_list_get_list(
			query->query_folder->attribute_list );

	sys_string =
	query_attribute_statistics_list_get_build_each_temp_file_sys_string(
			query_attribute_statistics_list->application_name,
			query_attribute_statistics_list->folder_name,
			query_attribute_statistics_list->list,
			query->query_where );

	if ( !sys_string )
	{
		printf(
"<p>There are no number attributes or dates in this folder to perform statistics on.\n" );
		document_close();
		return 0;
	}

	if ( system( sys_string ) ){}

	query_attribute_statistics_list_populate_list(
		query_attribute_statistics_list->list );

	query_attribute_statistics_list_output_table(
		query_attribute_statistics_list->folder_name,
		query->query_where
		query_attribute_statistics_list->list,
		application_name );

	query_attribute_statistics_remove_temp_file(
		query_attribute_statistics_list->list );

	document_close();

	return 0;
}

