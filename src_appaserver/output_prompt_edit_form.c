/* $APPASERVER_HOME/src_appaserver/output_prompt_edit_form.c		*/
/* --------------------------------------------------------------------	*/
/*									*/
/* This is the form that displays the blank cells for data entry forms.	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "session.h"
#include "attribute.h"
#include "document.h"
#include "folder.h"
#include "dictionary.h"
#include "form.h"
#include "related_folder.h"
#include "query.h"
#include "role_folder.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "role.h"
#include "process_parameter_list.h"
#include "lookup_before_drop_down.h"
#include "row_security.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define EMAIL_TEXT_ITEM_WIDTH		100
#define DROP_DOWN_NUMBER_COLUMNS	2
#define INSERT_UPDATE_KEY		"prompt"
#define COOKIE_KEY_PREFIX		"edit"

/* Prototypes */
/* ---------- */
void build_related_folder_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			LIST *element_list,
			RELATED_FOLDER *related_folder,
			LIST *done_folder_name_list,
			LIST *isa_folder_list,
			ATTRIBUTE *attribute,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
		  	char *no_display_push_button_prefix,
		  	char *no_display_push_button_heading,
			boolean row_level_non_owner_forbid,
			char *one2m_folder_name_for_processes,
			boolean attribute_exists_in_preprompt_dictionary,
			char *state,
			LIST *foreign_attribute_name_list );

char *get_done_folder_name(
			char *folder_name,
			char *related_attribute_name );

void mark_ignore_for_prelookup_skipped(
			LIST *mto1_related_folder_list,
			LIST *lookup_before_drop_down_folder_list );

LIST *get_preprompt_attribute_name_list(
				DICTIONARY *preprompt_dictionary );

LIST *prompt_mto1_recursive_element_list_fetch(
			LIST *done_folder_name_list,
			char **post_change_javascript,
			LIST *exclude_attribute_name_list,
			FOLDER *prompt_folder,
			boolean prompt_folder_drop_down_multi_select,
			LIST *prompt_recursive_mto1_folder_list,
			char *application_name,
			char *session,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char *role_name,
			boolean omit_push_buttons,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down );

int exists_hidden_text_datatype(	LIST *attribute_list );

LIST *get_radio_button_element_list(
					LIST *attribute_list,
					int folder_lookup_transmit_output,
					char new_button_yn,
					boolean delete_button,
					boolean group_button,
					boolean sort_order_button );

void output_prompt_edit_form(
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			char *state,
			LIST *mto1_isa_related_folder_list,
			APPASERVER *appaserver,
			LIST *mto1_related_folder_list,
			ROLE_FOLDER *role_folder,
			char *appaserver_mount_point,
			DICTIONARY *preprompt_dictionary,
			boolean override_row_restrictions,
			LIST *prompt_recursive_folder_list,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *target_frame,
			boolean omit_new_button,
			boolean omit_delete_button,
			char *lookup_insert_folder_name );

LIST *output_prompt_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char **post_change_javascript,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			LIST *attribute_list,
			LIST *mto1_related_folder_list,
			int folder_lookup_transmit_output,
			char new_button_yn,
			char *notepad,
			LIST *exclude_attribute_name_list,
			boolean row_level_non_owner_forbid,
			DICTIONARY *preprompt_dictionary,
			LIST *prompt_recursive_folder_list,
			char *role_name,
			boolean delete_button,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			boolean group_button,
			boolean sort_order_button,
			char *state );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name, *state;
	ROLE_FOLDER *role_folder;
	APPASERVER *appaserver;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *original_post_dictionary = {0};
	ROLE *role;
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	char *target_frame;
	boolean omit_new_button = 0;
	boolean omit_delete_button = 0;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 8 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role state target_frame [post_dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];
	target_frame = argv[ 7 ];

	if ( !*target_frame ) target_frame = EDIT_FRAME;

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role =
		role_new_role(
			application_name,
			role_name );

	appaserver =
		appaserver_folder_with_load_new(
			application_name,
			session,
			folder_name );

	if ( list_length( appaserver->
				folder->
				mto1_isa_related_folder_list ) )
	{
		omit_new_button = 1;
	}

	if ( argc == 9 )
	{
		original_post_dictionary =
			dictionary_index_string2dictionary(
				argv[ 8 ] );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					original_post_dictionary,
					application_name,
					appaserver->folder->attribute_list,
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
					application_name,
					appaserver->
						folder->
						attribute_list,
					(LIST *)0 /* operation_name_list */ );
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
			dictionary_appaserver->preprompt_dictionary,
			0 /* folder_lookup_before_drop_down */ );

	lookup_before_drop_down->insert_folder_name =
		lookup_before_drop_down_get_dictionary_insert_folder_name(
			dictionary_appaserver->
				lookup_before_drop_down_dictionary );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root )
	{
		lookup_before_drop_down->first_prelookup =
			lookup_before_drop_down_first_prelookup(
				dictionary_appaserver->
					lookup_before_drop_down_dictionary );
	}

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root )
	{
		target_frame = PROMPT_FRAME;
	}
	else
	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_skipped )
	{
		target_frame = EDIT_FRAME;

		omit_new_button = 1;

		mark_ignore_for_prelookup_skipped(
			appaserver->folder->mto1_related_folder_list,
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list );
	}

	appaserver->prompt_recursive =
		prompt_recursive_new(
			application_name,
			appaserver->folder->folder_name /* query_folder_name */,
			appaserver->folder->mto1_related_folder_list );

	if ( !omit_new_button )
	{
		omit_new_button =
			related_folder_mto1_exists_drop_down_multi_select(
				appaserver->folder->mto1_related_folder_list );
	}

	role_folder =
		role_folder_new(
			application_name,
			session,
			role_name,
			folder_name );

	omit_delete_button = 
		  list_length( appaserver->
				folder->
				mto1_isa_related_folder_list );

	output_prompt_edit_form(
		application_name,
		session,
		login_name,
		appaserver->folder->folder_name,
		role_name,
		state,
		appaserver->folder->mto1_isa_related_folder_list,
		appaserver,
		appaserver->folder->mto1_related_folder_list,
		role_folder,
		appaserver_parameter_file->
			appaserver_mount_point,
		dictionary_appaserver->
			preprompt_dictionary,
		role_get_override_row_restrictions(
			role->override_row_restrictions_yn ),
		appaserver->
			prompt_recursive->
			prompt_recursive_folder_list,
		lookup_before_drop_down,
		dictionary_appaserver->
			lookup_before_drop_down_dictionary,
		target_frame,
		omit_new_button,
		omit_delete_button,
		lookup_before_drop_down->insert_folder_name );

	exit( 0 );
}

