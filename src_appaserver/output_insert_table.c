/* $APPASERVER_HOME/src_appaserver/output_insert_table.c		*/
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
#include "folder.h"
#include "relation.h"
#include "operation_list.h"
#include "document.h"
#include "application.h"
#include "dictionary.h"
#include "query.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "decode_html_post.h"
#include "dictionary2file.h"
#include "attribute.h"
#include "session.h"
#include "role.h"
#include "drillthru.h"
#include "pair_one2m.h"
#include "vertical_new.h"
#include "insert_table.h"

/* Constants */
/* --------- */

#define ONE_ROW_INSERTED_MESSAGE "<h3>One row inserted.</h3>\n"

#define DUPLICATE_MULTIPLE_ROWS_MESSAGE "<h3>Warning: A duplication occurred. If you are generating more bottom rows for data entry, then ignore this message.</h3>\n"

#define DUPLICATE_NON_INSERTED_MESSAGE "<h3>Warning: A duplication occurred.</h3>\n"


/* Prototypes */
/* ---------- */
LIST *output_insert_table_form_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			LIST *attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_related_folder_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			LIST *posted_attribute_name_list,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_process );

void primary_data_list_string_build_dictionaries(
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *query_dictionary,
			char *primary_data_list_string,
			LIST *attribute_list );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *folder_name;
	char *role_name;
	char *target_frame;
	char *message;
	INSERT_TABLE *insert_table;
	DICTIONARY_SEPARATE *dictionary_separate;
	DOCUMENT *document;
	int number_rows_outputted = 0;
	LIST *posted_attribute_name_list = {0};
	LIST *no_display_pressed_attribute_name_list = {0};
	LIST *ignore_attribute_name_list;
	LIST *include_attribute_name_list;
	APPASERVER_PARAMETER *appaserver_parameter;
	int output_submit_reset_buttons_in_trailer = 1;
	int output_submit_reset_buttons_in_heading = 1;
	char detail_base_folder_name[ 128 ];
	char action_string[ 512 ];
	ROLE *role;
	LIST *automatic_preselection_dictionary_list = {0};
	boolean with_dynarch_menu = 0;
	DICTIONARY_SEPARATE *dictionary_separate = {0};
	DICTIONARY *non_prefixed_dictionary = {0};
	DICTIONARY *query_dictionary = {0};
	char *primary_data_list_string;
	PAIR_ONE2M *pair_one2m = {0};
	boolean output_content_type = 1;
	VERTICAL_NEW_OUTPUT_INSERT_TABLE *vertical_new_output_insert_table;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr, 
