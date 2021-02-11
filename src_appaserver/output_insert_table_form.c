/* $APPASERVER_HOME/src_appaserver/output_insert_table_form.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
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
#include "dictionary_appaserver.h"
#include "pair_one2m.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */

#define ONE_ROW_INSERTED_MESSAGE "<h3>One row inserted.</h3>\n"
#define DUPLICATE_MULTIPLE_ROWS_MESSAGE "<h3>Warning: A duplication occurred. If you are generating more bottom rows for data entry, then ignore this message.</h3>\n"
#define DUPLICATE_NON_INSERTED_MESSAGE "<h3>Warning: A duplication occurred.</h3>\n"
#define INSERT_UPDATE_KEY	"edit"
#define DEFAULT_TARGET_FRAME	EDIT_FRAME

/* Prototypes */
/* ---------- */
LIST *get_insert_table_element_list(
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
	char *login_name;
	char *application_name;
	char *session;
	char *parameter_folder_name;
	char *role_name;
	char *state;
	char *insert_update_key;
	char *target_frame;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary = {0};
	FORM *form;
	FOLDER *folder;
	int number_rows_outputted = 0;
	LIST *posted_attribute_name_list = {0};
	LIST *no_display_pressed_attribute_name_list = {0};
	LIST *ignore_attribute_name_list;
	LIST *include_attribute_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int output_submit_reset_buttons_in_trailer = 1;
	int output_submit_reset_buttons_in_heading = 1;
	char detail_base_folder_name[ 128 ];
	char action_string[ 512 ];
	ROLE *role;
	LIST *automatic_preselection_dictionary_list = {0};
	boolean with_dynarch_menu = 0;
	char *vertical_new_button_folder_name;
	char *vertical_new_button_base_folder_name;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char *primary_data_list_string;
	PAIR_ONE2M *pair_one2m;
	char *message = {0};

	/* This needs to be made into a list. */
	/* ---------------------------------- */
	RELATED_FOLDER *ajax_fill_drop_down_related_folder = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 9 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role state insert_update_key target_frame [message]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	parameter_folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];
	insert_update_key = argv[ 7 ];
	target_frame = argv[ 8 ];

	if ( argc == 10 ) message = argv[ 9 ];

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( strcmp( state, "insert" ) != 0 )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: invalid state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	role = role_new_role(	application_name,
				role_name );

	folder = folder_new_folder( 	application_name,
					session,
					parameter_folder_name );

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
			(LIST *)0 /* mto1_related_folder_list */ );

	folder->mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			session,
			folder->folder_name,
			role_name,
			1 /* isa_flag */,
			related_folder_recursive_all,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			folder->mto1_isa_related_folder_list,
			role_name );

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
	}
	else
	{
		dictionary_appaserver =
			dictionary_appaserver_new(
				(DICTIONARY *)0 /* post_dictionary */,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ );
	}

	/* Vertical new button */
	/* ------------------- */ 
	vertical_new_button_folder_name =
		appaserver_library_get_vertical_new_button_folder_name(
			dictionary_appaserver->query_dictionary,
			VERTICAL_NEW_PUSH_BUTTON_PREFIX ); 

	/* Fetch and then reset the non_prefixed_dictionary. */
	/* ------------------------------------------------- */
	vertical_new_button_base_folder_name =
		appaserver_library_get_vertical_new_button_folder_name(
			dictionary_appaserver->non_prefixed_dictionary,
			VERTICAL_NEW_PUSH_BUTTON_BASE_PREFIX );

	primary_data_list_string =
		dictionary_get_string(
			dictionary_appaserver->
				non_prefixed_dictionary,
			PRIMARY_DATA_LIST_KEY );

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_small_new();

	if ( timlib_strlen( primary_data_list_string ) )
	{
		primary_data_list_string_build_dictionaries(
			dictionary_appaserver->
				non_prefixed_dictionary,
			dictionary_appaserver->
				query_dictionary,
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

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		appaserver_library_populate_last_foreign_attribute_key(
			dictionary_appaserver->
				query_dictionary,
			folder->mto1_isa_related_folder_list,
			attribute_get_primary_attribute_name_list(
				folder->attribute_list ) );
	}

	if ( attribute_exists_omit_insert_login_name(
				folder->attribute_list ) )
	{
		dictionary_add_login_name_if_necessary(
			dictionary_appaserver->query_dictionary,
			folder->attribute_name_list,
			login_name );
	}

	folder->attribute_name_list =
		folder_get_attribute_name_list(
			folder->attribute_list );

	form = form_new( folder->folder_name,
			 application_get_title_string( application_name ) );

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
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	form->dont_output_operations = 1;

	if ( dictionary_length( dictionary_appaserver->query_dictionary ) )
	{
		char *reference_number_attribute_name;

		posted_attribute_name_list = 
				dictionary_get_index_zero_key_list(
					dictionary_appaserver->
						query_dictionary, 
					folder->attribute_name_list );

		/* Remove any reference number if insert */
		/* ------------------------------------- */
		if ( ( reference_number_attribute_name =
			attribute_get_reference_number_attribute_name(
				folder->attribute_list,
				posted_attribute_name_list ) ) )
		{
			char key[ 128 ];

			dictionary_delete(
				dictionary_appaserver->
					query_dictionary,
				reference_number_attribute_name );

			sprintf( key,
				 "%s_0",
				 reference_number_attribute_name );

			dictionary_delete(
				dictionary_appaserver->
					query_dictionary,
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
		if ( !vertical_new_button_folder_name )
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
	      	      appaserver_library_get_ignore_pressed_attribute_name_list(
				dictionary_appaserver->
					ignore_dictionary,
				folder->attribute_name_list,
				(DICTIONARY *)0 /* query_dictionary */ ) );

	} /* if query_dictionary */

	folder->pair_one2m_related_folder_list =
		related_folder_get_pair_one2m_related_folder_list(
				folder->application_name,
				folder->folder_name,
				role_name );

	pair_one2m =
		pair_one2m_new(
			ignore_attribute_name_list		/* out only */,
			dictionary_appaserver->
				pair_1tom_dictionary		/* in/out   */,
			application_name			/* in only  */,
			folder->pair_one2m_related_folder_list	/* in only  */,
			posted_attribute_name_list		/* in only  */,
			role					/* in only  */,
			session					/* in only  */);

	/* If pair 1tom and not the vertical new button */
	/* -------------------------------------------- */
	if ( !vertical_new_button_folder_name
	&&    pair_one2m->is_participating )
	{
		if ( pair_one2m->insert_is_completed
		||   pair_one2m->submit_button_on_top_frame )
		{
			document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

			if ( message && *message )
				printf( "%s\n", message );

			printf( "<h1>Insert Complete</h1>\n" );
			fflush( stdout );

			if ( system(
			  "echo '<h2>' && date.sh && echo '</h2>'" )
				) {};

			fflush( stdout );

			if ( pair_one2m->inserted_duplicate )
			{
				fflush( stdout );
				printf( DUPLICATE_NON_INSERTED_MESSAGE );
			}

			document_close();
			exit( 0 );
		}

		folder = pair_one2m->pair_insert_folder;

	} /* if pair one2m */

	list_append_string_list(
		ignore_attribute_name_list,
		appaserver_library_get_omit_insert_attribute_name_list(
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
		appaserver_library_get_no_display_pressed_attribute_name_list(
				dictionary_appaserver->
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
	document_set_javascript_module( document, "validate_date" );

	document_set_folder_javascript_files(
				document,
				application_name,
				folder->folder_name );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	if ( attribute_exists_date_attribute(
		folder->attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
	}

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

		document->onload_control_string =
			document_set_onload_control_string(
				document->onload_control_string,
				form_set_post_change_javascript_row_zero(
					post_change_javascript ) );
	}

	/* If any new button (along the left column) is pressed. */
	/* ----------------------------------------------------- */
	if ( vertical_new_button_base_folder_name )
	{
		char onload_control_string[ 1024 ];
		char sys_string[ 1024 ];
		char *output_filename;
		char *prompt_filename;
		APPASERVER_LINK_FILE *appaserver_link_file;

		/* Make the prompt screen blank. */
		/* ----------------------------- */
		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->
					document_root,
				"blank_screen" /* filename_stem */,
				application_name,
				0 /* process_id */,
				session,
				"html" );

		output_filename =
			appaserver_link_get_output_filename(
				appaserver_link_file->
					output_file->
					document_root_directory,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		prompt_filename =
			appaserver_link_get_link_prompt(
				appaserver_link_file->
					link_prompt->
					prepend_http_boolean,
				appaserver_link_file->
					link_prompt->
					http_prefix,
				appaserver_link_file->
					link_prompt->server_address,
				appaserver_link_file->application_name,
				appaserver_link_file->filename_stem,
				appaserver_link_file->begin_date_string,
				appaserver_link_file->end_date_string,
				appaserver_link_file->process_id,
				appaserver_link_file->session,
				appaserver_link_file->extension );

		if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
		{
			sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '' n n > %s 2>>%s",
				application_name,
				session,
				login_name,
				role->role_name,
				output_filename,
				appaserver_error_get_filename(
					application_name ) );
		}
		else
		{
			sprintf(sys_string,
		 		"output_blank_screen.sh '%s' '' n > %s 2>>%s",
		 		application_get_background_color(
					application_name ),
				output_filename,
		 		appaserver_error_get_filename(
					application_name ) );
		}

		if ( system( sys_string ) ) {};

		sprintf(onload_control_string,
			"window.open('%s','%s');",
			prompt_filename,
			PROMPT_FRAME );

		document->onload_control_string =
			document_set_onload_control_string(
				document->onload_control_string,
				onload_control_string );

		/* Vertical new button */
		/* ------------------- */ 
		appaserver_library_set_vertical_new_button_folder_name(
			dictionary_appaserver->non_prefixed_dictionary,
			vertical_new_button_folder_name,
			VERTICAL_NEW_PUSH_BUTTON_PREFIX );

		appaserver_library_set_vertical_new_button_folder_name(
			dictionary_appaserver->non_prefixed_dictionary,
			vertical_new_button_base_folder_name,
			VERTICAL_NEW_PUSH_BUTTON_BASE_PREFIX );

		dictionary_appaserver->query_dictionary =
			dictionary_small_new();

		output_submit_reset_buttons_in_trailer = 0;
	}

	folder->mto1_related_folder_list =
		appaserver_remove_attribute_name_list_from_related_folder_list(
				folder->mto1_related_folder_list,
				ignore_attribute_name_list );

	list_append_string_list(
		ignore_attribute_name_list,
		related_folder_get_common_non_primary_attribute_name_list(
			application_name,
			folder->folder_name,
			folder->mto1_related_folder_list ) );

	document_output_body(
			document->application_name,
			document->onload_control_string );

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
			dictionary_appaserver->query_dictionary,
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
					state,
					login_name,
					application_name,
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

	if ( vertical_new_button_base_folder_name )
	{
		folder->insert_rows_number = 1;
	}

	form->html_help_file_anchor = folder->html_help_file_anchor;
	form->process_id = getpid();

	fflush( stdout );

	if ( pair_one2m->is_participating )
	{
		if ( pair_one2m->inserted_duplicate )
			printf( DUPLICATE_MULTIPLE_ROWS_MESSAGE );
		else
		if ( !pair_one2m->continuing_series )
			printf( ONE_ROW_INSERTED_MESSAGE );
	}

	include_attribute_name_list =
		list_subtract(	folder->attribute_name_list,
				ignore_attribute_name_list );

	form->regular_element_list =
		get_insert_table_element_list(
			&ajax_fill_drop_down_related_folder,
			login_name,
			application_name,
			session,
			role_name,
			folder->attribute_list,
			include_attribute_name_list,
			folder->mto1_related_folder_list,
			dictionary_appaserver->query_dictionary,
			dictionary_appaserver->preprompt_dictionary,
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
				primary_attribute_name_list ) /* select */ );

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
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	form_output_table_heading(	form->regular_element_list,
					0 /* form_number */ );

	if ( element_exists_reference_number( form->regular_element_list ) )
	{
		form->current_reference_number =
			appaserver_library_get_reference_number(
				application_name,
				folder->insert_rows_number );
	}

	if ( automatic_preselection_dictionary_list )
	{
		number_rows_outputted += form_output_body(
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
			(char *)0 /* attribute_not_null */,
			(char *)0 /* appaserver_user_foreign_login_name */ );
	}
	else
	{
		number_rows_outputted += form_output_insert_rows(
			&form->current_reference_number,
			form->hidden_name_dictionary,
			form->regular_element_list,
			form->current_row,
			folder->insert_rows_number,
			dictionary_appaserver->query_dictionary,
			form->application_name,
			login_name );
	}

	dictionary_appaserver_output_as_hidden(
			dictionary_appaserver,
			1 /* with non_prefixed_dictionary */ );

	if ( number_rows_outputted <= ROWS_FOR_SUBMIT_AT_BOTTOM )
	{
		output_submit_reset_buttons_in_trailer = 0;
	}

	printf( "</table>\n" );
	printf( "<table border=1>\n" );

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
	LIST *primary_attribute_name_list;
	LIST *primary_data_list;
	char *attribute_name;
	char *attribute_data;
	char key[ 128 ];

	if ( !timlib_strlen( primary_data_list_string ) )
		return;

	primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			attribute_list );

	dictionary_set_string(	non_prefixed_dictionary,
				PRIMARY_DATA_LIST_KEY,
				primary_data_list_string );

	primary_data_list =
		list_string2list(	primary_data_list_string,
					FOLDER_DATA_DELIMITER );

	if ( !list_rewind( primary_data_list ) ) return;
	list_rewind( primary_attribute_name_list );

	do {
		attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		attribute_data =
			list_get_pointer(
				primary_data_list );

		strcpy( key, attribute_name );
		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			attribute_data );

		sprintf( key, "%s_0", attribute_name );
		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			attribute_data );

		list_next( primary_attribute_name_list );

	} while( list_next( primary_data_list ) );

}

