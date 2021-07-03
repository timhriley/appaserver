/* $APPASERVER_HOME/src_appaserver/output_prompt_insert_form.c		*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include "timlib.h"
#include "list.h"
#include "session.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "folder.h"
#include "related_folder.h"
#include "dictionary.h"
#include "form.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "environ.h"
#include "process_parameter_list.h"
#include "role_folder.h"
#include "role.h"
#include "dictionary_appaserver.h"
#include "lookup_before_drop_down.h"
#include "relation.h"
#include "pair_one2m.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY			"prompt"
#define COOKIE_KEY_PREFIX			"iinsert"

/* Prototypes */
/* ---------- */
void build_related_folder_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			LIST *element_list,
			LIST *attribute_list,
			char *attribute_name,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *isa_folder_list,
			char *folder_name,
			int tab_index,
			boolean omit_drop_down_new_push_button,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_push_button_prefix,
			char *ignore_push_button_heading,
			char *post_change_javascript,
			LIST *role_folder_insert_list,
			char *form_name,
			boolean is_primary_attribute,
			char *state,
			LOOKUP_BEFORE_DROP_DOWN *
				lookup_before_drop_down,
			LIST *foreign_attribute_name_list );

LIST *output_prompt_insert_attribute_element_list(
			int *current_reference_number,
			char *application_name,
			LIST *attribute_list,
			char *attribute_name,
			boolean omit_push_buttons,
			boolean omit_ignore_push_buttons,
			LIST *role_folder_insert_list,
			boolean is_primary_attribute,
			int tab_index,
			char *folder_post_change_javascript );

void get_with_isa_variables(	LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char **isa_multi_attribute_name,
				char **isa_multi_attribute_data,
				LIST **isa_folder_list,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *mto1_isa_related_folder_list,
				DICTIONARY *non_prefixed_dictionary,
				boolean override_row_restrictions );

void get_without_isa_variables(	LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean override_row_restrictions );

void get_selected_choose_isa_drop_down_with_isa_variables(
				LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean override_row_restrictions );

void get_not_selected_choose_isa_drop_down_with_isa_variables(
				LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				LIST **isa_folder_list,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *mto1_isa_related_folder_list,
				boolean override_row_restrictions );