void output_prompt_edit_form(
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			char *state,
			LIST *mto1_isa_related_folder_list,
			APPASERVER *appaserver,
			LIST *mto1_related_folder_list,
			ROLE_FOLDER *role_folder,
			char *appaserver_mount_point,
			DICTIONARY *preprompt_dictionary,
			boolean override_row_restrictions,
			LIST *prompt_recursive_folder_list,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			DICTIONARY *lookup_before_drop_down_dictionary,
			char *target_frame,
			boolean omit_new_button,
			boolean omit_delete_button,
			char *lookup_insert_folder_name )
{
	LIST *exclude_attribute_name_list;
	DOCUMENT *document;
	char *post_change_javascript = {0};
	char new_button_yn;
	char *remember_keystrokes_onload_control_string = {0};
	LIST *remember_button_non_multi_element_name_list;
	LIST *remember_button_multi_element_name_list;
	char *destination_multi_select_element_name;
	char *prelookup_button_control_string = {0};
	boolean with_dynarch_menu;
	boolean omit_format_initial_capital = 0;
	boolean delete_button;
	boolean group_button;
	boolean sort_order_button;
	FORM *form;
	boolean with_prelookup_skip_button = 0;

	/* This needs to be made into a list. */
	/* ---------------------------------- */
	RELATED_FOLDER *ajax_fill_drop_down_related_folder = {0};

	form = form_new( INSERT_UPDATE_KEY,
			 application_title_string( application_name ) );

	form_set_folder_parameters(	form,
					state,
					login_name,
					application_name,
					session,
					folder_name,
					role_name );

	/* Need to implement isa relationships for the group count button. */
	/* --------------------------------------------------------------- */
	group_button = ( list_length( mto1_related_folder_list ) &&
			 !list_length( mto1_isa_related_folder_list ) );

	/* See folder_append_isa_mto1_related_folder_list() */
	/* ------------------------------------------------ */
	if ( list_rewind( mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;

		do {
			isa_related_folder =
				list_get_pointer(
					mto1_isa_related_folder_list );

			mto1_related_folder_list =
				related_folder_get_mto1_related_folder_list(
				   mto1_related_folder_list,
				   application_name,
				   session,
				   isa_related_folder->folder->folder_name,
				   role_name,
				   0 /* isa_flag */,
				   related_folder_no_recursive,
				   override_row_restrictions,
				   (LIST *)0 /* root_primary_..._name_list */,
				   0 /* recursive_level */ );

		} while( list_next( mto1_isa_related_folder_list ) );
	}

	exclude_attribute_name_list =
		appaserver_get_exclude_attribute_name_list(
			appaserver->folder->attribute_list,
			"lookup" );

	mto1_related_folder_list =
		appaserver_remove_attribute_name_list_from_related_folder_list(
			mto1_related_folder_list,
			exclude_attribute_name_list );

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
			mto1_related_folder_list );

	if ( omit_new_button ||
	     appaserver->folder->row_level_non_owner_view_only )
		new_button_yn = 'n';
	else
		new_button_yn = role_folder->insert_yn;

	if ( !preprompt_dictionary )
		preprompt_dictionary = dictionary_new();

	if ( omit_delete_button ||
	     appaserver->folder->row_level_non_owner_view_only )
		delete_button = 0;
	else
		delete_button = (role_folder->delete_yn == 'y');

	related_folder_set_ignore_output_for_duplicate(
				mto1_related_folder_list );

	if ( appaserver_library_sort_attribute_name(
			appaserver->folder->attribute_list ) )
	{
		sort_order_button = 1;
	}
	else
	{
		sort_order_button = 0;
	}

	form->regular_element_list =
		output_prompt_element_list(
			&ajax_fill_drop_down_related_folder,
			&post_change_javascript,
			login_name,
			application_name,
			session,
			folder_name,
			appaserver->folder->attribute_list,
			mto1_related_folder_list,
			1 /* appaserver->folder->lookup_transmit_output */,
			new_button_yn,
			appaserver->folder->notepad,
			exclude_attribute_name_list,
			appaserver->folder->row_level_non_owner_forbid,
			preprompt_dictionary,
			prompt_recursive_folder_list,
			role_name,
			delete_button,
			lookup_before_drop_down,
			group_button,
			sort_order_button,
			state );

	if ( !list_length( form->regular_element_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty element_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got regular_element_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
element_list_display( form->regular_element_list ) );
m2( application_name, msg );
}
*/
	/* Set other attributes */
	/* -------------------- */
	form_set_post_process( form, "post_prompt_edit_form" );
	form_set_insert_update_key( form, INSERT_UPDATE_KEY );
	form->target_frame = target_frame;

	form->html_help_file_anchor =
		appaserver->folder->html_help_file_anchor;

	document = document_new(
			application_title_string( application_name ),
			application_name );

	if ( post_change_javascript )
	{
		document->onload_control_string =
			post_change_javascript;
	}

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );

	list_rewind( form->regular_element_list );

	while ( ( destination_multi_select_element_name =
		element_get_destination_multi_select_element_name(
			form->regular_element_list ) ) )
	{
		char submit_control_string[ 128 ];

		sprintf(submit_control_string,
			"post_change_multi_select_all('%s');",
			destination_multi_select_element_name );

		form_append_submit_control_string(
				form->submit_control_string,
				submit_control_string );

		document_set_javascript_module(
			document,
			"post_change_multi_select" );
	}

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
			post_change_javascript,
			COOKIE_KEY_PREFIX,
			folder_name );

	form_append_remember_keystrokes_submit_control_string(
			&form->onclick_keystrokes_save_string,
			form,
			remember_button_non_multi_element_name_list,
			remember_button_multi_element_name_list,
			COOKIE_KEY_PREFIX,
			folder_name );

	remember_keystrokes_onload_control_string =
			form_get_onload_control_string( form );

	document_set_javascript_module(
			document,
			"verify_attribute_widths" );

	document->output_content_type = 1;

	with_dynarch_menu =
		appaserver_frameset_menu_horizontal(
			application_name,
			login_name );

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			with_dynarch_menu,
			0 /* not with_close_head */ );

	if ( attribute_exists_date_attribute(
		appaserver->folder->attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
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

		if ( system( sys_string ) ) {};
	}

	document_output_close_head( stdout );

	if ( appaserver_frameset_menu_horizontal(
			application_name,
			login_name ) )
	{
		char sys_string[ 1024 ];

		document_output_dynarch_html_body(
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				document->onload_control_string );

		printf( "<ul id=menu>\n" );

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %c %c 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				"" /* title */,
				'n' /* not content_type_yn */,
				'y' /* omit_html_head_yn */,
				appaserver_error_get_filename(
					application_name ) );

		fflush( stdout );
		if ( system( sys_string ) ) {};
		fflush( stdout );
		printf( "</ul>\n" );
	}
	else
	{
		document_output_body(
			document->application_name,
			document->onload_control_string );
	}

	/* form->subtitle_string = FORM_LOOKUP_SUBTITLE_MESSAGE; */

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root )
	{
		char form_title[ 512 ];
		char initial_capital_state[ 128 ];
		char initial_capital_folder[ 128 ];
		char initial_capital_related_folder[ 128 ];

		format_initial_capital(	initial_capital_state,
					state );
		format_initial_capital(	initial_capital_folder,
					folder_name );

		if ( lookup_insert_folder_name )
		{
			format_initial_capital(
					initial_capital_related_folder,
					lookup_insert_folder_name );
		}
		else
		{
			format_initial_capital(
					initial_capital_related_folder,
					lookup_before_drop_down->
						base_folder->
						folder_name );
		}

		if ( strcmp( state, "insert" ) == 0
		||   !lookup_before_drop_down->first_prelookup )
		{
			sprintf(form_title,
"%s %s<br><small>Prelookup %s</small>\n",
				initial_capital_state,
				initial_capital_related_folder,
				initial_capital_folder );
		}
		else
		{
			sprintf(form_title,
"%s %s<br><small>Prelookup %s:<br>If you know exactly what you are looking for, then fill out 1 or 2 rows of this query form.</small>",
				initial_capital_state,
				initial_capital_related_folder,
				initial_capital_folder );
			with_prelookup_skip_button = 1;
		}

		form->form_title = strdup( form_title );
		omit_format_initial_capital = 1;
	}
	else
	if ( strcmp( state, "insert" ) != 0
	&&   strcmp( state, "lookup" ) != 0
	&&   strcmp( state, "update" ) != 0 )
	{
		/* The process name is stored in the state variable :-( */
		/* ---------------------------------------------------- */
		char form_title[ 512 ];

		sprintf(form_title,
			"%s: Prelookup %s",
			state,
			folder_name );

		form->form_title = strdup( form_title );
	}

	form_output_title(	form->application_title,
				"lookup" /* form->state */,
				form->form_title,
				form->folder_name,
				form->subtitle_string,
				omit_format_initial_capital );

	form_append_submit_control_string(
	   form->submit_control_string,
	   appaserver_library_verify_attribute_widths_submit_control_string(
			form->regular_element_list,
			"output_prompt_edit_form" ) );

	form->table_border = 1;

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete
	||   lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_skipped )
	{
		prelookup_button_control_string =
			appaserver_library_prelookup_button_control_string(
				application_name,
				appaserver_parameter_file_get_cgi_directory(),
				appaserver_library_server_address(),
				login_name,
				session,
				folder_name,
				lookup_before_drop_down->
					base_folder->folder_name,
				role_name,
				state );
	}

	form_output_heading(	form->login_name,
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
				1 /* output_submit_reset_buttons */,
				with_prelookup_skip_button,
				form->submit_control_string,
				form->table_border,
				(char *)0 /* caption_string */,
				form->html_help_file_anchor,
				form->process_id,
				appaserver_library_server_address(),
				form->optional_related_attribute_name,
				remember_keystrokes_onload_control_string,
				(char *)0 /* post_change_javascript */ );

	form_output_row(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->regular_element_list,
		(DICTIONARY *)0 /* query_dictionary */,
		0 /* row */,
		1 /* with push buttons */,
		(FILE *)0 /* spool_file */,
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* background_color */,
		(char *)0 /* appaserver_user_foreign_login_name */,
		(LIST *)0 /* non_edit_folder_name_list */ );

	printf( "</table>\n" );

	output_dictionary_as_hidden(
		dictionary_add_prefix(
			lookup_before_drop_down_dictionary,
			LOOKUP_BEFORE_DROP_DOWN_PREFIX ) );

	output_dictionary_as_hidden(
		dictionary_add_prefix(
			preprompt_dictionary,
			PREPROMPT_PREFIX ) );

	printf( "<table border=0>\n" );

	form_output_trailer(
		0 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		prelookup_button_control_string,
		application_name,
		1 /* with_back_to_top_button, independent upon dynarch */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();
}

