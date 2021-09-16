/* $APPASERVER_HOME/src_appaserver/post_change_sort_order.c	*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the sort order radio	*/
/* button on the lookup forms. The folder must have one of	*/
/* these attribute names:					*/
/* sort_order, display_order, or sequence_number.		*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "session.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "document.h"
#include "application.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "environ.h"
#include "decode_html_post.h"
#include "role.h"
#include "role_folder.h"
#include "dictionary_appaserver.h"
#include "row_security.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_sort_order_post_change_process_execute(
				PROCESS *post_change_process,
				DICTIONARY *post_dictionary,
				char *folder_name,
				char *application_name );

void change_sort_order_state_one(
				char *application_name,
				FOLDER *folder,
				char *login_name,
				char *session,
				ROLE *role,
				DICTIONARY *query_dictionary,
				DICTIONARY *ignore_dictionary );

void change_sort_order_state_two(
				char *application_name,
				FOLDER *folder,
				DICTIONARY *post_dictionary );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char *dictionary_string;
	DICTIONARY *post_dictionary;
	FOLDER *folder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	ROLE *role;
	char *role_name;
	char *database_string = {0};
	char *state;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( argc == 8 )
	{
		dictionary_string = argv[ 7 ];

		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );
	}
	else
	{
		post_dictionary =
			post2dictionary(
				stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );
	}

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_dictionary = dictionary_appaserver->working_post_dictionary;

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( strcmp(	login_name,
			session_get_login_name(
					application_name,
					session ) ) != 0 )
	{
		session_access_failed_message_and_exit(
			application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
		login_name,
		role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	role =
		role_new_role(
			application_name,
			role_name );

	folder =
		folder_fetch(	
			folder_name,
			1 /* fetch_attribute_list */,
			0 /* not fetch_one2m_relation_list */,
			0 /* not fetch_one2m_recursive_relation_list */,
			0 /* not fetch_mto1_isa_recursive_relation_list */,
			1 /* fetch_mto1_relation_list */ );

	document = document_new( "", application_name );
	document->output_content_type = 1;

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "sort_order" );

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

	if ( strcmp( state, "one" ) == 0 )
	{
		dictionary_appaserver_parse_multi_attribute_keys(
			dictionary_appaserver->query_dictionary,
			QUERY_RELATION_OPERATOR_STARTING_LABEL );

		change_sort_order_state_one(
				application_name,
				folder,
				login_name,
				session,
				role,
				dictionary_appaserver->
					query_dictionary,
				dictionary_appaserver->
					ignore_dictionary );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		change_sort_order_state_two(
				application_name,
				folder,
				post_dictionary );

		printf( "<h3>Update Complete</h3\n" );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	document_close();

	exit( 0 );
}