LIST *get_insert_table_element_list(
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
	ELEMENT_APPASERVER *element;
	LIST *foreign_attribute_name_list = {0};
	int objects_outputted = 0;
	LIST *primary_attribute_name_list;
	LIST *isa_folder_list;
	int max_drop_down_size = INT_MAX;
	boolean is_primary_attribute;
	DICTIONARY *parameter_dictionary;
	DICTIONARY *where_clause_dictionary;

	if ( !list_reset( include_attribute_name_list ) )
		return list_new_list();

/*
Can't do because: benthic SPECIES_MEASUREMENT has BENTHIC_SPECIES
drop-down needing SWEEP.sweep_number in the where clause.
	parameter_dictionary = dictionary_copy( preprompt_dictionary );
*/

	parameter_dictionary = dictionary_copy( query_dictionary );

	dictionary_append_dictionary(
		parameter_dictionary,
		preprompt_dictionary );

	where_clause_dictionary = dictionary_copy( preprompt_dictionary );

	isa_folder_list =
		appaserver_get_isa_folder_list(
			application_name );

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	return_list = list_new_list();
	ignore_attribute_name_list = list_new();

	if ( row_level_non_owner_forbid )
	{
		list_append_pointer(	ignore_attribute_name_list,
					"login_name" );
	}

	/* For each attribute */
	/* ------------------ */
	do {
		attribute_name = 
			list_get_pointer(
				include_attribute_name_list );

		/* If the attribute is accounted for already */
		/* ----------------------------------------- */
		if ( list_exists_string( ignore_attribute_name_list,
					 attribute_name ) )
		{
			continue;
		}

		is_primary_attribute =
			list_exists_string(	
					primary_attribute_name_list,
					attribute_name );

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
			       &foreign_attribute_name_list,
			       ignore_attribute_name_list,
			       (LIST *)0 /* omit_update_attribute_name_list */,
			       mto1_related_folder_list,
			       attribute_name,
			       (LIST *)0 /* include_attribute_name_list */ ) ) )
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
					 foreign_attribute_name_list,
					 row_dictionary_list_length,
					 parameter_dictionary,
					 where_clause_dictionary,
					 0 /* prompt_data_element_only */,
					 folder_post_change_javascript,
					 max_drop_down_size,
					 row_level_non_owner_forbid,
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

		if ( list_exists_string(	ignore_attribute_name_list,
						attribute_name ) )
		{
			continue;
		}

		element_list =
		   	appaserver_library_get_insert_attribute_element_list(
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