LIST *output_prompt_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char **post_change_javascript,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			LIST *attribute_list,
			LIST *mto1_related_folder_list,
			int folder_lookup_transmit_output,
			char new_button_yn,
			char *notepad,
			LIST *exclude_attribute_name_list,
			boolean row_level_non_owner_forbid,
			DICTIONARY *preprompt_dictionary,
			LIST *prompt_recursive_folder_list,
			char *role_name,
			boolean delete_button,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			boolean group_button,
			boolean sort_order_button,
			char *state )
{
	LIST *return_list;
	ATTRIBUTE *attribute;
	RELATED_FOLDER *related_folder;
	LIST *foreign_attribute_name_list = {0};
	LIST *element_list;
	LIST *radio_button_element_list = {0};
	LIST *isa_folder_list;
	char *hint_message;
	boolean omit_push_buttons = 0;
	LIST *done_folder_name_list = list_new();
	char *no_display_push_button_prefix = {0};
	char *no_display_push_button_heading = {0};
	boolean attribute_exists_in_preprompt_dictionary;
	FOLDER *folder;
	FOLDER *prompt_folder;
	ELEMENT_APPASERVER *element;
	char element_name[ 128 ];
	LIST *omit_update_attribute_name_list;

	folder =
		folder_new_folder(
			application_name,
			session,
			folder_name );

	omit_update_attribute_name_list =
		attribute_omit_update_attribute_name_list(
			attribute_list );

	folder->one2m_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder->folder_name,
			role_name,
			update,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /* parent_primary_attribute_name_list */,
			(LIST *)0 /* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	if (	lookup_before_drop_down->lookup_before_drop_down_state !=
		lookup_participating_not_root )
	{
		folder->join_1tom_related_folder_list =
			related_folder_join_1tom_related_folder_list(
				folder->one2m_related_folder_list );
	}

	no_display_push_button_prefix = NO_DISPLAY_PUSH_BUTTON_PREFIX;
	no_display_push_button_heading = NO_DISPLAY_PUSH_BUTTON_HEADING;

	return_list = list_new_list();

	isa_folder_list =
		appaserver_get_isa_folder_list(
			application_name );

	if ( *notepad )
	{
		ELEMENT_APPASERVER *element;

		list_append_pointer(
				return_list,
				(char *)element_non_edit_text_new_element(
					"" /* name */,
					notepad,
					99 /* column span */,
					ELEMENT_TITLE_NOTEPAD_PADDING_EM ) );

		element = element_appaserver_new( table_row, "" );

		list_append_pointer(
				return_list, 
				element );
	}
 
	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root )
	{
		omit_push_buttons = 1;
		no_display_push_button_prefix = (char *)0;
		no_display_push_button_heading = (char *)0;
	}
	else
	{
		radio_button_element_list =
			get_radio_button_element_list(
				attribute_list,
				folder_lookup_transmit_output,
				new_button_yn,
				delete_button,
				group_button,
				sort_order_button );

		if ( list_length( radio_button_element_list ) )
		{
			list_append_list(	return_list,
						radio_button_element_list );
		}
	}

	if ( !list_length( attribute_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty attribute_list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if (	lookup_before_drop_down->lookup_before_drop_down_state !=
		lookup_skipped
	&&      list_rewind( prompt_recursive_folder_list ) )
	{
		PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
		LIST *prompt_mto1_recursive_element_list;

		do {
			prompt_recursive_folder =
				list_get_pointer(
					prompt_recursive_folder_list );

			if ( lookup_before_drop_down->
				lookup_before_drop_down_state ==
					lookup_skipped
			&&   related_folder_exists_mto1_folder_name(
				prompt_recursive_folder->
				   prompt_recursive_mto1_folder_list,
				prompt_recursive_folder->
					prompt_folder->
					folder_name ) )
			{
				prompt_folder = (FOLDER *)0;
			}
			else
			{
				prompt_folder =
					prompt_recursive_folder->
						prompt_folder;
			}

			prompt_mto1_recursive_element_list =
				prompt_mto1_recursive_element_list_fetch(
					done_folder_name_list,
					post_change_javascript,
					exclude_attribute_name_list,
					prompt_folder,
					prompt_recursive_folder->
					   prompt_folder_drop_down_multi_select,
					prompt_recursive_folder->
					   prompt_recursive_mto1_folder_list,
					application_name,
					session,
					login_name,
					preprompt_dictionary,
					role_name,
					omit_push_buttons,
					lookup_before_drop_down );

			if ( prompt_mto1_recursive_element_list )
			{
				list_append_list(
					return_list,
					prompt_mto1_recursive_element_list );
			}

		} while( list_next( prompt_recursive_folder_list ) );
	}

	list_rewind( attribute_list );

	do {
		attribute = list_get_pointer( attribute_list );

		if ( list_exists_string(
			attribute->attribute_name,
			exclude_attribute_name_list ) )
		{
			continue;
		}

		attribute_exists_in_preprompt_dictionary =
			dictionary_key_exists_index_zero_or_one(
				preprompt_dictionary,
				attribute->attribute_name );

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
				&foreign_attribute_name_list,
				exclude_attribute_name_list,
				omit_update_attribute_name_list,
				mto1_related_folder_list,
				attribute->attribute_name,
			        (LIST *)0 /* include_attribute_name_list */) )
		&&   !related_folder->omit_lookup_before_drop_down )
		{
			build_related_folder_element_list(
			   	ajax_fill_drop_down_related_folder,
			   	return_list,
			   	related_folder,
			   	done_folder_name_list,
			   	isa_folder_list,
			   	attribute,
			   	lookup_before_drop_down,
			   	application_name,
			   	session,
			   	role_name,
			   	login_name,
			   	preprompt_dictionary,
		  	   	no_display_push_button_prefix,
		  	   	no_display_push_button_heading,
			   	row_level_non_owner_forbid,
			   	folder_name
					/* one2m_folder_name_for_processes */,
			 	  attribute_exists_in_preprompt_dictionary,
			   	state,
			   	foreign_attribute_name_list );

				continue;
		}

		if ( strcmp( attribute->datatype, "hidden_text" ) == 0 )
		{
			element_list = (LIST *)0;
		}
		else
		{
			element_list =
			     attribute_prompt_element_list(
				attribute->attribute_name,
				(char *)0 /* prepend_folder_name */,
				attribute->datatype,
				attribute->post_change_javascript,
				attribute->width,
				attribute->hint_message,
				attribute->primary_key_index,
				omit_push_buttons );
		}

		if ( element_list )
		{
			list_append_list(
				return_list,
				element_list );

			list_append_pointer(
				exclude_attribute_name_list,
				attribute->attribute_name );
		}

	} while( list_next( attribute_list ) );

	if ( list_rewind( mto1_related_folder_list ) )
	{
		LIST *subtract_list;

		do {
			related_folder =
				list_get_pointer(
					mto1_related_folder_list );
	
			if ( related_folder->ignore_output ) continue;

			if ( isa_folder_list
			&&   list_length( isa_folder_list )
			&&   appaserver_isa_folder_accounted_for(
				isa_folder_list,
				related_folder->folder->folder_name,
				related_folder->related_attribute_name ) )
			{
				continue;
			}
	
			foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
			   folder_get_primary_attribute_name_list(
				related_folder->folder->
					attribute_list ),
			   related_folder->related_attribute_name,
			   related_folder->folder_foreign_attribute_name_list );
	
			if ( list_length(
				list_subtract_string_list(
					foreign_attribute_name_list,
					exclude_attribute_name_list ) ) == 0 )
			{
				continue;
			}
	
			subtract_list =
				list_subtract(
					foreign_attribute_name_list,
					omit_update_attribute_name_list );

			if (	list_length( subtract_list ) !=
				list_length( foreign_attribute_name_list ) )
			{
				continue;
			}

			hint_message =
				related_folder_get_hint_message(
					attribute->hint_message,
					related_folder->hint_message,
					related_folder->folder->notepad );

			list_append_list(
				return_list,
				related_folder_prompt_element_list(
				   (RELATED_FOLDER **)0
				       /* ajax_fill_drop_down_related_folder */,
				   application_name,
				   session,
				   role_name,
				   login_name,
				   related_folder->folder->folder_name,
				   related_folder->folder->
					populate_drop_down_process,
				   related_folder->folder->
					attribute_list,
				   foreign_attribute_name_list,
				   0 /* dont omit_ignore_push_buttons */,
				   preprompt_dictionary,
		  		   no_display_push_button_heading,
		  		   no_display_push_button_prefix,
				   (char *)0 /* post_change_java... */,
				   hint_message,
				   0 /* max_drop_down_size */,
				   (LIST *)0 /* common_non_primary_a... */,
				   0 /* not is_primary_attribute */,
				   related_folder->folder->
					row_level_non_owner_view_only,
				   related_folder->folder->
					row_level_non_owner_forbid,
			   	   related_folder->
					related_attribute_name,
			   	   related_folder->
					drop_down_multi_select,
				   related_folder->
					folder->
					no_initial_capital,
				   (char *)0 /* state */,
				   (char *)0 /* one2m_folder_name...processes*/,
				   0 /* tab_index */,
				   0 /* not set_first_initial_data */,
				   1 /* output_null_option */,
				   1 /* output_not_null_option */,
				   1 /* output_select_option */,
				   (char *)0
				   /* appaserver_user_foreign_login_name */,
			           related_folder->omit_lookup_before_drop_down
				   ) );
	
		} while( list_next( mto1_related_folder_list ) );
	}

	if ( list_rewind( folder->join_1tom_related_folder_list ) )
	{
		RELATED_FOLDER *related_folder;

		do {
			related_folder =
				list_get(
					folder->join_1tom_related_folder_list );

			/* Make two line breaks */
			/* -------------------- */
			element = element_appaserver_new( linebreak, "" );

			list_set( return_list, element );
			list_set( return_list, element );

			/* Make the push button element. */
			/* ----------------------------- */
			sprintf( element_name,
		 		 "%s%s",
				 NO_DISPLAY_PUSH_BUTTON_PREFIX,
				 related_folder->
					one2m_folder->
					folder_name );

			element =
				element_appaserver_new(
					toggle_button, 
					strdup( element_name ) );

			element_toggle_button_set_heading(
				element->toggle_button,
				NO_DISPLAY_PUSH_BUTTON_HEADING );

			list_append_pointer(
					return_list, 
					element );

			/* Make the prompt. */
			/* ---------------- */
			element =
				element_appaserver_new(
					prompt,
				 	related_folder->
						one2m_folder->
						folder_name );

			list_append_pointer(
					return_list, 
					element );

		} while( list_next( folder->join_1tom_related_folder_list ) );
	}

	return return_list;
}

