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
#include "query_statistic.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "decode_html_post.h"
#include "role.h"
#include "dictionary_separate.h"

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
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char *dictionary_string;
	DICTIONARY *original_post_dictionary;
	QUERY_STATISTIC *query_statistic;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY_SEPARATE *dictionary_separate;
	QUERY *query;
	ROLE *role;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr, 
			"Usage: %s login_name folder role dictionary\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	role_name = argv[ 3 ];
	dictionary_string = argv[ 4 ];

	dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_string_new(
			dictionary_string );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role =
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	if ( !role )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			role_name );
		exit( 1 );
	}

	query =
		query_table_new(
			folder_name,
			login_name,
			dictionary_separate->
				ignore_select_attribute_name_list,
			role,
			dictionary_separate->query_dictionary,
			(DICTIONARY *)0 /* sort_dictionary */ );

	if ( !query )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_row_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query->folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query->folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	document_quick_output( application_name );

	query_statistic =
		query_statistic_new(
			folder_attribute_name_list(
				folder_attribute_number_list(
					query->
						folder->
						folder_attribute_list ) )
				/* folder_attribute_number_name_list */,
			folder_attribute_name_list(
				folder_attribute_date_list(
					query->
						folder->
						folder_attribute_list ) )
				/* folder_attribute_date_name_list */,
			folder_attribute_name_list(
				folder_attribute_date_time_list(
					query->
						folder->
						folder_attribute_list ) )
				/* folder_attribute_date_time_name_list */,
			query->from_clause,
			query->where_clause );

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
*/

	document_close();

	return 0;
}