LIST *output_prompt_insert_element_list(	
			int *current_reference_number,
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *allowed_attribute_name_list,
			LIST *mto1_related_folder_list,
			boolean omit_push_buttons,
			boolean omit_ignore_push_buttons,
			char *folder_notepad,
			char *isa_multi_attribute_name,
			char *isa_multi_attribute_data,
			LIST *isa_folder_list,
			char *form_name,
			DICTIONARY *preprompt_dictionary,
			char *post_change_javascript,
			char *state,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name = {0};
	char *database_string = {0};
	char *session;
	char *folder_name;
	char *role_name, *state;
	FORM *form;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	LIST *allowed_attribute_name_list = {0};
	DICTIONARY *original_post_dictionary = {0};
	LIST *mto1_isa_related_folder_list = {0};
	LIST *mto1_related_folder_list;
	LIST *attribute_list;
	char *folder_notepad;
	char *html_help_file_anchor;
	char *isa_multi_attribute_name = {0};
	char *isa_multi_attribute_data = {0};
	boolean omit_push_buttons;
	boolean omit_ignore_push_buttons = 0;
	FOLDER *folder;
	ROLE *role;
	LIST *remember_button_non_multi_element_name_list;
	LIST *remember_button_multi_element_name_list;
	char *prelookup_button_control_string = {0};
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	boolean with_dynarch_menu = 0;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	LIST *isa_folder_list = {0};
	PAIR_ONE2M *pair_one2m;
	RELATED_FOLDER *ajax_fill_drop_down_related_folder = {0};

	/* -------------------------------------------- */
	/* Maybe called from output_results.		*/
	/* output_results does "onload=window.open()"	*/
	/* -------------------------------------------- */
	/* application_name = environ_get_application_name( argv[ 0 ] ); */

	if ( argc >= 3 )
	{
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

		appaserver_error_starting_argv_append_file(
			argc,
			argv,
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );
	environ_prepend_dot_to_path();
	environ_appaserver_home();
	add_utility_to_path();
	add_local_bin_to_path();
	add_relative_source_directory_to_path( application_name );

	/* Note: optionally there could be a trailing dictionary string */
	/* ------------------------------------------------------------ */
	if ( argc < 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state omit_buttons_yn [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	/* Maybe called from output_results. */
	/* --------------------------------- */
	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				state ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	if ( strcmp( state, "insert" ) != 0 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid state of (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	omit_push_buttons = ( *argv[ 7 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role = role_new_role(	application_name,
				role_name );

	folder = folder_with_load_new(
			application_name,
			BOGUS_SESSION,
			folder_name,
			role );

	if ( argc == 9 )
	{
		char *dictionary_string;
		char escaped_delimiter_string[ 3 ];
		char delimiter_string[ 2 ];

		dictionary_string = argv[ 8 ];

		/* ---------------------------------------------------- */
		/* If from output_results, then this is executed by	*/
		/* window.open(). Therefore, the delimiters are escaped.*/
		/* ---------------------------------------------------- */
		sprintf( escaped_delimiter_string,
			 "\\%c",
			 DICTIONARY_ALTERNATIVE_DELIMITER );

		if ( timlib_exists_string(
				dictionary_string,
				escaped_delimiter_string ) )
		{
			sprintf( delimiter_string,
				 "%c",
				 DICTIONARY_ALTERNATIVE_DELIMITER );

			search_replace_string(
				dictionary_string,
				escaped_delimiter_string,
				delimiter_string );

			sprintf( escaped_delimiter_string,
			 	 "\\%c",
				 '&' );

			sprintf( delimiter_string,
				 "%c",
				 '&' );

			search_replace_string(
				dictionary_string,
				escaped_delimiter_string,
				delimiter_string );
		}

		original_post_dictionary = 
			dictionary_string2dictionary( 
				dictionary_string );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					original_post_dictionary,
					application_name,
					folder->attribute_list,
					(LIST *)0 /* operation_name_list */) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: exiting early.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}
	}
	else
	{
		dictionary_appaserver =
			dictionary_appaserver_new(
				(DICTIONARY *)0,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	/* If came from output_choose_isa_drop_down */
	/* ---------------------------------------- */
	if ( ( mto1_isa_related_folder_list =
		related_folder_get_isa_related_folder_list(
			application_name,
			session,
			folder_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			related_folder_recursive_all ) ) )
	{
		get_with_isa_variables(
				&mto1_related_folder_list,
				&attribute_list,
				&allowed_attribute_name_list,
				&folder_notepad,
				&html_help_file_anchor,
				&isa_multi_attribute_name,
				&isa_multi_attribute_data,
				&isa_folder_list,
				application_name,
				session,
				folder_name,
				role_name,
				mto1_isa_related_folder_list,
				dictionary_appaserver->
					non_prefixed_dictionary,
				role_get_override_row_restrictions(
				role->override_row_restrictions_yn ) );
	}
	else
	{
		get_without_isa_variables(
			&mto1_related_folder_list,
			&attribute_list,
			&allowed_attribute_name_list,
			&folder_notepad,
			&html_help_file_anchor,
			application_name,
			session,
			folder_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ) );
	}

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			state );

	lookup_before_drop_down->lookup_before_drop_down_state =
		lookup_before_drop_down_get_state(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->
				preprompt_dictionary,
			0 /* folder_lookup_before_drop_down */ );

	form =
		form_new(
			INSERT_UPDATE_KEY,
			application_title_string(
				application_name ) );

	form_set_folder_parameters(
		form,
		state,
		login_name,
		application_name,
		session,
		folder_name,
		role_name );

	form_set_post_process( form, "post_prompt_insert_form" );
	form->target_frame = EDIT_FRAME;
	form->output_row_zero_only = 1;
	form->html_help_file_anchor = html_help_file_anchor;

	pair_one2m =
		pair_one2m_prompt_form_new(
			folder_name
				/* one_folder_name */ );

	pair_one2m->prompt_form_folder_list =
		pair_one2m_prompt_form_folder_list(
			relation_one2m_pair_relation_list(
				relation_one2m_relation_list(
					pair_one2m->one_folder_name ) ) );


	omit_ignore_push_buttons =
		( list_length( pair_one2m->
				prompt_form_folder_list ) > 0 );

	form->regular_element_list =
		output_prompt_insert_element_list(
			&form->current_reference_number,
			&ajax_fill_drop_down_related_folder,
			login_name,
			application_name,
			session,
			folder_name,
			role_name,
			attribute_list,
			allowed_attribute_name_list,
			mto1_related_folder_list,
			omit_push_buttons,
			omit_ignore_push_buttons,
			folder_notepad,
			isa_multi_attribute_name,
			isa_multi_attribute_data,
			isa_folder_list,
			form->form_name,
			dictionary_appaserver->
				preprompt_dictionary,
			folder->post_change_javascript,
			state,
			lookup_before_drop_down );

	/* Setup remember button */
	/* --------------------- */
	remember_button_non_multi_element_name_list =
		element_list2remember_keystrokes_non_multi_element_name_list(
			form->regular_element_list );

	remember_button_multi_element_name_list =
		element_list2remember_keystrokes_multi_element_name_list(
			form->regular_element_list );

	form->onload_control_string =
		form_get_remember_keystrokes_onload_control_string(
			form->form_name,
			remember_button_non_multi_element_name_list,
			remember_button_multi_element_name_list,
			folder->post_change_javascript,
			COOKIE_KEY_PREFIX,
			folder_name );

	/* Appends to form->submit_control_string */
	/* -------------------------------------- */
	form_append_remember_keystrokes_submit_control_string(
		&form->onclick_keystrokes_save_string,
		form,
		remember_button_non_multi_element_name_list,
		remember_button_multi_element_name_list,
		COOKIE_KEY_PREFIX,
		folder_name );

	form_set_new_button_onclick_keystrokes_save_string(
		form->regular_element_list,
		form->onclick_keystrokes_save_string );

	pair_one2m->keystrokes_save_function =
		form->onclick_keystrokes_save_string;

	pair_one2m_set_folder_button_string(
		pair_one2m->prompt_form_folder_list,
		pair_one2m->keystrokes_save_function );

	/* Create the two pair_one2m hidden elements */
	/* ----------------------------------------- */
	if ( list_length( pair_one2m->
				prompt_form_folder_list ) > 0 )
	{
		ELEMENT_APPASERVER *element;

		pair_one2m->prompt_one_element_name =
			pair_one2m_prompt_element_name(
				PAIR_ONE2M_PREFIX,
				PAIR_ONE2M_ONE_FOLDER_LABEL );

		element =
			element_appaserver_new(
				hidden,
				strdup(
					pair_one2m->
						prompt_one_element_name ) );

		element->hidden->data = folder_name;

		list_set(
			form->regular_element_list,
			element );

		pair_one2m->prompt_many_element_name =
			pair_one2m_prompt_element_name(
				PAIR_ONE2M_PREFIX,
				PAIR_ONE2M_MANY_FOLDER_LABEL );

		element =
			element_appaserver_new(
				hidden,
				strdup(
					pair_one2m->
						prompt_many_element_name ) );

		list_set(
			form->regular_element_list,
			element );
	}

	document = document_new(
			application_title_string( application_name ),
			application_name );

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "null2slash" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );
	document_set_javascript_module( document, "validate_date" );
	document_set_javascript_module( document, "push_button_submit" );

	document_set_folder_javascript_files(
		document,
		application_name,
		folder_name );

	if ( appaserver_frameset_menu_horizontal(
			application_name,
			login_name ) )
	{
		with_dynarch_menu = 1;
	}

	if ( with_dynarch_menu )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' '%s' 2>>%s",
			application_name,
			session,
			login_name,
			role_name,
			"" /* title */,
			"y" /* content_type_yn */,
			appaserver_error_get_filename(
					application_name ) );
		if ( system( sys_string ) ){};
	}
	else
	{
		document->output_content_type = 1;
	}

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

	if ( attribute_exists_date_attribute( attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
	}

	document_output_body(
			document->application_name,
			document->onload_control_string );

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

	form->table_border = 1;

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
		form->target_frame,
		form->target_frame,
		0 /* no output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		folder->post_change_javascript );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_skipped )
	{
		printf(
		"<h3>Error. You must fill out the pre-lookup form.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( ajax_fill_drop_down_related_folder )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
			 "ajax_fill_drop_down.sh '%s' '%s' %s %s %s '%s'",
			 login_name,
			 role_name,
			 session,
			 ajax_fill_drop_down_related_folder->
				folder->
				folder_name /* one2m_folder */,
			 ajax_fill_drop_down_related_folder->
				one2m_folder->
				folder_name /* mto1_folder */,
			 list_display(
				ajax_fill_drop_down_related_folder->
				folder->
				primary_attribute_name_list ) /* select */ );

		if ( system( sys_string ) ){};
	}

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		(char *)0 /* spool_filename */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null_string */,
		(char *)0 /* appaserver_user_foreign_login_name */,
		(LIST *)0 /* non_edit_folder_name_list */ );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete )
	{
		char *back_to_prelookup_folder_name;

		back_to_prelookup_folder_name =
			( lookup_before_drop_down->
					insert_folder_name )
				? lookup_before_drop_down->
					insert_folder_name
				: lookup_before_drop_down->
					base_folder->folder_name;

		prelookup_button_control_string =
			appaserver_library_prelookup_button_control_string(
				application_name,
				appaserver_parameter_file_get_cgi_directory(),
				appaserver_library_get_server_address(),
				login_name,
				session,
				folder_name,
				back_to_prelookup_folder_name,
				role_name,
				state );
	}

	printf( "</table>\n" );

	output_dictionary_as_hidden(
		dictionary_add_prefix(
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_PREFIX ) );

	output_dictionary_as_hidden(
		dictionary_add_prefix(
			dictionary_appaserver->
				preprompt_dictionary,
			PREPROMPT_PREFIX ) );

	printf( "<table border=0>\n<tr>\n" );

	if ( list_length( pair_one2m->prompt_form_folder_list ) )
	{
		char submit_control_string[ 1024 ];

		sprintf(submit_control_string,
			"%s %s",
			form->submit_control_string,
			pair_folder_element_copy_function(
				pair_one2m_prompt_element_name(
					PAIR_ONE2M_PREFIX,
					PAIR_ONE2M_MANY_FOLDER_LABEL ),
				folder->folder_name ) );

		form_output_prompt_insert_trailer(
			submit_control_string,
			form->html_help_file_anchor,
			form->onload_control_string,
			prelookup_button_control_string,
			application_name,
			folder->post_change_javascript
				/* reset_post_change_javascript */,
			pair_one2m->prompt_form_folder_list );
	}
	else
	{
		form_output_prompt_insert_trailer(
			form->submit_control_string,
			form->html_help_file_anchor,
			form->onload_control_string,
			prelookup_button_control_string,
			application_name,
			folder->post_change_javascript
				/* reset_post_change_javascript */,
			pair_one2m->prompt_form_folder_list );
	}

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();
	return 0;
}