LIST *get_radio_button_element_list(
					LIST *attribute_list,
					int folder_lookup_transmit_output,
					char new_button_yn,
					boolean delete_button,
					boolean group_button,
					boolean sort_order_button )
{
	LIST *return_list = list_new_list();
	int output_table_row_boolean = 0;
	ELEMENT_APPASERVER *element;
	char onclick_function[ 512 ];

	/* Create the button to set all the no display buttons. */
	/* ---------------------------------------------------- */
	element = element_appaserver_new( toggle_button, "" );

	element->toggle_button->heading = NO_DISPLAY_PUSH_BUTTON_HEADING;

	sprintf( onclick_function,
		 "timlib_set_all_push_buttons( this, '%s' )",
		 NO_DISPLAY_PUSH_BUTTON_PREFIX );

	element->toggle_button->onclick_function = strdup( onclick_function );

	list_set( return_list, element );

	element = element_appaserver_new( table_opening, "" );

	list_set( return_list, element );

	/* Create the statistics button */
	/* ---------------------------- */
	element =
		element_appaserver_new(
			radio_button, 
			LOOKUP_OPTION_RADIO_BUTTON_NAME );

	element->radio_button->value = 
				STATISTICS_PUSH_BUTTON_NAME;

	element->radio_button->heading = "Statistics";

	list_append_pointer(
			return_list, 
			element );

	output_table_row_boolean = 1;

/*
	if ( ( attribute_list_get_primary_datatype_attribute_string_list(
						attribute_list,
						"date" ) 
	||   attribute_list_get_primary_datatype_attribute_string_list(
						attribute_list,
						"current_date" )
	||   attribute_list_get_primary_datatype_attribute_string_list(
						attribute_list,
						"current_date_time" )
	||   attribute_list_get_primary_datatype_attribute_string_list(
						attribute_list,
						"date_time" )
		)
*/
	/* Create the time chart button */
	/* ---------------------------- */
	if ( attribute_list_exists_lookup_time_chart_output( attribute_list ) )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}
	
		/* First present "Grace Chart" */
		/* --------------------------- */
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = 
					GRACE_CHART_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = "Grace Chart";
	
		list_append_pointer(
				return_list, 
				element );

		/* Second present "Google Chart" */
		/* ----------------------------- */
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = 
					GOOGLE_CHART_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = "Google Chart";
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	
	} /* if time chart button */

	/* Create the histogram button (maybe) */
	/* ----------------------------------- */
	if ( attribute_list_exists_lookup_histogram_output( attribute_list ) )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}
	
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = 
					HISTOGRAM_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = "Histogram";
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	
	} /* if histogram button */

	/* Create the group button (maybe) */
	/* ------------------------------- */
	if ( group_button )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}
	
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value =
					GROUP_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = "Group Count";
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	
	} /* if group button */

	/* Create the transmit button (maybe) */
	/* ---------------------------------- */
	if ( folder_lookup_transmit_output )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}
	
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = 
					TRANSMIT_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = TRANSMIT_PUSH_BUTTON_LABEL;
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	
	} /* if folder_lookup_transmit_output */

	/* Create the new button (maybe) */
	/* ----------------------------- */
	if ( new_button_yn == 'y' )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}

		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value =
					RADIO_NEW_PUSH_BUTTON_NAME;
	
		element_radio_button_set_heading(
			element->radio_button,
			INSERT_NEW_PUSH_BUTTON_LABEL );
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	}

	/* Create the delete radio button (maybe) */
	/* -------------------------------------- */
	if ( delete_button )
	{
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = DELETE_PUSH_BUTTON_NAME;
		element->radio_button->heading = "delete";
		element->radio_button->checked = 0;
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	}

	/* Create the sort_order button */
	/* ---------------------------- */
	if ( sort_order_button )
	{
		if ( !output_table_row_boolean )
		{
			element = element_appaserver_new( linebreak, "" );
			list_append_pointer(
					return_list, 
					element );
		}
	
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = SORT_ORDER_PUSH_BUTTON_NAME;
	
		element->radio_button->heading = "Sort Order";
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;

	} /* if sort_order button */

	/* Create the lookup radio button (maybe) */
	/* -------------------------------------- */
	if ( output_table_row_boolean )
	{
		element =
			element_appaserver_new(
				radio_button,
				LOOKUP_OPTION_RADIO_BUTTON_NAME );

		element->radio_button->value = "lookup";
		element->radio_button->heading = "lookup";
		element->radio_button->checked = 1;
	
		list_append_pointer(
				return_list, 
				element );

		output_table_row_boolean = 1;
	}

	element = element_appaserver_new( table_closing, "" );
	list_append_pointer(
			return_list, 
			element );

