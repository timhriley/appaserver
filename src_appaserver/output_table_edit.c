/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_table_edit.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "environ.h"
#include "application.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver_parameter.h"
#include "post_dictionary.h"
#include "session.h"
#include "frameset.h"
#include "document.h"
#include "table_edit.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *target_frame;
	char *results_string;
	char *error_string;
	char dictionary_string[ STRING_SIZE_HASH_TABLE ];
	POST_DICTIONARY *post_dictionary;
	TABLE_EDIT *table_edit;
	boolean menu_horizontal_boolean;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
	"Usage: %s session login role folder target_frame results error\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];
	results_string = argv[ 6 ];
	error_string = argv[ 7 ];

	string_input( dictionary_string, stdin, STRING_SIZE_HASH_TABLE );

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			dictionary_string );

	table_edit =
		/* -------------------------------------------- */
		/* Returns null if lookup permission isn't set. */
		/* Drilldown many to one is affected.		*/
		/* -------------------------------------------- */
		table_edit_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			getpid() /* process_id */,
			results_string,
			error_string,
			( menu_horizontal_boolean =
				application_menu_horizontal_boolean(
					application_name ) ),
			post_dictionary->original_post_dictionary,
			appaserver_parameter_data_directory(),
			(char *)0 /* drilldown_base_folder_name */,
			(char *)0 /* drilldown_primary_data_list_string */,
			0 /* not viewonly_boolean */,
			0 /* not omit_delete_boolean */ );

	if ( !table_edit )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"table_edit_new(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !table_edit->query_table_edit )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
	"table_edit_new(%s) returned an empty table_edit->query_table_edit.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !table_edit->query_table_edit->query_fetch )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
"table_edit_new(%s) returned an empty table_edit->query_table_edit->query_fetch.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !table_edit->form_table_edit )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
	"table_edit_new(%s) returned an empty table_edit->form_table_edit.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !table_edit->appaserver_spool_filename )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
"table_edit_new(%s) returned an empty table_edit->appaserver_spool_filename.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	table_edit_spool_file_output(
		table_edit->appaserver_spool_filename,
		table_edit->query_table_edit->query_fetch->row_list );

	table_edit_output(
		APPASERVER_USER_PRIMARY_KEY,
		login_name,
		table_edit->
			table_edit_input->
			folder_row_level_restriction->
			non_owner_viewonly,
		table_edit->
			table_edit_document->
			html,
		table_edit->form_table_edit->open_html,
		table_edit->
			form_table_edit->
			heading_container_string,
		table_edit->table_edit_input->folder_operation_list,
		table_edit->query_table_edit->query_fetch->row_list
			/* query_fetch_row_list */,
		table_edit->relation_join,
		table_edit->
			row_security->
			regular_widget_list,
		table_edit->
			row_security->
			viewonly_widget_list,
		table_edit->
			table_edit_input->
			row_security_role_update_list,
		table_edit->
			table_edit_input->
			table_edit_state,
		table_edit->form_table_edit->close_html,
		table_edit->
			table_edit_document->
			trailer_html );

	return 0;
}