"Usage: %s login_name session folder role target_frame message dictionary\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session_key = argv[ 2 ];
	folder_name = argv[ 3 ];
	role_name = argv[ 4 ];
	target_frame = argv[ 5 ];
	message = argv[ 6 ];
	dictionary_separate =
		dictionary_separate_string_new(
			argv[ 7 ] );

	session_environment_set( application_name );

	appaserver_parameter = appaserver_parameter_new();

	vertical_new_output_insert_table =
		vertical_new_output_insert_table_new(
			insert_table->
				dictionary_separate->
				non_prefixed_dictionary /* in/out */,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name, /* many_folder_name */,
			appaserver_parameter->document_root_directory,
			VERTICAL_NEW_BUTTON_ONE_PREFIX );

	insert_table =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		insert_table_output_new(
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			target_frame,
			message,
			dictionary_string );

	primary_data_list_string =
		dictionary_get(
			dictionary_separate->non_prefixed_dictionary,
			PRIMARY_DATA_LIST_KEY );

	non_prefixed_dictionary = dictionary_small();

	if ( timlib_strlen( primary_data_list_string ) )
	{
		primary_data_list_string_build_dictionaries(
			non_prefixed_dictionary,
			dictionary_separate->query_dictionary,
			primary_data_list_string,
			folder->attribute_list );
	}

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

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		appaserver_library_populate_last_foreign_attribute_key(
			dictionary_separate->query_dictionary,
			folder->mto1_isa_related_folder_list,
			attribute_primary_key_list(
				folder->attribute_list ) );
	}

	if ( attribute_exists_omit_insert_login_name(
			folder->attribute_list ) )
	{
		dictionary_add_login_name_if_necessary(
			dictionary_separate->query_dictionary,
			folder->attribute_name_list,
			login_name );
	}

	folder->attribute_name_list =
		folder_get_attribute_name_list(
			folder->attribute_list );

	folder->mto1_related_folder_list = 
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session,
			folder->folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_key_list */,
			0 /* recursive_level */ );

	if ( dictionary_length( dictionary_separatge->query_dictionary ) )
	{
		char *reference_number_attribute_name;

		posted_attribute_name_list =
			dictionary_get_index_zero_key_list(
				dictionary_separate->query_dictionary, 
				folder->attribute_name_list );

		/* Remove any reference number if insert */
		/* ------------------------------------- */
		if ( ( reference_number_attribute_name =
			attribute_reference_number_attribute_name(
				folder->attribute_list,
				posted_attribute_name_list ) ) )
		{
			char key[ 128 ];

			dictionary_delete(
				dictionary_separate->query_dictionary,
				reference_number_attribute_name );

			sprintf( key,
				 "%s_0",
				 reference_number_attribute_name );

			dictionary_delete(
				dictionary_separate->query_dictionary,
				key );

			list_remove_string(
				posted_attribute_name_list,
				reference_number_attribute_name );
		}

		/* ---------------------------------------------------- */
		/* The vertical drop-down may have something selected.  */
		/* So only capture the posted attributes if not		*/
		/* from the vertical new button.			*/
		/* ---------------------------------------------------- */
		if ( !vertical_new_button )
		{
			list_append_string_list(	
				ignore_attribute_name_list,
				posted_attribute_name_list );
		}
		else
		{
			posted_attribute_name_list = list_new();
		}

		list_append_string_list(
			ignore_attribute_name_list,
			dictionary_separate->
				ignore_select_attribute_name_list );

	} /* if query_dictionary */

	/* If pair 1tom and not the vertical new button */
	/* -------------------------------------------- */
	if ( !vertical_new_button
	&&   pair_one2m_participating(
		dictionary_separate->
			pair_one2m_dictionary ) )
	{
		pair_one2m =
			pair_one2m_post_new(
				dictionary_separate->
					pair_one2m_dictionary );

		pair_one2m->one_folder_name =
			pair_one2m_folder_name(
				PAIR_ONE2M_ONE_FOLDER_LABEL,
				pair_one2m->pair_one2m_dictionary );

		pair_one2m->many_folder_name =
			pair_one2m_folder_name(
				PAIR_ONE2M_MANY_FOLDER_LABEL,
				pair_one2m->pair_one2m_dictionary );

		pair_one2m->fulfilled_folder_name_list =
			pair_one2m_fulfilled_folder_name_list(
				PAIR_ONE2M_FULFILLED_LIST_LABEL,
				pair_one2m->pair_one2m_dictionary );

		pair_one2m->one2m_pair_relation_list =
			relation_one2m_pair_relation_list(
				relation_one2m_relation_list(
					pair_one2m->one_folder_name ) );

		/* If pressed a related folder button */
		/* ---------------------------------- */
		if ( strcmp(	pair_one2m->one_folder_name,
				pair_one2m->many_folder_name ) != 0 )
		{
			pair_one2m->next_folder_name =
				pair_one2m->many_folder_name;

			pair_one2m_set_fulfilled_name_list(
				pair_one2m->fulfilled_folder_name_list,
				pair_one2m->many_folder_name,
				pair_one2m->
					one2m_pair_relation_list );

			dictionary_separate->pair_one2m_dictionary =
				pair_one2m_fulfilled_dictionary(
					dictionary_separate->
						pair_one2m_dictionary,
					PAIR_ONE2M_FULFILLED_LIST_LABEL,
					pair_one2m->
						fulfilled_folder_name_list );
		}
		else
		/* If doing a series */
		/* ----------------- */
		{
			pair_one2m->next_folder_name =
				pair_one2m_next_folder_name(
					pair_one2m->
						fulfilled_folder_name_list,
					pair_one2m->
						one2m_pair_relation_list );
		}

		if ( !pair_one2m->next_folder_name )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter->
						appaserver_mount_point );

			if ( message && *message )
				printf( "%s\n", message );

			printf( "<h1>Insert Complete</h1>\n" );
			fflush( stdout );

			if ( system(
			  "echo '<h2>' && date.sh && echo '</h2>'" )
				) {};

			fflush( stdout );

			document_close();
			exit( 0 );
		}

		folder =
			folder_fetch(
			   pair_one2m->next_folder_name,
			   1 /* fetch_attribute_list */,
			   0 /* not fetch_one2m_relation_list */,
			   0 /* not fetch_one2m_recursive_relation_list */,
			   1 /* fetch_mto1_isa_recursive_relation_list */,
			   1 /* fetch_mto1_relation_list */ );
	}

	form = form_new( folder->folder_name,
			 application_title_string(
				application_name ) );

	form->dont_output_operations = 1;

	list_append_string_list(
		ignore_attribute_name_list,
		appaserver_library_omit_insert_attribute_name_list(
			folder->attribute_list ) );

	if ( list_rewind( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;

		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			folder->mto1_related_folder_list =
				related_folder_get_mto1_related_folder_list(
				   folder->mto1_related_folder_list,
				   application_name,
				   session,
				   isa_related_folder->folder->folder_name,
				   role_name,
				   0 /* isa_flag */,
				   related_folder_no_recursive,
				   role_get_override_row_restrictions(
				   role->override_row_restrictions_yn ),
				   (LIST *)0
					/* root_primary_att..._name_list */,
				   0 /* recursive_level */ );

			list_append_string_list(
				folder->attribute_name_list,
				folder_get_attribute_name_list(
					isa_related_folder->
					folder->
					attribute_list ) );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	folder->mto1_related_folder_list =
		related_folder_remove_duplicate_mto1_related_folder_list(
			folder->mto1_related_folder_list );

	no_display_pressed_attribute_name_list =
		appaserver_library_no_display_pressed_attribute_name_list(
				dictionary_separate->
					ignore_dictionary, 
				folder->attribute_name_list );

	list_append_unique_string_list(
			ignore_attribute_name_list,
			no_display_pressed_attribute_name_list );

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
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' '%s' 2>>%s",
			application_name,
			session,
			login_name,
			role_name,
			"" /* title */,
			"y" /* content_type_yn */,
			appaserver_error_get_filename(
				application_name ) );
		if ( system( sys_string ) ) {};
		fflush( stdout );
	}
	else
	{
		document->output_content_type = output_content_type;
	}

	form_set_post_process( form, "post_edit_table_form" );
	form_set_current_row( form, 1 );
	form->target_frame = target_frame;

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "null2slash" );
	document_set_javascript_module( document, "push_button_submit" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "validate_date" );

	document_set_folder_javascript_files(
		document,
		application_name,
		folder->folder_name );

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			0 /* not with_close_head */ );

	if ( attribute_exists_date_attribute(
		folder->attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
	}

	printf( "</head>\n" );

	if ( folder->post_change_javascript
	&&   *folder->post_change_javascript )
	{
		char post_change_javascript[ 1024 ];

		strcpy(	post_change_javascript,
			folder->post_change_javascript );

		search_replace_string(
				post_change_javascript,
				"$state",
				"insert" );

		document_body->onload_control_string =
			document_set_onload_control_string(
				document->onload_control_string,
				form_set_post_change_javascript_row_zero(
					post_change_javascript ) );
	}

	/* If any new button (along the left column) is pressed. */
	/* ----------------------------------------------------- */
	if ( vertical_new_button )
	{
		document_body->onload_control_string =
			document_body_set_onload_control_string(
				document->onload_control_string,
				vertical_new_button->onload_control_string );

		query_dictionary = dictionary_small();

		output_submit_reset_buttons_in_trailer = 0;
	}

	list_append_string_list(
		ignore_attribute_name_list,
		related_folder_insert_common_non_primary_key_list(
			folder->folder_name,
			folder->mto1_related_folder_list ) );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	if ( pair_one2m && pair_one2m_inserted_duplicate(
		PAIR_ONE2M_DUPLICATE_KEY,
		pair_one2m->pair_one2m_dictionary ) )
	{
		message = DUPLICATE_MULTIPLE_ROWS_MESSAGE;
	}

	if ( message && *message )
	{
		printf( "%s\n", message );
	}

	if ( ( automatic_preselection_dictionary_list =
		related_folder_list_get_preselection_dictionary_list(
			application_name,
			session,
			login_name,
			folder->folder_name,
			dictionary_separate->query_dictionary,
			folder->mto1_related_folder_list ) ) )
	{
		if ( !list_length(
			automatic_preselection_dictionary_list ) )
		{
			char buffer[ 128 ];

			printf( "<h1>Insert %s complete</h1>\n",
				format_initial_capital(
					buffer,
					folder->folder_name ) );
			document_close();
			exit( 0 );
		}
	}

	form_set_folder_parameters(	form,
					"insert" /* state */,
					login_name,
					application_name,
					session,
					folder->folder_name,
					role_name );

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

	if ( folder->notepad && *folder->notepad )
	{
		printf( "%s\n", folder->notepad );
	}

	form->table_border = 1;
	form->insert_update_key = insert_update_key;
	form->target_frame = target_frame;

	if ( *detail_base_folder_name )
	{
		sprintf(	action_string,
		"%s/post_edit_table_form?%s+%s+%s+%s+%s+%s+detail!%s+%s+%d+",
				appaserver_parameter_cgi_directory(),
				login_name,
				application_name,
				session,
				folder->folder_name,
				role_name,
				form->state,
				detail_base_folder_name,
				target_frame,
				getpid() );

		form->action_string = action_string;
	}

	if ( vertical_new_button->one_folder_name )
	{
		folder->insert_rows_number = 1;
	}

	form->html_help_file_anchor = folder->html_help_file_anchor;
	form->process_id = getpid();

	fflush( stdout );

	include_attribute_name_list =
		list_subtract(	folder->attribute_name_list,
				ignore_attribute_name_list );

	form->regular_element_list =
		output_insert_table_form_element_list(
			&ajax_fill_drop_down_related_folder,
			login_name,
			application_name,
			session,
			role_name,
			folder->attribute_list,
			include_attribute_name_list,
			folder->mto1_related_folder_list,
			dictionary_separate->query_dictionary,
			dictionary_separate->preprompt_dictionary,
			0 /* row_dictionary_list_length */,
			no_display_pressed_attribute_name_list,
			posted_attribute_name_list,
			folder->row_level_non_owner_forbid,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			folder->post_change_javascript,
			INT_MAX /* max_query_rows_for_drop_downs */,
			folder->folder_name
				/* one2m_folder_name_for_processes */ );

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
				primary_key_list ) /* select */ );

		if ( system( sys_string ) ) {};
	}

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
		(char *)0 /* post_change_javascript */ );

	form_output_table_heading(	form->regular_element_list,
					0 /* form_number */ );

	if ( element_exists_reference_number( form->regular_element_list ) )
	{
		form->current_reference_number =
			appaserver_library_reference_number(
				application_name,
				folder->insert_rows_number );
	}

	if ( automatic_preselection_dictionary_list )
	{
		number_rows_outputted +=
			form_output_body(
				&form->current_reference_number,
				form->hidden_name_dictionary,
				form->output_row_zero_only,
				automatic_preselection_dictionary_list,
				form->regular_element_list,
				form->viewonly_element_list,
				(char *)0 /* spool_filename */,
				0 /* row_level_non_owner_view_only */,
				application_name,
				login_name,
				(char *)0
				     /* attribute_not_null_string */,
				(char *)0
				     /* appaserver_user_foreign_login_name */,
				(LIST *)0 /* non_edit_folder_name_list */ );
	}
	else
	{
		number_rows_outputted += form_output_insert_rows(
			&form->current_reference_number,
			form->hidden_name_dictionary,
			form->regular_element_list,
			form->current_row,
			folder->insert_rows_number,
			dictionary_separate->query_dictionary,
			form->application_name,
			login_name );
	}

	if ( number_rows_outputted <= ROWS_FOR_SUBMIT_AT_BOTTOM )
	{
		output_submit_reset_buttons_in_trailer = 0;
	}

	printf( "</table>\n" );

	dictionary_separate_output_as_hidden(
		dictionary_separate,
		1 /* with non_prefixed_dictionary */ );

	printf( "<table border=0>\n" );

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
		(char *)0 /* post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();

	return 0;
}

void primary_data_list_string_build_dictionaries(
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *query_dictionary,
			char *primary_data_list_string,
			LIST *attribute_list )
{
	LIST *primary_key_list;
	LIST *primary_data_list;
	char *attribute_name;
	char *attribute_data;
	char key[ 128 ];

	if ( !timlib_strlen( primary_data_list_string ) )
		return;

	primary_key_list =
		attribute_primary_key_list(
			attribute_list );

	dictionary_set_string(
		non_prefixed_dictionary,
		PRIMARY_DATA_LIST_KEY,
		primary_data_list_string );

	primary_data_list =
		list_string2list(
			primary_data_list_string,
			FOLDER_DATA_DELIMITER );

	if ( !list_rewind( primary_data_list ) ) return;
	list_rewind( primary_key_list );

	do {
		attribute_name =
			list_get_pointer(
				primary_key_list );

		attribute_data =
			list_get_pointer(
				primary_data_list );

		strcpy( key, attribute_name );
		dictionary_set(
			query_dictionary,
			strdup( key ),
			attribute_data );

		sprintf( key, "%s_0", attribute_name );
		dictionary_set(
			query_dictionary,
			strdup( key ),
			attribute_data );

		list_next( primary_key_list );

	} while( list_next( primary_data_list ) );
}

LIST *output_insert_table_form_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *role_name,
			LIST *attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_related_folder_list,
			DICTIONARY *query_dictionary,
			DICTIONARY *preprompt_dictionary,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			LIST *posted_attribute_name_list,
			boolean row_level_non_owner_forbid,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes )
{
	LIST *return_list;
	LIST *element_list;
	LIST *ignore_attribute_name_list;
	char *attribute_name;
	RELATED_FOLDER *related_folder;
	APPASERVER_ELEMENT *element;
	int objects_outputted = 0;
	LIST *primary_key_list;
	LIST *isa_folder_list;
	int max_drop_down_size = INT_MAX;
	boolean is_primary_attribute;
	DICTIONARY *parameter_dictionary;
	DICTIONARY *where_clause_dictionary;
	LIST *foreign_attribute_name_list = {0};

	if ( !list_length( include_attribute_name_list ) )
		return list_new_list();

	parameter_dictionary =
		dictionary_copy(
			query_dictionary );

	dictionary_append_dictionary(
		parameter_dictionary,
		preprompt_dictionary );

	where_clause_dictionary = dictionary_copy( preprompt_dictionary );

	isa_folder_list =
		appaserver_get_isa_folder_list(
			application_name );

	primary_key_list =
		folder_get_primary_key_list(
			attribute_list );

	return_list = list_new_list();

	ignore_attribute_name_list = list_new();

	if ( row_level_non_owner_forbid )
	{
		list_append_pointer(	ignore_attribute_name_list,
					"login_name" );
	}

	list_rewind( include_attribute_name_list );

	/* For each attribute */
	/* ------------------ */
	do {
		attribute_name = 
			list_get(
				include_attribute_name_list );

		/* If the attribute is accounted for already */
		/* ----------------------------------------- */
		if ( list_exists_string(
			attribute_name,
			ignore_attribute_name_list ) )
		{
			continue;
		}

		is_primary_attribute =
			list_exists_string(	
				attribute_name,
				primary_key_list );

		if ( ( related_folder =
		       related_folder_insert_table_consumes_related_folder(
			       &foreign_attribute_name_list,
			       ignore_attribute_name_list,
			       attribute_omit_insert_attribute_name_list(
					attribute_list ),
			       mto1_related_folder_list,
			       attribute_name ) ) )
		{
			ATTRIBUTE *attribute;
			RELATED_FOLDER **only_one_ajax_fill_drop_down;

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
					application_name,
					msg,
					(char *)0 );
				exit( 1 );
			}

			if ( related_folder->ignore_output ) continue;

			if ( isa_folder_list
			&&   list_length( isa_folder_list )
			&&   appaserver_isa_folder_accounted_for(
				isa_folder_list,
				related_folder->folder->folder_name,
				related_folder->
					related_attribute_name ) )
			{
				continue;
			}