#ifdef NOT_DEFINED
# Retired 2020-08-03
int folder_lookup_email_output,
	/* Create the email_output text field (maybe) */
	/* ------------------------------------------ */
	if ( folder_lookup_email_output )
	{
		element = element_appaserver_new( linebreak, "" );
		list_append_pointer(
				return_list, 
				element );

		element = element_appaserver_new( empty_column, "" );
		list_append_pointer(
				return_list, 
				element );

		element = element_appaserver_new( 
				prompt,
				EMAIL_OUTPUT_NAME );
		list_append_pointer(
				return_list, 
				element );

		element = element_appaserver_new(
				javascript_filename,
				"/javascript/jquery_min.js" );
		list_append_pointer(
				return_list, 
				element );

		element = element_appaserver_new(
				javascript_filename,
				"/javascript/show_hide_radio_button.js" );
		list_append_pointer(
				return_list, 
				element );

		element =
			element_appaserver_new(
				text_item,
				EMAIL_OUTPUT_NAME );
		element->text_item->attribute_width = EMAIL_TEXT_ITEM_WIDTH;
		element->text_item->post_change_javascript =
			"show_hide_radio_button( this );";
		list_append_pointer( 	return_list,
					element );

	} /* if folder_lookup_email_output */
#endif

	return return_list;
}