void change_sort_order_state_one(
				char *application_name,
				FOLDER *folder,
				char *login_name,
				char *session,
				ROLE *role,
				DICTIONARY *query_dictionary,
				DICTIONARY *ignore_dictionary )
{
	FORM *form;
	char action_string[ 512 ];
	char *sort_attribute_name;
	LIST *move_attribute_name_list;
	LIST *ignore_attribute_name_list;
	ROW_SECURITY *row_security;
	DICTIONARY *sort_dictionary;
	APPASERVER_ELEMENT *element;
	char onclick[ 1024 ];
	char key[ 128 ];
	char *attribute_name;
	LIST *form_button_list;
	FORM_BUTTON *form_button;

	if ( ! ( sort_attribute_name =
			appaserver_library_sort_attribute_name(
				folder->attribute_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty sort_attribute_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Build the renumber button */
	/* ------------------------- */
	sprintf( onclick,
		 "sort_order_renumber( '%s' )",
		 sort_attribute_name );

	form_button = form_button_new( "Renumber", strdup( onclick ) );
	form_button_list = list_new();
	list_append_pointer( form_button_list, form_button );

	/* Can't ignore the sort attribute */
	/* ------------------------------- */
	sprintf( key, "%s_0", sort_attribute_name );
	dictionary_remove_key( ignore_dictionary, key );

	/* Can't ignore any primary key */
	/* ---------------------------- */
	if ( !list_rewind( folder->primary_key_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty primary_key_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		attribute_name =
			list_get_pointer(
				folder->primary_key_list );

		sprintf( key, "%s_0", attribute_name );
		dictionary_remove_key( ignore_dictionary, key );

	} while( list_next( folder->primary_key_list ) );

	/* These are the attributes that javascript moves up or down. */
	/* ---------------------------------------------------------- */
	move_attribute_name_list =
		attribute_name_list_extract(
			folder->attribute_list );

	ignore_attribute_name_list =
		appaserver_library_no_display_pressed_attribute_name_list(
			ignore_dictionary, 
			move_attribute_name_list );

	move_attribute_name_list = 
		list_subtract( 	move_attribute_name_list, 
				ignore_attribute_name_list );

	/* But don't move the sort_attribute_name */
	/* -------------------------------------- */
	list_subtract_string(
		move_attribute_name_list,
		sort_attribute_name );

	/* Select in order by the sort_attribute_name */
	/* ------------------------------------------ */
	sort_dictionary = dictionary_small();

	dictionary_set_pointer(
		sort_dictionary,
		sort_attribute_name,
		"yes" );

	form =
		form_new(
			SORT_ORDER_ATTRIBUTE_NAME,
			 application_title_string(
				application_name ) );

	sprintf(action_string,
		"%s/post_change_sort_order?%s+%s+%s+%s+%s+two",
		appaserver_parameter_file_get_cgi_directory(),
		login_name,
		application_name,
		session,
		folder->folder_name,
		role->role_name );

	form->action_string = action_string;
	form->table_border = 1;

	form_output_title(
		form->application_title,
		"sort" /* state */,
		form->form_title,
		folder->folder_name,
		form->subtitle_string,
		0 /* not omit_format_initial_capital */ );

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		form->form_name,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		1 /* output_submit_reset_buttons_in_heading */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* post_change_javascript */ );

	row_security = row_security_calloc();

	row_security->query_dictionary = query_dictionary;
	row_security->sort_dictionary = sort_dictionary;

	row_security->row_security_element_list_structure =
		row_security_sort_order_structure_new(
			row_security->query_dictionary,
			row_security->sort_dictionary,
			folder );

	form->regular_element_list =
		row_security->
			row_security_element_list_structure->
			regular_element_list;

	/* ------------------------------------------------- */
	/* Create the move and here radio buttons.	     */
	/* Setting in reverse order because they're stacked. */
	/* ------------------------------------------------- */
	element =
		element_appaserver_new(
			radio_button,
			"here" );

	sprintf( onclick,
		 "sort_order_move( '%s' )",
		 list_display( move_attribute_name_list ) );

	element->radio_button->onclick = strdup( onclick );

	element->radio_button->state = "sort";

	list_prepend_pointer( form->regular_element_list, element );

	element =
		element_appaserver_new(
			radio_button,
			"move" );

	list_prepend_pointer( form->regular_element_list, element );

	/* Set the readonly flags */
	/* ---------------------- */
	element_list_set_readonly( form->regular_element_list );

	form_output_table_heading(
		form->regular_element_list,
		0 /* form_number */ );

	form->row_dictionary_list =
		row_security->
			row_security_element_list_structure->
				row_dictionary_list;

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		folder->row_level_non_owner_view_only,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null_string */,
		(char *)0 /* appaserver_user_foreign_login_name */,
		(LIST *)0 /* non_edit_folder_name_list */ );

	printf( "</table>\n" );
	printf( "<table border=0>\n" );

	form_output_trailer(
		1 /* output_submit_reset_buttons_in_trailer */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_back_to_top_button */,
		0 /* form_number */,
		form_button_list,
		(char *)0 /* post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );
}

void change_sort_order_state_two(
				char *application_name,
				FOLDER *folder,
				DICTIONARY *post_dictionary )
{
	char *table_name;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	int row;
	char *sort_attribute_name;
	LIST *primary_data_list;
	char key[ 128 ];
	char *new_sort;
	char buffer[ 16 ];

	sort_attribute_name =
		appaserver_library_sort_attribute_name(
			folder->attribute_list );

	folder->primary_key_list =
		attribute_primary_key_list(
			folder->attribute_list );

	table_name = get_table_name( application_name, folder->folder_name );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 list_display( folder->primary_key_list ) );

	output_pipe = popen( sys_string, "w" );

	for( row = 1;; row++ )
	{
		primary_data_list =
			dictionary_get_data_list(
					folder->primary_key_list,
					post_dictionary,
					row );

		if ( !list_length( primary_data_list ) ) break;

		sprintf( key, "%s_%d", sort_attribute_name, row );

		if ( ! ( new_sort = dictionary_fetch( key, post_dictionary ) ) )
		{
			break;
		}

		fprintf( output_pipe,
		 	 "%s^%s^%s\n",
			 list_display_delimited( primary_data_list, '^' ),
		 	 sort_attribute_name,
			 /* ----------------------------- */
			 /* Trim any thousands separators */
			 /* ----------------------------- */
		 	 trim_character( buffer, ',', new_sort ) );
	}

	pclose( output_pipe );

	if ( folder->post_change_process )
	{
		post_change_sort_order_post_change_process_execute(
			folder->post_change_process,
			post_dictionary,
			folder->folder_name,
			application_name );
	}

}

void post_change_sort_order_post_change_process_execute(
			PROCESS *post_change_process,
			DICTIONARY *post_dictionary,
			char *folder_name,
			char *application_name )
{
	process_convert_parameters(
		&post_change_process->executable,
		application_name,
		(char *)0 /* session */,
		"sort" /* state */,
		(char *)0 /* login_name */,
		folder_name,
		(char *)0 /* role_name */,
		(char *)0 /* target_frame */,
		post_dictionary /* parameter_dictionary */,
		post_dictionary /* where_clause_dictionary */,
		(LIST *)0 /* attribute_list */,
		(LIST *)0 /* prompt_list */,
		(LIST *)0 /* primary_key_list */,
		(LIST *)0 /* primary_data_list */,
		0 /* row */,
		post_change_process->process_name,
		(PROCESS_SET *)0,
		(char *)0
		/* one2m_folder_name_for_processes */,
		(char *)0 /* operation_row_count_string */,
		(char *)0 /* prompt */ );

	if ( system( post_change_process->executable ) ){}
}

