/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_edit_table_form.c		*/
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
#include "String.h"
#include "timlib.h"
#include "list.h"
#include "list_usage.h"
#include "form.h"
#include "folder.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "document.h"
#include "application.h"
#include "dictionary.h"
#include "query.h"
#include "piece.h"
#include "role_folder.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "dictionary2file.h"
#include "attribute.h"
#include "session.h"
#include "role.h"
#include "lookup_before_drop_down.h"
#include "row_security.h"
#include "dictionary_separate.h"
#include "edit_table_form.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_edit_table_form(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			DICTIONARY *post_dictionary,
			char *appaserver_mount_point );

LIST *subtract_join_1tom_ignore_dictionary_related_folder_list(
			LIST *join_1tom_related_folder_list,
			DICTIONARY *ignore_dictionary );

int get_rows_inserted(	DICTIONARY *non_prefixed_dictionary );

int get_cells_updated(
			DICTIONARY *non_prefixed_dictionary );

char *get_cells_updated_folder_name_list_string(
			DICTIONARY *non_prefixed_dictionary );

char *get_results_string(
			DICTIONARY *non_prefixed_dictionary );

int main( int argc, char **argv )
{
	char *login_name;
	char *application_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *state_for_heading;
	char *insert_update_key;
	char *target_frame;
	DOCUMENT *document;
	char decoded_dictionary_string[ STRING_INPUT_BUFFER ];
	char dictionary_string[ STRING_INPUT_BUFFER ];
	DICTIONARY *original_post_dictionary = {0};
	FORM *form;
	FOLDER *folder;
	int number_rows_outputted = 0;
	char *results_string;
	OPERATION_LIST_STRUCTURE *operation_list_structure;
	ROLE_FOLDER *role_folder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int output_submit_reset_buttons_in_trailer = 0;
	int output_submit_reset_buttons_in_heading = 0;
	char detail_base_folder_name[ 128 ];
	char action_string[ 512 ];
	ROLE *role;
	boolean with_dynarch_menu = 0;
	LIST *non_edit_folder_name_list = {0};
	ROW_SECURITY *row_security;
	DICTIONARY_SEPARATE *dictionary_separate;
	char content_type_yn = {0};
	int rows_inserted;
	int cells_updated;
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	boolean make_primary_keys_non_edit = 0;
	char *appaserver_user_foreign_login_name;
	/* --------------------------------------------------------------- */
	/* Operation_list_list is only defined to decide on which buttons. */
	/* Row security gets its own operation_list.			   */
	/* --------------------------------------------------------------- */
	LIST *operation_list;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name session folder role insert_update_key target_frame [content_type_yn]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 2 ];
	folder_name = argv[ 3 ];
	role_name = argv[ 4 ];
	insert_update_key = argv[ 5 ];
	target_frame = argv[ 6 ];

	if ( argc == 8 ) content_type_yn = *argv[ 7 ];

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( string_input( dictionary_string, stdin, STRING_INPUT_BUFFER ) )
	{
		string_decode_html_post(
			decoded_dictionary_string, 
			dictionary_string );

		post_dictionary =
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: string_input() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	output_edit_table_form(
		application_name,
		login_name,
		session,
		folder_name,
		role_name,
		insert_update_key,
		target_frame,
		post_dictionary,
		appaserver_parameter_file->appaserver_mount_point );

	return 0;
}