int exists_hidden_text_datatype( LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute =
				(ATTRIBUTE *)
					list_get_pointer(
						attribute_list );
			if ( strcmp( attribute->datatype, "hidden_text" ) == 0 )
				return 1;
		} while( list_next( attribute_list ) );
	}
	return 0;
}

LIST *prompt_mto1_recursive_element_list_fetch(
				LIST *done_folder_name_list,
				char **post_change_javascript,
				LIST *exclude_attribute_name_list,
				FOLDER *prompt_folder,
				boolean prompt_folder_drop_down_multi_select,
				LIST *prompt_recursive_mto1_folder_list,
				char *application_name,
				char *session,
				char *login_name,
				DICTIONARY *preprompt_dictionary,
				char *role_name,
				boolean omit_push_buttons,
				LOOKUP_BEFORE_DROP_DOWN
					*lookup_before_drop_down )
{
	LIST *recursive_element_list;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	char *no_display_push_button_prefix = {0};
	char *no_display_push_button_heading = {0};
	boolean set_first_initial_data;
	boolean output_null_option;
	boolean output_not_null_option;
	/* boolean output_select_option; */

	if ( !omit_push_buttons )
	{
		no_display_push_button_prefix = NO_DISPLAY_PUSH_BUTTON_PREFIX;
		no_display_push_button_heading = NO_DISPLAY_PUSH_BUTTON_HEADING;
	}

	recursive_element_list = list_new_list();

	if ( prompt_folder )
	{
		list_append_pointer(
			done_folder_name_list,
			strdup( get_done_folder_name(
				prompt_folder->folder_name,
				(char *)0 /* related_attribute_name */ ) ) );
	}

	*post_change_javascript =
		prompt_recursive_get_gray_drop_downs_javascript(
			prompt_folder,
			prompt_folder_drop_down_multi_select,
			prompt_recursive_mto1_folder_list );

	if ( prompt_folder
	&&   lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		prompt_folder->folder_name ) )
	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		/* output_select_option = 0; */
	}
	else
	if ( prompt_folder
	&&   lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		prompt_folder->folder_name ) )
	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		/* output_select_option = 1; */
	}
	else
	{
		set_first_initial_data = 0;
		output_null_option = 1;
		output_not_null_option = 1;
		/* output_select_option = 1; */
	}

	if ( prompt_folder )
	{
		list_append_list(
			recursive_element_list,
			related_folder_prompt_element_list(
				(RELATED_FOLDER **)0,
				application_name,
				session,
				role_name,
				login_name,
				prompt_folder->folder_name,
				prompt_folder->populate_drop_down_process,
				prompt_folder->attribute_list,
				attribute_primary_attribute_name_list(
					prompt_folder->attribute_list ),
				0 /* dont omit_ignore_push_buttons */,
				preprompt_dictionary,
	  			no_display_push_button_prefix,
	  			no_display_push_button_heading,
				*post_change_javascript,
				prompt_folder->notepad /* hint_message */,
				0 /* max_drop_down_size */,
				(LIST *)0 /* common_non_primary_a... */,
				0 /* not is_primary_attribute */,
				prompt_folder->row_level_non_owner_view_only,
				prompt_folder->row_level_non_owner_forbid,
				(char *)0 /* related_attribute_name */,
				prompt_folder_drop_down_multi_select,
				prompt_folder->no_initial_capital,
				(char *)0 /* state */,
				(char *)0 /* one2m_folder_name_for_processes */,
				0 /* tab_index */,
				set_first_initial_data,
				output_null_option,
				output_not_null_option,
				1 /* output_select_option */,
				(char *)0
				/* appaserver_user_foreign_login_name */,
			        0 /* not omit_lookup_before_drop_down */
				) );
	
		list_append_string_list(
			exclude_attribute_name_list,
			attribute_primary_attribute_name_list(
				prompt_folder->attribute_list ) );
	}

	if ( !list_rewind( prompt_recursive_mto1_folder_list ) )
		return recursive_element_list;

	do {
		prompt_recursive_mto1_folder =
			list_get_pointer( prompt_recursive_mto1_folder_list );

		list_append_pointer(
			done_folder_name_list,
			strdup( get_done_folder_name(
				prompt_recursive_mto1_folder->
					folder->
					folder_name,
				(char *)0 /* related_attribute_name */ ) ) );

		list_append_list(
			recursive_element_list,
			related_folder_prompt_element_list(
				(RELATED_FOLDER **)0,
				application_name,
				session,
				role_name,
				login_name,
				prompt_recursive_mto1_folder->
					folder->
					folder_name,
				prompt_recursive_mto1_folder->
					folder->
					populate_drop_down_process,
				prompt_recursive_mto1_folder->
					folder->
					attribute_list,
				attribute_primary_attribute_name_list(
					prompt_recursive_mto1_folder->
						folder->
						attribute_list ),
				0 /* dont omit_ignore_push_buttons */,
				preprompt_dictionary,
	  			no_display_push_button_prefix,
	  			no_display_push_button_heading,
				*post_change_javascript,
				prompt_recursive_mto1_folder->
					folder->
					notepad,
				0 /* max_drop_down_size */,
				(LIST *)0 /* common_non_primary_a... */,
				0 /* not is_primary_attribute */,
				prompt_recursive_mto1_folder->
					folder->
					row_level_non_owner_view_only,
				prompt_recursive_mto1_folder->
					folder->
					row_level_non_owner_forbid,
				(char *)0 /* related_attribute_name */,
				0 /* drop_down_multi_select */,
				prompt_recursive_mto1_folder->
					folder->
					no_initial_capital,
				(char *)0 /* state */,
				(char *)0 /* one2m_folder_name_for_processes */,
				0 /* tab_index */,
				0 /* not set_first_initial_data */,
				1 /* output_null_option */,
				1 /* output_not_null_option */,
				1 /* output_select_option */,
				(char *)0
				/* appaserver_user_foreign_login_name */,
			        0 /* not omit_lookup_before_drop_down */
				) );

			list_append_string_list(
				exclude_attribute_name_list,
				attribute_primary_attribute_name_list(
					prompt_recursive_mto1_folder->
						folder->
						attribute_list ) );

	} while( list_next( prompt_recursive_mto1_folder_list ) );

	return recursive_element_list;
}

