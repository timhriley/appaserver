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
#include "decode_html_post.h"
#include "dictionary2file.h"
#include "attribute.h"
#include "session.h"
#include "role.h"
#include "lookup_before_drop_down.h"
#include "row_security.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY	"edit"
#define DEFAULT_TARGET_FRAME	EDIT_FRAME

/* Prototypes */
/* ---------- */
LIST *subtract_join_1tom_ignore_dictionary_related_folder_list(
			LIST *join_1tom_related_folder_list,
			DICTIONARY *ignore_dictionary );

int get_rows_inserted(	DICTIONARY *non_prefixed_dictionary );

int get_columns_updated(
			DICTIONARY *non_prefixed_dictionary );

char *get_columns_updated_folder_name_list_string(
			DICTIONARY *non_prefixed_dictionary );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	char *state;
	char *state_for_heading;
	char *insert_update_key;
	char *target_frame;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary = {0};
	FORM *form;
	FOLDER *folder;
	int number_rows_outputted = 0;
	LIST *no_display_pressed_attribute_name_list = {0};
	LIST *attribute_name_list;
	LIST *ignore_attribute_name_list;
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
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char content_type_yn = {0};
	int rows_inserted;
	int columns_updated;
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	boolean make_primary_keys_non_edit = 0;
	char *appaserver_user_foreign_login_name;
	/* --------------------------------------------------------------- */
	/* Operation_list_list is only defined to decide on which buttons. */
	/* Row security gets its own operation_list.			   */
	/* --------------------------------------------------------------- */
	LIST *operation_list;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc < 9 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored insert_update_key target_frame [content_type_yn]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* message = argv[ 6 ]; */
	insert_update_key = argv[ 7 ];
	target_frame = argv[ 8 ];

	if ( argc == 10 ) content_type_yn = *argv[ 9 ];

	state = "update";

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role_folder = role_folder_new_role_folder(	application_name,
							session,
							role_name,
							folder_name );

	if ( role_folder_viewonly( role_folder ) ) state = "lookup";

	role = role_new_role(	application_name,
				role_name );

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	folder->mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			BOGUS_SESSION,
			folder->folder_name,
			role->role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	folder->folder_mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			session,
			folder_name,
			role_name,
			1 /* isa_flag */,
			related_folder_recursive_all,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( list_length( folder->folder_mto1_isa_related_folder_list ) )
	{
		make_primary_keys_non_edit = 1;
	}

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			folder->folder_mto1_isa_related_folder_list,
			role_name );

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
			session,
			folder->folder_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			folder->mto1_related_folder_list );

	if ( get_line( dictionary_string, stdin ) )
	{
		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		original_post_dictionary =
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );

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

		dictionary_appaserver_parse_multi_attribute_keys(
			dictionary_appaserver->query_dictionary,
			QUERY_RELATION_OPERATOR_STARTING_LABEL );
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

	folder->one2m_related_folder_list =
		related_folder_get_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder->folder_name,
			role_name,
			update,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /*  parent_primary_attribute_name_list */,
			(LIST *)0 /*  original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	folder->join_1tom_related_folder_list =
		related_folder_get_join_1tom_related_folder_list(
			folder->one2m_related_folder_list );

	folder->join_1tom_related_folder_list =
		subtract_join_1tom_ignore_dictionary_related_folder_list(
			folder->join_1tom_related_folder_list,
			dictionary_appaserver->ignore_dictionary );

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
			folder->lookup_before_drop_down );

	rows_inserted =
		get_rows_inserted(
			dictionary_appaserver->
				non_prefixed_dictionary );

	columns_updated =
		get_columns_updated(
			dictionary_appaserver->
				non_prefixed_dictionary );

	/* If come from detail */
	/* ------------------- */
	if ( instr( "!", insert_update_key, 1 ) > -1 )
	{
		piece( detail_base_folder_name, '!', insert_update_key, 1 );
		insert_update_key[
			instr( "!", insert_update_key, 1 ) ] = '\0';
	}
	else
	{
		*detail_base_folder_name = '\0';
	}

	ignore_attribute_name_list = list_new_list();

	attribute_name_list =
		folder_get_attribute_name_list(
			folder->attribute_list );

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

	form = form_new( folder_name,
			 application_get_title_string(
				application_name ) );

	if ( rows_inserted )
	{
		char subtitle_string[ 32 ];

		sprintf( subtitle_string, "Rows inserted: %d", rows_inserted );
		form->subtitle_string = strdup( subtitle_string );
	}
	else
	if ( columns_updated )
	{
		char *columns_updated_folder_name_list_string;
		char subtitle_string[ 1024 ];

		columns_updated_folder_name_list_string =
			get_columns_updated_folder_name_list_string(
				dictionary_appaserver->
					non_prefixed_dictionary );

		if ( !columns_updated_folder_name_list_string
		&&   !*columns_updated_folder_name_list_string )
		{
			sprintf(subtitle_string,
				"Columns updated: %d",
				columns_updated );
		}
		else
		{
			char buffer[ 1024 ];

			sprintf(subtitle_string,
				"Columns updated: %d in %s",
				columns_updated,
				format_initial_capital(
				 buffer,
				 columns_updated_folder_name_list_string ) );
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

	no_display_pressed_attribute_name_list =
		appaserver_library_get_no_display_pressed_attribute_name_list(
			dictionary_appaserver->
				ignore_dictionary, 
			attribute_name_list );

	if ( !no_display_pressed_attribute_name_list )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get no_display_pressed_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	list_append_unique_string_list(
			ignore_attribute_name_list,
			no_display_pressed_attribute_name_list );

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
	form_set_target_frame( form, target_frame );

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
			application_get_relative_source_directory(
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

	form_set_folder_parameters(	form,
					state_for_heading,
					login_name,
					application_name,
					(char *)0 /* database_string */,
					session,
					folder->folder_name,
					role_name );

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->target_frame,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

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
		form->database_string,
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
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */,
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
			role /* login_role */,
			folder->folder_name /* select_folder_name */,
			login_name,
			state,
			dictionary_appaserver->
				preprompt_dictionary,
			dictionary_appaserver->
				query_dictionary,
			dictionary_appaserver->
				sort_dictionary,
			no_display_pressed_attribute_name_list );

	row_security->select_folder->join_1tom_related_folder_list =
		folder->join_1tom_related_folder_list;

	row_security->row_security_element_list_structure =
		row_security_element_list_structure_new(
			application_name,
			row_security->row_security_state,
			row_security->login_name,
			row_security->state,
			row_security->login_role,
			row_security->preprompt_dictionary,
			row_security->query_dictionary,
			row_security->sort_dictionary,
			row_security->
				no_display_pressed_attribute_name_list,
			row_security->select_folder,
			row_security->attribute_not_null_folder,
			row_security->foreign_login_name_folder,
			(LIST *)0 /* where_clause_attribute_name_list */,
			(LIST *)0 /* where_clause_data_list */,
			non_edit_folder_name_list,
			make_primary_keys_non_edit,
			omit_delete_dont_care,
			0 /* omit_operation_buttons */,
			role_folder->update_yn,
			0 /* not ajax_fill_drop_down_omit */ );

	form->regular_element_list =
		row_security->
			row_security_element_list_structure->
			regular_element_list;

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
			 "ajax_fill_drop_down.sh %s '%s' '%s' %s %s %s '%s'",
			 application_name,
			 login_name,
			 role_name,
			 session,
			 ajax_fill_drop_down_related_folder->
				folder->
				folder_name /* one2m_folder */,
			 ajax_fill_drop_down_related_folder->
				one2m_related_folder->
				folder_name /* mto1_folder */,
			 list_display(
				ajax_fill_drop_down_related_folder->
				folder->
				primary_attribute_name_list ) /* select */ );

		if ( system( sys_string ) ) {};
	}

	if ( list_length(	row_security->
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

	form_output_table_heading(	form->regular_element_list,
					0 /* form_number */ );

	form->row_dictionary_list =
		row_security->
			row_security_element_list_structure->
			row_dictionary_list;

	if ( strcmp(	row_security->select_folder->folder_name,
			"appaserver_user" ) == 0 )
	{
		appaserver_user_foreign_login_name = "login_name";
	}
	else
	{
		appaserver_user_foreign_login_name =
			related_folder_get_appaserver_user_foreign_login_name(
				row_security->
				select_folder->
				mto1_append_isa_related_folder_list );
	}

	number_rows_outputted += form_output_body(
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
		appaserver_user_foreign_login_name );

	if ( number_rows_outputted >= ROWS_FOR_SUBMIT_AT_BOTTOM
	&& ( strcmp( state, "lookup" ) != 0
	||   list_length( operation_list ) ) )
	{
		output_submit_reset_buttons_in_trailer = 1;
	}

	dictionary_appaserver_output_as_hidden(
					dictionary_appaserver,
					0 /* not with_prefixed_dictionary */ );

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		with_dynarch_menu /* with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		/* ------------------------------------------------------ */
		/* Sets row zero for javascript to loop through each row. */
		/* ------------------------------------------------------ */
		folder->post_change_javascript );

	document_close();

	return 0;

} /* main() */

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
} /* get_rows_inserted() */

int get_columns_updated( DICTIONARY *non_prefixed_dictionary )
{
	char *columns_updated_string;

	if ( ( columns_updated_string =
		dictionary_get_pointer(
			non_prefixed_dictionary,
			COLUMNS_UPDATED_KEY ) ) )
	{
		dictionary_set_pointer(	non_prefixed_dictionary,
					COLUMNS_UPDATED_KEY,
					"0" );
		return atoi( columns_updated_string );
	}
	else
	{
		return 0;
	}
} /* get_columns_updated() */

char *get_columns_updated_folder_name_list_string(
				DICTIONARY *non_prefixed_dictionary )
{
	char *columns_updated_folder_name_list_string;

	columns_updated_folder_name_list_string =
		dictionary_get_pointer(
			non_prefixed_dictionary,
			COLUMNS_UPDATED_CHANGED_FOLDER_KEY );

	dictionary_set_pointer(	non_prefixed_dictionary,
				COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
				"" );
	return columns_updated_folder_name_list_string;
} /* get_columns_updated_folder_name_list_string() */

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
					one2m_related_folder->
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

} /* subtract_join_1tom_ignore_dictionary_related_folder_list() */