void output_edit_table_form(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			DICTIONARY *post_dictionary,
			char *appaserver_mount_point )
{
	EDIT_TABLE_FORM *edit_table_form =
		edit_table_form_fetch(
			application_name,
			login_name,
			session,
			folder_name,
			role_name,
			insert_update_key,
			target_frame,
			post_dictionary );

	if ( !edit_table_form )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: edit_table_form_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );

	}

	folder->join_1tom_related_folder_list =
		subtract_join_1tom_ignore_dictionary_related_folder_list(
			folder->join_1tom_related_folder_list,
			dictionary_separate->ignore_dictionary );

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			dictionary_separate->
				lookup_before_drop_down_dictionary,
			state );

	lookup_before_drop_down->lookup_before_drop_down_state =
		lookup_before_drop_down_get_state(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			dictionary_separate->
				lookup_before_drop_down_dictionary,
			dictionary_separate->preprompt_dictionary,
			folder->lookup_before_drop_down );

	rows_inserted =
		get_rows_inserted(
			dictionary_separate->
				non_prefixed_dictionary );

	cells_updated =
		get_cells_updated(
			dictionary_separate->
				non_prefixed_dictionary );

	results_string =
		get_results_string(
			dictionary_separate->
				non_prefixed_dictionary );

	/* If come from detail */
	/* ------------------- */
	if ( instr( "!", insert_update_key, 1 ) > -1 )
	{
		piece( detail_base_folder_name, '!', insert_update_key, 1 );

		insert_update_key
			[ instr( "!", insert_update_key, 1 ) ] = '\0';
	}
	else
	{
		*detail_base_folder_name = '\0';
	}

	ignore_attribute_name_list = list_new_list();

	/* Change state (maybe) */
	/* -------------------- */
	if ( strcmp( state, "query" ) == 0 || strcmp( state, "lookup" ) == 0 )
	{
		state = "update";
	}

	/* Note: this must follow previous statement */
	/* ----------------------------------------- */
	if ( role_folder->update_yn != 'y' )
	{
		state = "lookup";
	}

	form =
		form_new(
			folder_name,
			application_title_string(
				application_name ) );

	if ( rows_inserted )
	{
		char subtitle_string[ 32 ];

		sprintf( subtitle_string, "Rows inserted: %d", rows_inserted );
		form->subtitle_string = strdup( subtitle_string );
	}
	else
	if ( cells_updated )
	{
		char *cells_updated_folder_name_list_string;
		char subtitle_string[ 1024 ];

		cells_updated_folder_name_list_string =
			get_cells_updated_folder_name_list_string(
				dictionary_separate->
					non_prefixed_dictionary );

		if ( !cells_updated_folder_name_list_string
		&&   !*cells_updated_folder_name_list_string )
		{
			sprintf(subtitle_string,
				"Cells updated: %d",
				cells_updated );
		}
		else
		{
			char buffer[ 1024 ];

			sprintf(subtitle_string,
				"Cells updated: %d in %s",
				cells_updated,
				format_initial_capital(
				 buffer,
				 cells_updated_folder_name_list_string ) );
		}

		form->subtitle_string = strdup( subtitle_string );
	}

	operation_list_structure =
		operation_list_structure_new(
			application_name,
			session,
			folder_name,
			role_name,
			dont_omit_delete );

	operation_list = operation_list_structure->operation_list;

	if ( strcmp( state, "update" ) == 0 
	||   list_length( operation_list ) )
	{
		output_submit_reset_buttons_in_heading = 1;
	}

	document = document_new( insert_update_key, application_name );

	if ( strcmp( target_frame, PROMPT_FRAME ) == 0
	&&   appaserver_frameset_menu_horizontal(
		application_name,
		login_name ) )
	{
		with_dynarch_menu = 1;
	}

	if ( with_dynarch_menu )
	{
		char sys_string[ 1024 ];

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' '%c' 2>>%s",
			application_name,
			session,
			login_name,
			role_name,
			"" /* title */,
			(content_type_yn) ? content_type_yn : 'y',
			appaserver_error_get_filename(
				application_name ) );
		if ( system( sys_string ) ) {};
		fflush( stdout );
	}
	else
	{
		document->output_content_type = 1;
	}

	form_set_post_process( form, "post_edit_table_form" );
	form_set_current_row( form, 1 );
	form->target_frame = target_frame;

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "null2slash" );
	document_set_javascript_module( document, "push_button_submit" );
	document_set_javascript_module( document, "form" );

	document_set_folder_javascript_files(
		document,
		application_name,
		folder_name );

	document_output_head_stream(
		stdout,
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_parameter_file->appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		with_dynarch_menu,
		0 /* not with_close_head */ );

	if ( attribute_exists_date_attribute(
		folder->attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
	}

	document_output_close_head( stdout );

	if ( folder->post_change_javascript
	&&   *folder->post_change_javascript )
	{
		char post_change_javascript[ 1024 ];

		strcpy(	post_change_javascript,
			folder->post_change_javascript );
		
		search_replace_string(
			post_change_javascript,
			"$state",
			"update" );

		document->onload_control_string =
			document_set_onload_control_string(
				document->onload_control_string,
				form_set_post_change_javascript_row_zero(
					post_change_javascript ) );
	}

	document_output_body(
		document->application_name,
		document->onload_control_string );

	if ( strcmp( state, "lookup" ) == 0 )
		state_for_heading = "view";
	else
		state_for_heading = state;

	form_set_folder_parameters(
		form,
		state_for_heading,
		login_name,
		application_name,
		session,
		folder->folder_name,
		role_name );

	form_output_title(
		form->application_title,
		form->state,
		form->form_title,
		form->folder_name,
		form->subtitle_string,
		0 /* not omit_format_initial_capital */ );

	if ( results_string ) printf( "%s\n", results_string );

	form->table_border = 1;
	form->insert_update_key = insert_update_key;
	form->target_frame = target_frame;

	if ( *detail_base_folder_name )
	{
		sprintf(	action_string,
		"%s/post_edit_table_form?%s+%s+%s+%s+%s+%s+detail!%s+%s+%d+",
				appaserver_parameter_file_get_cgi_directory(),
				login_name,
				application_name,
				session,
				folder->folder_name,
				role_name,
				state,
				detail_base_folder_name,
				target_frame,
				getpid() );

		form->action_string = action_string;
	}

	form->html_help_file_anchor = folder->html_help_file_anchor;
	form->process_id = getpid();

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
		output_submit_reset_buttons_in_heading,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		folder->post_change_javascript );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_skipped )
	{
		non_edit_folder_name_list =
			lookup_before_drop_down_get_non_edit_folder_name_list(
				lookup_before_drop_down->
					lookup_before_drop_down_folder_list,
				folder->mto1_related_folder_list );
	}

	row_security =
		row_security_new(
			application_name,
			folder,
			role,
			login_name,
			state,
			dictionary_separate->
				preprompt_dictionary,
			dictionary_separate->
				query_dictionary,
			dictionary_separate->
				sort_dictionary,
			appaserver_no_display_pressed_attribute_name_list(
				dictionary_separate->
					ignore_dictionary, 
				dictionary_separate->
					query_dictionary, 
				folder->attribute_name_list ) );

	row_security->folder->join_1tom_related_folder_list =
		folder->join_1tom_related_folder_list;

	row_security->row_security_element_list_structure =
		row_security_edit_table_structure_new(
			row_security->query_dictionary,
			application_name,
			row_security->row_security_state,
			row_security->folder,
			row_security->role,
			row_security->login_name,
			row_security->state,
			row_security->sort_dictionary,
			row_security->
				no_display_pressed_attribute_name_list,
			row_security->attribute_not_null_join,
			row_security->attribute_not_null_folder,
			row_security->foreign_login_name_folder,
			make_primary_keys_non_edit,
			omit_delete_dont_care,
			0 /* omit_operation_buttons */,
			0 /* not ajax_fill_drop_down_omit */,
			row_security->row_security_is_participating );

	form->regular_element_list =
		row_security->
			row_security_element_list_structure->
			regular_element_list;

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: regular_element_list = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
element_appaserver_list_display( form->regular_element_list ) );
m2( application_name, msg );
}
*/
	form->viewonly_element_list =
		row_security->
			row_security_element_list_structure->
			viewonly_element_list;

	if ( row_security->row_security_element_list_structure->
		ajax_fill_drop_down_related_folder )
	{
		char sys_string[ 1024 ];
		RELATED_FOLDER *ajax_fill_drop_down_related_folder;

		ajax_fill_drop_down_related_folder =
			row_security->row_security_element_list_structure->
			ajax_fill_drop_down_related_folder;

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
				primary_key_list ) /* select */ );

		if ( system( sys_string ) ) {};
	}

	if ( list_length(
		row_security->
			row_security_element_list_structure->
			row_dictionary_list ) >=
		FOLDER_MIN_ROWS_SORT_BUTTONS )
	{
		element_list_set_omit_heading_sort_button(
			form->regular_element_list,
			folder->join_1tom_related_folder_list );

		form_output_sort_buttons(
				stdout,
				form->regular_element_list,
				form->form_name );
	}

	form_output_table_heading(
		form->regular_element_list,
		0 /* form_number */ );

	form->row_dictionary_list =
		row_security->
			row_security_element_list_structure->
			row_dictionary_list;

	if ( strcmp(	row_security->folder->folder_name,
			"appaserver_user" ) == 0 )
	{
		appaserver_user_foreign_login_name = "login_name";
	}
	else
	{
		appaserver_user_foreign_login_name =
			related_folder_appaserver_user_foreign_login_name(
				row_security->
					folder->
					mto1_append_isa_related_folder_list );
	}

	if ( row_security->row_security_state == lookup_only )
	{
		form->regular_element_list = (LIST *)0;
	}

	number_rows_outputted +=
		form_output_body(
			&form->current_reference_number,
			form->hidden_name_dictionary,
			form->output_row_zero_only,
			form->row_dictionary_list,
			form->regular_element_list,
			form->viewonly_element_list,
			dictionary2file_get_filename(
				form->process_id,
				appaserver_parameter_file->
					appaserver_data_directory,
				insert_update_key,
				folder_name,
				(char *)0 /* optional_related... */ ),
			folder->row_level_non_owner_view_only,
			application_name,
			login_name,
			row_security->attribute_not_null_string,
			appaserver_user_foreign_login_name,
			non_edit_folder_name_list );

	if ( number_rows_outputted >= ROWS_FOR_SUBMIT_AT_BOTTOM
	&& ( strcmp( state, "lookup" ) != 0
	||   list_length( operation_list ) ) )
	{
		output_submit_reset_buttons_in_trailer = 1;
	}

	printf( "</table>\n" );

	dictionary_separate_output_as_hidden(
		dictionary_separate,
		0 /* not with_prefixed_dictionary */ );

	printf( "<table border=0>\n" );

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		1 /* with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		/* ------------------------------------------------------ */
		/* Sets row zero for javascript to loop through each row. */
		/* ------------------------------------------------------ */
		folder->post_change_javascript );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();

	return 0;

}