LIST *get_preprompt_attribute_name_list(
				DICTIONARY *preprompt_dictionary )
{
	LIST *preprompt_attribute_name_list;
	LIST *key_list;
	char *key;
	char *key_without_index;
	char buffer[ 1024 ];

	if ( !preprompt_dictionary
	||   !dictionary_length( preprompt_dictionary ) )
	{
		return (LIST *)0;
	}

	key_list = dictionary_get_key_list( preprompt_dictionary );
	preprompt_attribute_name_list = list_new_list();
	list_rewind( key_list );
	do {
		key = list_get_pointer( key_list );
		key_without_index = trim_index( buffer, key );
		if ( strncmp(	key_without_index,
				"relation_operator_",
				18 ) != 0 )
		{
			list_append_pointer(	preprompt_attribute_name_list,
						strdup( key_without_index ) );
		}
	} while( list_next( key_list ) );

	return preprompt_attribute_name_list;

}

void mark_ignore_for_prelookup_skipped(
		LIST *mto1_related_folder_list,
		LIST *lookup_before_drop_down_folder_list )
{
	LIST *unfulfilled_folder_name_list;
	RELATED_FOLDER *related_folder;

	unfulfilled_folder_name_list =
		lookup_before_drop_down_get_unfulfilled_folder_name_list(
			lookup_before_drop_down_folder_list );

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		if ( list_exists_string(
			related_folder->folder->folder_name,
			unfulfilled_folder_name_list )
		&&   list_length( related_folder->
					folder->
					primary_attribute_name_list ) > 1 )
		{
			related_folder->ignore_output = 1;
		}

	} while( list_next( mto1_related_folder_list ) );

}