LIST *output_prompt_insert_element_list(
			int *current_reference_number,
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *allowed_attribute_name_list,
			LIST *mto1_related_folder_list,
			boolean omit_push_buttons,
			boolean omit_ignore_push_buttons,
			char *folder_notepad,
			char *isa_multi_attribute_name,
			char *isa_multi_attribute_data,
			LIST *isa_folder_list,
			char *form_name,
			DICTIONARY *preprompt_dictionary,
			char *post_change_javascript,
			char *state,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down )
{
	LIST *return_list;
	LIST *done_attribute_name_list;
	LIST *element_list;
	char *attribute_name;
	ELEMENT_APPASERVER *element;
	LIST *role_folder_insert_list;
	LIST *primary_attribute_name_list;
	boolean is_primary_attribute;
	int tab_index = 0;
	boolean omit_drop_down_new_push_button;
	char *ignore_push_button_prefix = {0};
	char *ignore_push_button_heading = {0};
	RELATED_FOLDER *related_folder;
	LIST *foreign_attribute_name_list = {0};

	if ( !omit_push_buttons )
	{
		omit_drop_down_new_push_button = 0;
		ignore_push_button_prefix = IGNORE_PUSH_BUTTON_PREFIX;
		ignore_push_button_heading = "ignore";
	}
	else
	{
		omit_drop_down_new_push_button = 1;
	}

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	role_folder_insert_list =
		role_folder_get_insert_list(
				application_name,
				session,
				role_name );

	return_list = list_new();
	done_attribute_name_list = list_new();

	allowed_attribute_name_list =
	    list_subtract_string_list(
		allowed_attribute_name_list,
		related_folder_get_common_non_primary_attribute_name_list(
			application_name,
			folder_name,
			mto1_related_folder_list ) );

	if ( *folder_notepad )
	{
		list_append_pointer(
			return_list,
			(char *)element_non_edit_text_new_element(
					"" /* name */,
					folder_notepad,
					99 /* column span */,
					ELEMENT_TITLE_NOTEPAD_PADDING_EM ) );
	}

	if ( !list_reset( allowed_attribute_name_list ) )
		return list_new_list();

	do {
		attribute_name =
			list_get_string(
				allowed_attribute_name_list );

		if ( list_exists_string(
			attribute_name,
			done_attribute_name_list ) )
		{
			continue;
		}

		is_primary_attribute =
			list_exists_string(	
				attribute_name,
				primary_attribute_name_list );

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
				&foreign_attribute_name_list,
				done_attribute_name_list,
			        (LIST *)0 /* omit_update_attribute_name_list */,
				mto1_related_folder_list,
				attribute_name,
				allowed_attribute_name_list ) ) )
		{
			build_related_folder_element_list(
				ajax_fill_drop_down_related_folder,
				return_list,
				attribute_list,
				attribute_name,
				application_name,
				session,
				role_name,
				login_name,
				related_folder,
				isa_folder_list,
				folder_name,
				++tab_index,
				omit_drop_down_new_push_button,
				omit_ignore_push_buttons,
				preprompt_dictionary,
				ignore_push_button_prefix,
				ignore_push_button_heading,
				post_change_javascript,
				role_folder_insert_list,
				form_name,
				is_primary_attribute,
				state,
				lookup_before_drop_down,
				foreign_attribute_name_list );

			continue;
		}

		if ( !list_exists_string(
			attribute_name,
			done_attribute_name_list ) )
		{
			element_list =
				output_prompt_insert_attribute_element_list(
					current_reference_number,
					application_name,
					attribute_list,
					attribute_name,
					omit_push_buttons,
					omit_ignore_push_buttons,
					role_folder_insert_list,
					is_primary_attribute,
					++tab_index,
					post_change_javascript );
	
			if ( element_list )
			{
				list_append_list(
					return_list,
					element_list );
	
				list_append_pointer(
					done_attribute_name_list,
					attribute_name );
			}
		}

	} while( list_next( allowed_attribute_name_list ) );

	/* If isa related folder, output info as hidden elements */
	/* ----------------------------------------------------- */
	if ( isa_multi_attribute_name )
	{
		if ( !isa_multi_attribute_data || !*isa_multi_attribute_data )
		{
			char msg[ 1024 ];
			sprintf( msg,
		"ERROR in %s/%s(): isa_multi_attribute_data is null",
				 __FILE__,
				 __FUNCTION__ );
			appaserver_output_error_message(
				application_name, msg, (char *)0 );
			exit( 1 );
		}

		element =
			element_appaserver_new(
				hidden,
				isa_multi_attribute_name );

		element->hidden->data = strdup( isa_multi_attribute_data );

		list_append(
			return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );
	}

	/* Create the lookup push button */
	/* ----------------------------- */
	element = element_appaserver_new( linebreak, "" );

	list_append(
		return_list, 
		element, 
		sizeof( ELEMENT_APPASERVER ) );

	element =
		element_appaserver_new(
			toggle_button, 
			LOOKUP_PUSH_BUTTON_NAME );

	element_toggle_button_set_heading(
		element->toggle_button, "lookup" );

	list_append(	return_list, 
			element, 
			sizeof( ELEMENT_APPASERVER ) );

	return return_list;
}