/* ---------------------------------------------------- */
/* This broke <Insert> <Reoccurring Transaction> 	*/
/* Test this: <Lookup> <Project Principle Investigator>	*/
/* ---------------------------------------------------- */
/*
			if ( ajax_fill_drop_down_related_folder
			&&   *ajax_fill_drop_down_related_folder )
			{
				only_one_ajax_fill_drop_down =
					(RELATED_FOLDER **)0;
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
				return_list,
				related_folder_insert_element_list(
					 only_one_ajax_fill_drop_down,
					 /* --------------------------- */
					 /* sets related_folder->folder */
					 /* --------------------------- */
					 related_folder,
					 application_name,
					 session,
					 login_name,
					 related_folder->
						foreign_attribute_name_list,
					 row_dictionary_list_length,
					 parameter_dictionary,
					 where_clause_dictionary,
					 0 /* prompt_data_element_only */,
					 folder_post_change_javascript,
					 max_drop_down_size,
					 override_row_restrictions,
					 is_primary_attribute,
					 role_name,
					 max_query_rows_for_drop_downs,
					 0 /* drop_down_multi_select */,
					 related_folder->
							folder->
							no_initial_capital,
					 one2m_folder_name_for_processes
					) );

			related_folder->ignore_output = 1;

			objects_outputted++;
		}

		if ( list_exists_string(
			attribute_name,
			ignore_attribute_name_list ) )
		{
			continue;
		}

		element_list =
		   	appaserver_library_insert_attribute_element_list(
				&objects_outputted,
				attribute_list,
				attribute_name,
				posted_attribute_name_list,
				is_primary_attribute,
				folder_post_change_javascript,
				application_name );

		if ( element_list )
		{
			list_append_list(
				return_list,
				element_list );

			list_append_pointer(
				ignore_attribute_name_list,
				attribute_name );
		}

	} while( list_next( include_attribute_name_list ) );

	if ( no_display_pressed_attribute_name_list 
	&&   list_rewind( no_display_pressed_attribute_name_list ) )
	{
		do {
			attribute_name = list_get_string(
				no_display_pressed_attribute_name_list );

			element =
				element_appaserver_new(
					hidden,
					attribute_name );

			list_append_pointer(
					return_list, 
					element );
		} while( list_next( no_display_pressed_attribute_name_list ) );
	}

	return return_list;

}