char *get_done_folder_name(	char *folder_name,
				char *related_attribute_name )
{
	static char done_folder_name[ 128 ];

	if (	related_attribute_name
	&& 	*related_attribute_name
	&&	strcmp( related_attribute_name, "null" ) != 0 )
	{
		sprintf(	done_folder_name,
				"%s|%s",
				folder_name,
				related_attribute_name );
	}
	else
	{
		strcpy( done_folder_name, folder_name );
	}
	return strdup( done_folder_name );
}

#ifdef NOT_DEFINED
boolean get_omit_delete_button(
			char *application_name,
			FOLDER *folder,
			ROLE *role,
			char *login_name )
{
	boolean return_value;

	row_security =
		row_security_new(
			application_name,
			folder,
			role,
			login_name,
			(char *)0 /* state */,
			(DICTIONARY *)0 /* preprompt_dictionary */,
			(DICTIONARY *)0 /* query_dictionary */,
			(DICTIONARY *)0 /* sort_dictionary */,
			(LIST *)0 /* ignore_attribute_name_list */ );

	if ( row_security_supervisor_logged_in(
		row_security->row_security_state ) )
	{
		return_value = 0;
	}
	else
	{
		if ( row_security_role_update_fetch(
				row_security->role_update_list,
				folder_name ) )
		{
			return_value = 1;
		}
		else
		{
			return_value = 0;
		}
	}

	return return_value;
}
#endif

void build_related_folder_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			LIST *element_list,
			RELATED_FOLDER *related_folder,
			LIST *done_folder_name_list,
			LIST *isa_folder_list,
			ATTRIBUTE *attribute,
			LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down,
			char *application_name,
			char *session,
			char *role_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
		  	char *no_display_push_button_prefix,
		  	char *no_display_push_button_heading,
			boolean row_level_non_owner_forbid,
			char *one2m_folder_name_for_processes,
			boolean attribute_exists_in_preprompt_dictionary,
			char *state,
			LIST *foreign_attribute_name_list )
{
	char *hint_message;
	boolean set_first_initial_data;
	boolean output_null_option;
	boolean output_not_null_option;
	boolean output_select_option;
	boolean drop_down_multi_select;
	DICTIONARY *send_preprompt_dictionary;
	RELATED_FOLDER **only_one_ajax_fill_drop_down;

	if ( related_folder->ignore_output )
	{
		return;
	}

	if ( isa_folder_list
	&&   list_length( isa_folder_list )
	&&   appaserver_isa_folder_accounted_for(
		isa_folder_list,
		related_folder->folder->folder_name,
		related_folder->
			related_attribute_name ) )
	{
		return;
	}

	if ( list_exists_string(
		get_done_folder_name(
			related_folder->
				folder->folder_name,
			related_folder->
			related_attribute_name ),
		done_folder_name_list ) )
	{
		return;
	}

	list_append_pointer(
		done_folder_name_list,
		strdup( get_done_folder_name(
			related_folder->
				folder->folder_name,
			related_folder->
			related_attribute_name ) ) );

	hint_message =
		related_folder_get_hint_message(
			attribute->hint_message,
			related_folder->hint_message,
			related_folder->folder->notepad );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_not_root
	&&   ( lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		related_folder->folder->folder_name )
	||      attribute_exists_in_preprompt_dictionary ) )

	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		output_select_option = 0;
	}
	else
	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_participating_is_root_all_complete
	&&   lookup_before_drop_down_is_fulfilled_folder(
		lookup_before_drop_down->
			lookup_before_drop_down_folder_list,
		related_folder->folder->folder_name ) )
	{
		set_first_initial_data = 1;
		output_null_option = 0;
		output_not_null_option = 0;
		output_select_option = 1;
	}
	else
	{
		set_first_initial_data = 0;
		output_null_option = 1;
		output_not_null_option = 1;
		output_select_option = 1;
	}

	drop_down_multi_select =
		related_folder->
			drop_down_multi_select;

	send_preprompt_dictionary = preprompt_dictionary;

/* ---------------------------------------------------- */
/* This broke <Insert> <Reoccurring Transaction> 	*/
/* Test this: <Lookup> <Project Principle Investigator>	*/
/* ---------------------------------------------------- */
/*
	if ( ajax_fill_drop_down_related_folder
	&&   *ajax_fill_drop_down_related_folder )
	{
		only_one_ajax_fill_drop_down = (RELATED_FOLDER **)0;
	}
	else
	{
		only_one_ajax_fill_drop_down =
			ajax_fill_drop_down_related_folder;
	}
*/

	only_one_ajax_fill_drop_down =
		ajax_fill_drop_down_related_folder;

	list_append_list(
		element_list,
		related_folder_prompt_element_list(
			only_one_ajax_fill_drop_down,
			application_name,
			session,
			role_name,
			login_name,
			related_folder->folder->folder_name,
			related_folder->folder->
				populate_drop_down_process,
			related_folder->folder->
				attribute_list,
			foreign_attribute_name_list,
			0 /* dont omit_ignore_push_buttons */,
			send_preprompt_dictionary,
	  		no_display_push_button_prefix,
	  		no_display_push_button_heading,
			(char *)0 /* post_change_java... */,
			hint_message,
			0 /* max_drop_down_size */,
			(LIST *)0 /* common_non_primary_a... */,
			0 /* not is_primary_attribute */,
			0 /* row_level_non_owner_view_only */,
			row_level_non_owner_forbid,
			related_folder->
				related_attribute_name,
			drop_down_multi_select,
			related_folder->
				folder->
				no_initial_capital,
			state,
			one2m_folder_name_for_processes,
			0 /* tab_index */,
			set_first_initial_data,
			output_null_option,
			output_not_null_option,
			output_select_option,
			(char *)0
			/* appaserver_user_foreign_login_name */,
			related_folder->omit_lookup_before_drop_down
			) );

	related_folder->ignore_output = 1;
}