int get_rows_inserted( DICTIONARY *non_prefixed_dictionary )
{
	char *rows_inserted_string;

	if ( ( rows_inserted_string =
		dictionary_get_pointer(
			non_prefixed_dictionary,
			ROWS_INSERTED_COUNT_KEY	) ) )
	{
		dictionary_set_pointer(	non_prefixed_dictionary,
					ROWS_INSERTED_COUNT_KEY,
					"0" );
		return atoi( rows_inserted_string );
	}
	else
	{
		return 0;
	}
}

int get_cells_updated( DICTIONARY *non_prefixed_dictionary )
{
	char *cells_updated_string;

	if ( ( cells_updated_string =
		dictionary_get_pointer(
			non_prefixed_dictionary,
			COLUMNS_UPDATED_KEY ) ) )
	{
		dictionary_set_pointer(	non_prefixed_dictionary,
					COLUMNS_UPDATED_KEY,
					"0" );
		return atoi( cells_updated_string );
	}
	else
	{
		return 0;
	}
}

char *get_cells_updated_folder_name_list_string(
				DICTIONARY *non_prefixed_dictionary )
{
	char *cells_updated_folder_name_list_string;

	cells_updated_folder_name_list_string =
		dictionary_get_pointer(
			non_prefixed_dictionary,
			COLUMNS_UPDATED_CHANGED_FOLDER_KEY );

	dictionary_set_pointer(
		non_prefixed_dictionary,
		COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
		"" );

	return cells_updated_folder_name_list_string;

}

char *get_results_string( DICTIONARY *non_prefixed_dictionary )
{
	return
		dictionary_get_pointer(
			non_prefixed_dictionary,
			RESULTS_STRING_KEY );
}

LIST *subtract_join_1tom_ignore_dictionary_related_folder_list(
			LIST *join_1tom_related_folder_list,
			DICTIONARY *ignore_dictionary )
{
	char key[ 128 ];
	LIST *return_related_folder_list = {0};
	RELATED_FOLDER *related_folder;

	if ( list_rewind( join_1tom_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					join_1tom_related_folder_list );

			sprintf( key,
				 "%s_0",
				 related_folder->
					one2m_folder->
					folder_name );

			if ( !dictionary_key_exists(
				ignore_dictionary,
				key ) )
			{
				if ( !return_related_folder_list )
				{
					return_related_folder_list =
						list_new();
				}

				list_append_pointer(
					return_related_folder_list,
					related_folder );

			}

		} while( list_next( join_1tom_related_folder_list ) );
	}

	return return_related_folder_list;

}