void get_without_isa_variables(	LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean override_row_restrictions )
{
	LIST *attribute_name_list;
	LIST *omit_insert_prompt_attribute_name_list;
	APPASERVER *appaserver;

	appaserver =
		appaserver_folder_new(
			application_name,
			session,
			folder_name );

	appaserver->folder->mto1_related_folder_list =
		related_folder_prompt_insert_mto1_related_folder_list(
			list_new_list(),
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			override_row_restrictions );

	appaserver->folder->attribute_list =
		attribute_get_attribute_list(
			appaserver->application_name,
			appaserver->folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	attribute_name_list =
		folder_get_attribute_name_list(
			appaserver->folder->attribute_list );

	omit_insert_prompt_attribute_name_list =
		appaserver_library_get_omit_insert_prompt_attribute_name_list(
			appaserver->folder->attribute_list );

	*allowed_attribute_name_list = list_subtract_list(
				attribute_name_list,
				omit_insert_prompt_attribute_name_list );

	folder_load(	&appaserver->folder->insert_rows_number,
			&appaserver->folder->lookup_email_output,
			&appaserver->folder->row_level_non_owner_forbid,
			&appaserver->folder->row_level_non_owner_view_only,
			&appaserver->folder->populate_drop_down_process,
			&appaserver->folder->post_change_process,
			&appaserver->folder->folder_form,
			&appaserver->folder->notepad,
			&appaserver->folder->html_help_file_anchor,
			&appaserver->folder->post_change_javascript,
			&appaserver->folder->lookup_before_drop_down,
			&appaserver->folder->data_directory,
			&appaserver->folder->index_directory,
			&appaserver->folder->no_initial_capital,
			&appaserver->folder->subschema_name,
			&appaserver->folder->create_view_statement,
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	*attribute_list = appaserver->folder->attribute_list;
	*mto1_related_folder_list =
		appaserver->folder->mto1_related_folder_list;
	*folder_notepad = appaserver->folder->notepad;
	*html_help_file_anchor = appaserver->folder->html_help_file_anchor;

}

void get_with_isa_variables(	LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char **isa_multi_attribute_name,
				char **isa_multi_attribute_data,
				LIST **isa_folder_list,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *mto1_isa_related_folder_list,
				DICTIONARY *non_prefixed_dictionary,
				boolean override_row_restrictions )
{
	char *local_isa_multi_attribute_name;
	char *local_isa_multi_attribute_data;
	RELATED_FOLDER *first_mto1_isa_related_folder;

	first_mto1_isa_related_folder =
		list_get_first_pointer(
			mto1_isa_related_folder_list );

	local_isa_multi_attribute_name =
		build_multi_attribute_key( 
			folder_get_primary_attribute_name_list(
				first_mto1_isa_related_folder->
					folder->
						attribute_list ),
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	/* If selected the isa drop down */
	/* ----------------------------- */ 
	if ( dictionary_get_index_data(
				&local_isa_multi_attribute_data,
				non_prefixed_dictionary,
				local_isa_multi_attribute_name,
				0 ) != -1 )
	{
		*isa_multi_attribute_name = local_isa_multi_attribute_name;
		*isa_multi_attribute_data = local_isa_multi_attribute_data;

		get_selected_choose_isa_drop_down_with_isa_variables(
				mto1_related_folder_list,
				attribute_list,
				allowed_attribute_name_list,
				folder_notepad,
				html_help_file_anchor,
				application_name,
				session,
				folder_name,
				role_name,
				override_row_restrictions );
	}
	else
	{
		get_not_selected_choose_isa_drop_down_with_isa_variables(
				mto1_related_folder_list,
				attribute_list,
				allowed_attribute_name_list,
				folder_notepad,
				html_help_file_anchor,
				isa_folder_list,
				application_name,
				session,
				folder_name,
				role_name,
				mto1_isa_related_folder_list,
				override_row_restrictions );
	}

}

void get_selected_choose_isa_drop_down_with_isa_variables(
				LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				boolean override_row_restrictions )
{
	APPASERVER *appaserver;
	LIST *attribute_name_list;
	LIST *omit_insert_prompt_attribute_name_list;
	LIST *omit_insert_attribute_name_list;
	LIST *primary_attribute_name_list;

	appaserver =
		appaserver_folder_new(
			application_name,
			session,
			folder_name );

	appaserver->folder->mto1_related_folder_list =
		related_folder_prompt_insert_mto1_related_folder_list(
			list_new(),
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			override_row_restrictions );

	appaserver->folder->attribute_list =
		attribute_get_attribute_list(
			appaserver->application_name,
			appaserver->folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				appaserver->folder->
					attribute_list );

	attribute_name_list =
		folder_get_attribute_name_list(
			appaserver->folder->attribute_list );

	omit_insert_prompt_attribute_name_list =
		appaserver_library_get_omit_insert_prompt_attribute_name_list(
			appaserver->folder->attribute_list );

	*allowed_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			omit_insert_prompt_attribute_name_list );

	omit_insert_attribute_name_list =
		appaserver_library_get_omit_insert_attribute_name_list(
			appaserver->folder->attribute_list );

	*allowed_attribute_name_list =
		list_subtract_list(
			*allowed_attribute_name_list,
			omit_insert_attribute_name_list );

	*allowed_attribute_name_list =
		list_subtract_list(
			*allowed_attribute_name_list,
			primary_attribute_name_list );

	folder_load(	&appaserver->folder->insert_rows_number,
			&appaserver->folder->lookup_email_output,
			&appaserver->folder->row_level_non_owner_forbid,
			&appaserver->folder->row_level_non_owner_view_only,
			&appaserver->folder->populate_drop_down_process,
			&appaserver->folder->post_change_process,
			&appaserver->folder->folder_form,
			&appaserver->folder->notepad,
			&appaserver->folder->html_help_file_anchor,
			&appaserver->folder->post_change_javascript,
			&appaserver->folder->lookup_before_drop_down,
			&appaserver->folder->data_directory,
			&appaserver->folder->index_directory,
			&appaserver->folder->no_initial_capital,
			&appaserver->folder->subschema_name,
			&appaserver->folder->create_view_statement,
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	*attribute_list = appaserver->folder->attribute_list;

	*mto1_related_folder_list =
		appaserver->folder->mto1_related_folder_list;

	*folder_notepad = appaserver->folder->notepad;

	*html_help_file_anchor = appaserver->folder->html_help_file_anchor;

}

void get_not_selected_choose_isa_drop_down_with_isa_variables(
				LIST **mto1_related_folder_list,
				LIST **attribute_list,
				LIST **allowed_attribute_name_list,
				char **folder_notepad,
				char **html_help_file_anchor,
				LIST **isa_folder_list,
				char *application_name,
				char *session,
				char *folder_name,
				char *role_name,
				LIST *mto1_isa_related_folder_list,
				boolean override_row_restrictions )
{
	APPASERVER *appaserver;
	LIST *attribute_name_list;
	LIST *omit_insert_prompt_attribute_name_list;
	LIST *omit_insert_attribute_name_list;
	RELATED_FOLDER *mto1_isa_related_folder;

	if ( !list_rewind( mto1_isa_related_folder_list ) )
	{
		char msg[ 1024 ];
		sprintf(msg,
			"ERROR in %s/%s(): empty mto1_isa_related_folder_list",
			__FILE__,
			__FUNCTION__ );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	appaserver =
		appaserver_folder_new(
			application_name,
			session,
			folder_name );

	appaserver->folder->attribute_list =
		attribute_get_attribute_list(
			appaserver->application_name,
			appaserver->folder->folder_name,
			(char *)0 /* attribute_name */,
			mto1_isa_related_folder_list,
			role_name );

	appaserver->folder->mto1_related_folder_list =
		related_folder_prompt_insert_mto1_related_folder_list(
			list_new_list(),
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			override_row_restrictions );

	*allowed_attribute_name_list = list_new();

	do {
		mto1_isa_related_folder =
			(RELATED_FOLDER *)
				list_get_pointer(
					mto1_isa_related_folder_list );

		list_append_list(
				appaserver->folder->attribute_list,
				attribute_non_primary_attribute_list(
					mto1_isa_related_folder->
						folder->
							attribute_list ) );

		list_append_string_list(
				*allowed_attribute_name_list,
				folder_get_non_primary_attribute_name_list(
					mto1_isa_related_folder->
					folder->attribute_list ) );

		appaserver->folder->mto1_related_folder_list =
			related_folder_prompt_insert_mto1_related_folder_list(
			   appaserver->folder->mto1_related_folder_list,
			   appaserver->application_name,
			   appaserver->session,
			   mto1_isa_related_folder->
				folder->folder_name,
			   role_name,
			   override_row_restrictions );

	} while( list_next( mto1_isa_related_folder_list ) );

	attribute_name_list =
			folder_get_attribute_name_list(
				appaserver->folder->attribute_list );

	omit_insert_prompt_attribute_name_list =
		appaserver_library_get_omit_insert_prompt_attribute_name_list(
			appaserver->folder->attribute_list );

	*allowed_attribute_name_list =
		list_subtract_list(
			attribute_name_list,
			omit_insert_prompt_attribute_name_list );

	omit_insert_attribute_name_list =
		appaserver_library_get_omit_insert_attribute_name_list(
			appaserver->folder->attribute_list );

	*allowed_attribute_name_list =
		list_subtract_list(
			*allowed_attribute_name_list,
			omit_insert_attribute_name_list );

	folder_load(	&appaserver->folder->insert_rows_number,
			&appaserver->folder->lookup_email_output,
			&appaserver->folder->row_level_non_owner_forbid,
			&appaserver->folder->row_level_non_owner_view_only,
			&appaserver->folder->populate_drop_down_process,
			&appaserver->folder->post_change_process,
			&appaserver->folder->folder_form,
			&appaserver->folder->notepad,
			&appaserver->folder->html_help_file_anchor,
			&appaserver->folder->post_change_javascript,
			&appaserver->folder->lookup_before_drop_down,
			&appaserver->folder->data_directory,
			&appaserver->folder->index_directory,
			&appaserver->folder->no_initial_capital,
			&appaserver->folder->subschema_name,
			&appaserver->folder->create_view_statement,
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	*attribute_list = appaserver->folder->attribute_list;

	*mto1_related_folder_list =
		appaserver->folder->mto1_related_folder_list;

	*folder_notepad = appaserver->folder->notepad;

	*html_help_file_anchor = appaserver->folder->html_help_file_anchor;

	*isa_folder_list =
		appaserver_get_isa_folder_list(
					application_name );

}

LIST *output_prompt_insert_attribute_element_list(
			int *current_reference_number,
			char *application_name,
			LIST *attribute_list,
			char *attribute_name,
			boolean omit_push_buttons,
			boolean omit_ignore_push_buttons,
			LIST *role_folder_insert_list,
			boolean is_primary_attribute,
			int tab_index,
			char *folder_post_change_javascript )
{
	ATTRIBUTE *attribute;
	ELEMENT_APPASERVER *element;
	char element_name[ 256 ];
	LIST *return_list;
	char ignore_element_name[ 256 ];
	int width;

	attribute =
		attribute_seek_attribute( 
			attribute_name,
			attribute_list );

	if ( !attribute )
	{
		char msg[ 1024 ];
		sprintf(msg,
	"ERROR in %s/%s(): cannot find attribute = (%s) in list\n",
			__FILE__,
			__FUNCTION__,
			attribute_name );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	if ( appaserver_exclude_permission(
		attribute->exclude_permission_list,
		"insert" ) )
	{
		return (LIST *)0;
	}

	if ( strcmp( attribute->datatype, "timestamp" ) == 0 )
		return( LIST *)0;

	return_list = list_new_list();

	if ( strcmp( attribute->datatype, "hidden_text" ) == 0 )
	{
		element =
			element_appaserver_new(
				hidden,
				attribute->attribute_name );
		list_append_pointer(	return_list,
					element );
		return return_list;
	}

	/* Start off with a line break */
	/* --------------------------- */
	element = element_appaserver_new( linebreak, "" );
	list_append_pointer(
			return_list, 
			element );

	if ( !omit_push_buttons )
	{
		if ( !omit_ignore_push_buttons )
		{
			sprintf( ignore_element_name, "%s%s",
			 	IGNORE_PUSH_BUTTON_PREFIX,
			 	attribute_name );
	
			element = element_appaserver_new(
					toggle_button, 
					strdup( ignore_element_name ) );
	
			element_toggle_button_set_heading(
				element->toggle_button, "ignore" );
	
			list_append_pointer(
					return_list, 
					element );
		}

		/* If New buttons appear before the drop downs */
		/* ------------------------------------------- */
		if ( role_folder_insert_list )
		{
			list_append_pointer(
					return_list,
					element_appaserver_new(
						empty_column,
						"" ) );
		}
	}

	/* Set the prompt */
	/* -------------- */
	if ( is_primary_attribute )
		sprintf( element_name, "*%s", attribute->attribute_name );
	else
		sprintf( element_name, "%s", attribute->attribute_name );

	element =
		element_appaserver_new(
			prompt,
			strdup( element_name ) );

	list_append_pointer(
			return_list, 
			element );

	width = attribute->width;

	if ( strcmp( attribute->datatype, "notepad" ) == 0 )
	{
		element =
			element_appaserver_new(
				notepad,
				attribute->attribute_name );

		element_notepad_set_attribute_width(
			element->notepad,
			width );

		element->notepad->heading = element->name;
	}
	else
	if ( strcmp( attribute->datatype,
		     "reference_number" ) == 0 )
	{
		*current_reference_number =
			appaserver_library_get_reference_number(
					application_name,
					1 );

		element =
			element_appaserver_new(
				reference_number,
				attribute->attribute_name );

		element_reference_number_set_attribute_width(
			element->reference_number, 
			width );
	}
	else
	if ( process_parameter_list_element_name_boolean(
			attribute->attribute_name ) )
	{
		element =
			element_get_yes_no_element(
				attribute->attribute_name,
				(char *)0 /* prepend_folder_name */,
				attribute->post_change_javascript,
				0 /* not with_is_null */,
				0 /* not with_not_null */ );
	}
	else
	if ( strcmp( attribute->datatype, "password" ) == 0 )
	{
		element =
			element_appaserver_new(
				password,
				attribute_name );

		element_password_set_attribute_width(
			element->password,
			width );
	}
	else
	{
		if ( strcmp( attribute->datatype, "time" ) == 0
		||   strcmp( attribute->datatype, "current_time" ) == 0 )
		{
			attribute->post_change_javascript =
				attribute_append_post_change_javascript(
					attribute->post_change_javascript,
					"validate_time(this)",
					1 /* place_first */ );
		}
		else
		if ( strcmp( attribute->datatype, "date" ) == 0
		||   strcmp( attribute->datatype, "current_date" ) == 0 )
		{
			attribute->post_change_javascript =
				attribute_append_post_change_javascript(
					attribute->post_change_javascript,
					"validate_date(this)",
					1 /* place_first */ );
		}

		if ( folder_post_change_javascript )
		{
			attribute->post_change_javascript =
				attribute_append_post_change_javascript(
					attribute->post_change_javascript,
					folder_post_change_javascript,
					0 /* not place_first */ );
		}

		element =
			element_text_item_variant_element(
				attribute->attribute_name,
				attribute->datatype,
				width,
				attribute->post_change_javascript,
				attribute->on_focus_javascript_function );
	}

	element->tab_index = tab_index;

	list_append_pointer(
			return_list, 
			element );

	/* Create the hint message */
	/* ----------------------- */
	if ( attribute->hint_message
	&&   *attribute->hint_message )
	{
		element = 
			element_appaserver_new( 
				non_edit_text,
				attribute->hint_message );

		list_append_pointer(
				return_list, 
				element );
	}

	/* Create a hidden query relational operator equals */
	/* ------------------------------------------------ */
	sprintf( element_name, 
		 "%s%s",
		 QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 attribute->attribute_name );

	element =
		element_appaserver_new(
			hidden,
			strdup( element_name ) );

	element->hidden->data = EQUAL_OPERATOR;

	list_set( return_list, element );

	return return_list;
}

void build_related_folder_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			LIST *element_list,
			LIST *attribute_list,
			char *attribute_name,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			RELATED_FOLDER *related_folder,
			LIST *isa_folder_list,
			char *folder_name,
			int tab_index,
			boolean omit_drop_down_new_push_button,
			boolean omit_ignore_push_buttons,
			DICTIONARY *preprompt_dictionary,
			char *ignore_push_button_prefix,
			char *ignore_push_button_heading,
			char *post_change_javascript,
			LIST *role_folder_insert_list,
			char *form_name,
			boolean is_primary_attribute,
			char *state,
			LOOKUP_BEFORE_DROP_DOWN
				*lookup_before_drop_down,
			LIST *foreign_attribute_name_list )
{
	char *hint_message;
	ATTRIBUTE *attribute;
	LIST *common_non_primary_attribute_name_list;
	boolean set_first_initial_data;

	if ( !related_folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty related_folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( related_folder->ignore_output ) return;

	if ( related_folder->folder->lookup_before_drop_down )
	{
		/* ------------------------------------ */
		/* Have an empty placeholder appear	*/
		/* instead of the New button.		*/
		/* ------------------------------------ */
		role_folder_insert_list = list_new();
	}

	attribute =
		attribute_seek_attribute(
			attribute_name,
			attribute_list );

	if ( !attribute )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot find attribute = (%s) in list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			attribute_name );
		exit( 1 );
	}

	if ( list_length( isa_folder_list )
	&&   appaserver_isa_folder_accounted_for(
		isa_folder_list,
		related_folder->folder->folder_name,
		related_folder->
			related_attribute_name ) )
	{
		return;
	}

	foreign_attribute_name_list =
		related_folder_foreign_attribute_name_list(
	   		folder_get_primary_attribute_name_list(
				related_folder->
					folder->
					attribute_list ),
	   		related_folder->related_attribute_name,
			related_folder->folder_foreign_attribute_name_list );

	if ( related_folder->copy_common_attributes )
	{
		common_non_primary_attribute_name_list =
			attribute_common_non_primary_attribute_name_list(
					application_name,
					folder_name,
					related_folder->
						folder->
						folder_name );
	}
	else
	{
		common_non_primary_attribute_name_list = (LIST *)0;
	}

	hint_message =
		related_folder_get_hint_message(
			attribute->hint_message,
			related_folder->hint_message,
			related_folder->folder->notepad );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		related_folder->folder->folder_name ) )
	{
		set_first_initial_data = 1;
	}
	else
	{
		set_first_initial_data = 0;
	}

	list_append_list(
		element_list,
		related_folder_prompt_insert_element_list(
			ajax_fill_drop_down_related_folder,
			application_name,
			session,
			role_name,
			login_name,
			related_folder->folder->folder_name,
			related_folder->relation_type_isa,
			related_folder->folder->
				populate_drop_down_process,
			related_folder->folder->
				attribute_list,
			foreign_attribute_name_list,
			omit_drop_down_new_push_button,
			omit_ignore_push_buttons,
			preprompt_dictionary,
			ignore_push_button_prefix,
			ignore_push_button_heading,
			post_change_javascript,
			hint_message,
			role_folder_insert_list,
			form_name,
			0 /* max_drop_down_size */,
			common_non_primary_attribute_name_list,
			is_primary_attribute,
			related_folder->
				related_attribute_name,
			0 /* drop_down_multi_select */,
			related_folder->
				folder->
				no_initial_capital,
			state,
			folder_name
				/* one2m_folder_name_for_processes */,
			++tab_index,
			set_first_initial_data,
			0 /* no output_null_option */,
			0 /* no output_not_null_option */,
			1 /* output_select_option */,
			related_folder->omit_lookup_before_drop_down
	 ) );

	related_folder->ignore_output = 1;
}

