/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_table_edit.c			*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "boolean.h"
#include "list.h"
#include "environ.h"
#include "appaserver_error.h"
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
	POST_DICTIONARY *post_dictionary;
	TABLE_EDIT *table_edit;
	boolean menu_horizontal_boolean;

	/* For convenience */
	/* --------------- */
	LIST *viewonly_element_list;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s session login role folder target_frame post_dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	target_frame = argv[ 5 ];

	post_dictionary =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_dictionary_string_new(
			argv[ 6 ] );

	session_environment_set( application_name );

	table_edit =
		table_edit_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			target_frame,
			( menu_horizontal_boolean =
				frameset_menu_horizontal(
					application_name,
					login_name ) ),
			post_dictionary->original_post_dictionary );

	if ( !table_edit )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: table_edit_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}

	if ( !table_edit->query_table_edit )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_table_edit is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !table_edit->form_table_edit )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_table_edit is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( table_edit->row_security->row_security_element_list->viewonly )
	{
		viewonly_element_list =
			table_edit->
				row_security->
				row_security_element_list->
				viewonly->
				element_list;
	}
	else
	{
		viewonly_element_list = (LIST *)0;
	}

	if ( !menu_horizontal_boolean ) document_output_content_type();

	table_edit_output(
		stdout /* output_stream */,
		application_name,
		table_edit->html,
		table_edit->form_table_edit->html,
		table_edit->folder->role_operation_list,
		table_edit->query_table_edit->row_dictionary_list,
		table_edit->
			row_security->
			row_security_element_list->
			regular->
			element_list,
		viewonly_element_list,
		table_edit->
			row_security->
			row_security_role,
		table_edit->state,
		table_edit->form_table_edit->trailer_html,
		table_edit->trailer_html );

	table_edit_spool_file(
		table_edit->spool_filename,
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			table_edit->folder->folder_attribute_append_isa_list ),
		table_edit->query_table_edit->row_dictionary_list,
		SQL_DELIMITER );

	return 0;
}
