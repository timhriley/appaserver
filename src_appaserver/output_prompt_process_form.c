/* $APPASERVER_HOME/src_appaserver/output_prompt_process_form.c		*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "attribute.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "folder.h"
#include "session.h"
#include "dictionary.h"
#include "document.h"
#include "form.h"
#include "appaserver.h"
#include "frameset.h"
#include "application.h"
#include "process_parameter_list.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "role.h"
#include "query.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define PROMPT_PROCESS_FORM_NAME		"prompt_process"
#define WITH_CHECK_EXECUTABLE_OK 		1
#define COOKIE_KEY				"process"

/* Prototypes */
/* ---------- */
LIST *get_hidden_preprompt_element_list(
				DICTIONARY *preprompt_dictionary );

char *get_preprompt_button_control_string(
				char *application_name,
				char *cgi_directory,
				char *server_address,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name );

void populate_process_parameter_list_element_list(
				boolean *exists_date_element,
				boolean *exits_time_element,
				LIST *element_list,
				char *login_name,
				char *application_name,
				char *process_name,
				boolean override_row_restrictions,
				PROCESS_PARAMETER_LIST *process_parameter_list,
				char *role_name,
				char *post_change_javascript,
				char passed_preprompt_yn,
				char *session,
				char *prompt_display_text,
				boolean prompt_display_bottom,
				char *document_root_directory,
				DICTIONARY *non_prefixed_dictionary,
				DICTIONARY *preprompt_dictionary );

LIST *get_element_list(		char **preprompt_help_text,
				char **post_change_javascript,
				char **html_help_file_anchor,
				char **target_frame,
				boolean *exists_date_element,
				boolean *exists_time_element,
				boolean *process_parameter_exists_preprompt,
				boolean *process_parameter_doing_preprompt,
				char *login_name,
				char *application_name,
				char *process,
				char *role_name,
				boolean override_row_restrictions,
				DICTIONARY *non_prefixed_dictionary,
				DICTIONARY *preprompt_dictionary,
				char passed_preprompt_yn,
				char *session,
				char *document_root_directory );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *process_name;
	char *role_name;
	DOCUMENT *document;
	FORM *form;
	LIST *remember_keystrokes_non_multi_element_name_list;
	LIST *remember_keystrokes_multi_element_name_list;
	char *target_frame;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 2048 ];
	char action_string[ 512 ];
	ROLE *role;
	char *post_change_javascript = {0};
	DICTIONARY *post_dictionary = {0};
	char passed_preprompt_yn;
	char *destination_multi_select_element_name;
	boolean exists_date_element = 0;
	boolean exists_time_element = 0;
	boolean process_parameter_exists_preprompt = 0;
	boolean process_parameter_doing_preprompt = 0;
	char *preprompt_button_control_string = {0};
	boolean with_dynarch_menu = 0;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char *preprompt_help_text = "";

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 7 )
	{
		fprintf( stderr,
"Usage: %s login_name ignored session process role passed_preprompt_yn [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	process_name = argv[ 4 ];
	role_name = argv[ 5 ];
	passed_preprompt_yn = *argv[ 6 ];

	if ( argc == 8 )
	{
		post_dictionary =
			dictionary_index_string2dictionary(
				argv[ 7 ] );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					post_dictionary,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
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
		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					(DICTIONARY *)0,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
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

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );

	if ( !*process_name )
	{
		fprintf( stderr, 
			 "ERROR in %s: no process specified\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	role = role_new_role(	application_name,
				role_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	form = form_new( PROMPT_PROCESS_FORM_NAME,
			 (char *)0 /* application_title */ );

	form_set_folder_parameters(	form,
					(char *)0 /* state */,
					login_name,
					application_name,
					session,
					process_name,
					role_name );

	form->regular_element_list =
		get_element_list(
			&preprompt_help_text,
			&post_change_javascript,
			&form->html_help_file_anchor,
			&target_frame,
			&exists_date_element,
			&exists_time_element,
			&process_parameter_exists_preprompt,
			&process_parameter_doing_preprompt,
			login_name,
			application_name,
			process_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			dictionary_appaserver->non_prefixed_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			passed_preprompt_yn,
			session,
			appaserver_parameter_file->
				document_root );
 
	if ( !list_length( form->regular_element_list ))
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get element list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Set the application title. */
	/* -------------------------- */
	if ( process_parameter_doing_preprompt )
	{
		if ( *preprompt_help_text )
		{
			sprintf(title,
				"<h2>%s</h2>",
			 	process_name );

			list_add_head(
				form->regular_element_list, 
				element_appaserver_new(
					linebreak,
					(char *)0 ) );

			list_add_head(
				form->regular_element_list,
				(char *)element_non_edit_text_new_element(
					"" /* name */,
					preprompt_help_text,
					6 /* column span */,
					ELEMENT_TITLE_NOTEPAD_PADDING_EM ) );
		}
		else
		{
			sprintf(title,
		"<h2>%s: First, filter a subsequent drop-down.</h2>",
			 	process_name );
		}
	}
	else
	{
		strcpy( title, process_name );
	}

	form->application_title = title;

	document = document_new(
			form->application_title,
			application_name );

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

	form->target_frame = target_frame;
	form_set_output_row_zero_only( form );

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "cookie" );
	document_set_javascript_module( document, "form" );
	document_set_javascript_module( document, "form_cookie" );
	document_set_javascript_module( document, "keystrokes" );
	document_set_javascript_module( document, "validate_date" );

	/* Setup remember keystrokes */
	/* ------------------------- */
	remember_keystrokes_non_multi_element_name_list =
		element_list2remember_keystrokes_non_multi_element_name_list(
			form->regular_element_list );

	remember_keystrokes_multi_element_name_list =
		element_list2remember_keystrokes_multi_element_name_list(
			form->regular_element_list );

	form->onload_control_string =
		form_get_remember_keystrokes_onload_control_string(
			form->form_name,
			remember_keystrokes_non_multi_element_name_list,
			remember_keystrokes_multi_element_name_list,
			post_change_javascript,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY );

	form_append_remember_keystrokes_submit_control_string(
			&form->onclick_keystrokes_save_string,
			form,
			remember_keystrokes_non_multi_element_name_list,
			remember_keystrokes_multi_element_name_list,
			(char *)0 /* cookie_key_prefix */,
			COOKIE_KEY );

	document->onload_control_string =
		document_set_onload_control_string(
			document->onload_control_string,
			form_get_onload_control_string( form ) );

	document_set_process_javascript_files(	document,
						application_name,
						process_name );

	if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		with_dynarch_menu = 1;
	}

	if ( with_dynarch_menu )
	{
		char sys_string[ 1024 ];

		document->output_content_type = 1;

		document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
		        1 /* with_dynarch_menu */ );

		document_output_dynarch_html_body(
				DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING,
				document->onload_control_string );

		printf( "<ul id=menu>\n" );

		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' '%c' '%c' 2>>%s",
			application_name,
			session,
			login_name,
			role_name,
			"" /* title */,
			'n' /* content_type_yn */,
			'y' /* omit_html_head_yn */,
			appaserver_error_get_filename(
				application_name ) );

		fflush( stdout );
		if ( system( sys_string ) ){};
		fflush( stdout );
		printf( "</ul>\n" );
	}
	else
	{
		document->output_content_type = 1;

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
	}

	if ( exists_date_element )
	{
		appaserver_library_output_calendar_javascript();
	}

	if ( !appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		document_output_body(
			document->application_name,
			document->onload_control_string );
	}

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->target_frame,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

	sprintf(	action_string,
		 	"%s/post_prompt_process_form?%s+%s+%s+%s+%s+%c",
			appaserver_library_get_http_prompt(
				appaserver_parameter_file_get_cgi_directory(),
				appaserver_library_get_server_address(),
				application_get_ssl_support_yn(
					application_name ),
				application_get_prepend_http_protocol_yn(
					application_name ) ),
			login_name,
			application_name,
			session,
			process_name,
			role_name,
			passed_preprompt_yn );

	form->table_border = 1;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		form->form_name,
		form->post_process,
		action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* post_change_javascript */ );

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
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	if ( process_parameter_exists_preprompt )
	{
		preprompt_button_control_string =
			get_preprompt_button_control_string(
				application_name,
				appaserver_parameter_file_get_cgi_directory(),
				appaserver_library_get_server_address(),
				login_name,
				session,
				process_name,
				role_name );
	}

	printf( "</table>\n" );
	printf( "<table border=1>\n" );

	form_output_trailer_post_change_javascript(
		1 /* output_submit_reset_buttons */,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		preprompt_button_control_string,
		application_name,
		with_dynarch_menu /* with_back_to_top_button */,
		0 /* form_number */,
		post_change_javascript );

	printf( "</table>\n" );
	printf( "</form>\n" );

	document_close();
	return 0;
}

LIST *get_element_list(	char **preprompt_help_text,
			char **post_change_javascript,
			char **html_help_file_anchor,
			char **target_frame,
			boolean *exists_date_element,
			boolean *exists_time_element,
			boolean *process_parameter_exists_preprompt,
			boolean *process_parameter_doing_preprompt,
			char *login_name,
			char *application_name,
			char *process_name,
			char *role_name,
			boolean override_row_restrictions,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *preprompt_dictionary,
			char passed_preprompt_yn,
			char *session,
			char *document_root_directory )
{
	LIST *element_list;
	PROCESS_PARAMETER_LIST *process_parameter_list = {0};
	PROCESS *process;
	PROCESS_SET *process_set;
	char *local_post_change_javascript = {0};
	char *local_prompt_display_text = {0};
	boolean local_prompt_display_bottom = 0;

	element_list = list_new_list();

	process = process_new_process(
				application_name,
				BOGUS_SESSION,
				process_name,
				(DICTIONARY *)0,
				role_name,
				WITH_CHECK_EXECUTABLE_OK );

	if ( process )
	{
		if ( *process->notepad )
		{
			list_append_pointer(
				element_list,
				(char *)element_non_edit_text_new_element(
					"" /* name */,
					process->notepad,
					6 /* column span */,
					ELEMENT_TITLE_NOTEPAD_PADDING_EM ) );
		}

		*html_help_file_anchor = process->html_help_file_anchor;

		local_post_change_javascript =
				process->post_change_javascript;

		*preprompt_help_text = process->preprompt_help_text;
	}
	else
	{
		process_set = process_new_process_set(
					application_name,
					BOGUS_SESSION,
					process_name );

		if ( !process_set )
		{
			document_quick_output_body(
				application_name,
				(char *)0 /* appaserver_mount_point */ );
			printf( "<h3>Error occurred. Check log.</h3>\n" );
			document_close();
			exit( 0 );
		}

		if ( *process_set->notepad )
		{
			list_append_pointer(
				element_list,
				(char *)element_non_edit_text_new_element(
					"" /* name */,
					process_set->notepad,
					6 /* column span */,
					ELEMENT_TITLE_NOTEPAD_PADDING_EM ) );
		}
		*html_help_file_anchor = process_set->html_help_file_anchor;

		local_post_change_javascript =
				process_set->post_change_javascript;

		local_prompt_display_text =
			process_set->prompt_display_text;

		local_prompt_display_bottom =
			process_set->prompt_display_bottom;

		*preprompt_help_text = process_set->preprompt_help_text;
	}

	/* If haven't passed preprompt yet. */
	/* -------------------------------- */
	if ( passed_preprompt_yn != 'y' )
	{
		process_parameter_list = 
			process_parameter_list_new(
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				1 /* with_preprompt */,
				non_prefixed_dictionary
					/* parameter_dictionary */,
				role_name,
				session,
				1 /* with_populate_drop_downs */ );
#ifdef NOT_DEFINED
		process_parameter_list = 
			process_parameter_list_new(
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				1 /* with_preprompt */,
				preprompt_dictionary
					/* parameter_dictionary */,
				role_name,
				session,
				1 /* with_populate_drop_downs */ );
#endif
	}

	if ( process_parameter_list
	&&   list_length( process_parameter_list->process_parameter_list )
	&&   passed_preprompt_yn != 'y' )
	{
		populate_process_parameter_list_element_list(
				exists_date_element,
				exists_time_element,
				element_list,
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				process_parameter_list,
				role_name,
				local_post_change_javascript,
				passed_preprompt_yn,
				session,
				local_prompt_display_text,
				local_prompt_display_bottom,
				document_root_directory,
				non_prefixed_dictionary,
				preprompt_dictionary );

		*target_frame = PROMPT_FRAME;
		*process_parameter_doing_preprompt = 1;
	}
	else
	/* -------------------- */
	/* If passed preprompt. */
	/* -------------------- */
	{
		PROCESS_PARAMETER_LIST *preprompt_process_parameter_list;

		passed_preprompt_yn = 'y';

		process_parameter_list = 
			process_parameter_list_new(
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				0 /* not with_preprompt */,
				non_prefixed_dictionary
					/* parameter_dictionary */,
				role_name,
				session,
				1 /* with_populate_drop_downs */ );

		populate_process_parameter_list_element_list(
				exists_date_element,
				exists_time_element,
				element_list,
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				process_parameter_list,
				role_name,
				local_post_change_javascript,
				passed_preprompt_yn,
				session,
				local_prompt_display_text,
				local_prompt_display_bottom,
				document_root_directory,
				non_prefixed_dictionary,
				preprompt_dictionary );

		*target_frame = EDIT_FRAME;

		preprompt_process_parameter_list = 
			process_parameter_list_new(
				login_name,
				application_name,
				process_name,
				override_row_restrictions,
				1 /* with_preprompt */,
				non_prefixed_dictionary
					/* parameter_dictionary */,
				role_name,
				session,
				1 /* with_populate_drop_downs */ );

		if ( preprompt_process_parameter_list
		&&   list_length( preprompt_process_parameter_list->
					process_parameter_list ) )
		{
			*process_parameter_exists_preprompt = 1;
		}
	}

	*post_change_javascript = local_post_change_javascript;

	return element_list;

}

void populate_process_parameter_list_element_list(
				boolean *exists_date_element,
				boolean *exists_time_element,
				LIST *element_list,
				char *login_name,
				char *application_name,
				char *process_name,
				boolean override_row_restrictions,
				PROCESS_PARAMETER_LIST *process_parameter_list,
				char *role_name,
				char *post_change_javascript,
				char passed_preprompt_yn,
				char *session,
				char *prompt_display_text,
				boolean prompt_display_bottom,
				char *document_root_directory,
				DICTIONARY *non_prefixed_dictionary,
				DICTIONARY *preprompt_dictionary )
{
	LIST *process_parameter_element_list;
	PROCESS_PARAMETER *process_parameter;

	if ( process_parameter_list->is_process_set
	&&   passed_preprompt_yn == 'y'
	&&   !prompt_display_bottom )
	{
		process_parameter_element_list =
			process_parameter_get_process_set_output_element_list(
				process_name,
				application_name,
				role_name,
				post_change_javascript,
				prompt_display_text );

		list_append_list(
			element_list,
			process_parameter_element_list );
	}

	if ( !list_reset( process_parameter_list->process_parameter_list ) )
		return;

	do {
		process_parameter =
			list_get_pointer(
				process_parameter_list->
				  	process_parameter_list );

		if ( !process_parameter->type )
		{
			fprintf(stderr,
		"Warning: %s/%s() got an empty process_parameter\n",
				__FILE__,
				__FUNCTION__ );
			continue;
		}

		/* ------------------- */
		/* If drop_down_prompt */
		/* ------------------- */
		if ( strcmp( 	process_parameter->type,
				"drop_down_prompt" ) == 0  )
		{
			process_parameter_element_list =
			process_parameter_get_drop_down_prompt_element_list(
			process_parameter->parameter_drop_down_prompt,
			post_change_javascript );

			list_append_list( 
				element_list, 
				process_parameter_element_list );
		}
		else
		/* ------------------------ */
		/* If folder with a process */
		/* ------------------------ */
		if ( strcmp( process_parameter->type, "folder" ) == 0
		&&   process_parameter->parameter_process )
		{
			process_parameter_element_list =
			process_parameter_get_folder_process_element_list(
				login_name,
				session,
				application_name,
				role_name,
				process_parameter->
					parameter_folder->
						folder_name,
				process_parameter->parameter_process->
					populate_drop_down_process,
				process_parameter->parameter_process->
					multi_select,
				(char *)0 /* state */ );

			list_append_list(
				element_list,
				process_parameter_element_list );

		}
		else
		/* -------------------------------------- */
		/* If folder without a process nor prompt */
		/* -------------------------------------- */
		if ( strcmp( process_parameter->type, "folder" ) == 0 
		&&  !process_parameter->parameter_process
		&&  !process_parameter->parameter_folder->prompt )
		{
			process_parameter_element_list = 
			process_parameter_get_folder_element_list(
				post_change_javascript,
				process_parameter->
					parameter_folder->
						folder_name,
				process_parameter->
					parameter_folder->
						primary_key_list,
				process_parameter->
					parameter_folder->
						primary_data_list,
				process_parameter->
					parameter_folder->
						multi_select,
				process_parameter->
					populate_helper_process,
				document_root_directory,
				application_name,
				session,
				login_name );

			list_append_list( 
				element_list, 
				process_parameter_element_list );
		}
		else
		/* ------------ */
		/* If attribute */
		/* ------------ */
		if ( strcmp( process_parameter->type, "attribute" ) == 0 )
		{
			process_parameter_element_list =
			process_parameter_get_attribute_element_list(
				exists_date_element,
				exists_time_element,
				application_name,
				process_parameter,
				post_change_javascript );

			list_append_list(
				element_list,
				process_parameter_element_list );
		}
		else
		/* ----------------------- */
		/* If folder with a prompt */
		/* ----------------------- */
		if ( strcmp( process_parameter->type, "folder" ) == 0
		&&  !process_parameter->parameter_process
		&&   process_parameter->parameter_folder->prompt )
		{
			process_parameter_element_list =
			process_parameter_get_folder_prompt_element_list(
			login_name,
			application_name,
			override_row_restrictions,
			post_change_javascript,
			process_parameter->
				parameter_folder->
					prompt,
			process_parameter->
				parameter_folder->
					multi_select,
			process_parameter->
				parameter_folder->
					folder_name,
			process_parameter->
				parameter_folder->
					primary_data_list,
			role_name );

			list_append_list(
				element_list,
				process_parameter_element_list );
		}
		else
		/* --------- */
		/* If prompt */
		/* --------- */
		if ((strcmp( process_parameter->type, "prompt" ) == 0
		&&  !process_parameter->parameter_process )
		|| ( strcmp( process_parameter->type, "folder" ) == 0
		&&   process_parameter->parameter_folder
		&&   process_parameter->parameter_folder->prompt ) )
		{
			process_parameter_element_list =
			process_parameter_get_prompt_element_list(
				process_parameter,
				process_parameter->
				   	parameter_prompt->
					   	hint_message,
				process_parameter->
					parameter_prompt->
						upload_filename_yn,
				process_parameter->
					parameter_prompt->
						date_yn,
				post_change_javascript );

			list_append_list(
				element_list,
				process_parameter_element_list );

			if ( process_parameter->
				parameter_prompt->date_yn == 'y' )
			{
				*exists_date_element = 1;
			}
		}
		else
		/* ------------------------ */
		/* If prompt with a process */
		/* ------------------------ */
		if ( ( strcmp( process_parameter->type, "prompt" ) == 0
		&&     process_parameter->parameter_process )
		||     strcmp( process_parameter->type, "process" ) == 0 )
		{
			process_parameter_element_list =
			process_parameter_get_prompt_process_element_list(
				login_name,
				session,
				application_name,
				role_name,
				process_parameter->parameter_process->prompt,
				process_parameter->parameter_process->
					populate_drop_down_process,
				process_parameter->parameter_process->
					multi_select,
				non_prefixed_dictionary 
					/* parameter_dictionary */ );

			list_append_list(
				element_list,
				process_parameter_element_list );

		}
		else
		{
			fprintf( stderr,
"Warning: %s/%s() got an invalid process_parameter type = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
			 	 process_parameter->type );
		}
	} while( list_next( process_parameter_list->
			    process_parameter_list ) );

	if ( process_parameter_list->is_process_set
	&&   passed_preprompt_yn == 'y'
	&&   prompt_display_bottom )
	{
		process_parameter_element_list =
			process_parameter_get_process_set_output_element_list(
				process_name,
				application_name,
				role_name,
				post_change_javascript,
				prompt_display_text );

		list_append_list(
			element_list,
			process_parameter_element_list );
	}

	if ( dictionary_length( preprompt_dictionary ) )
	{
		list_append_list(
			element_list,
			get_hidden_preprompt_element_list(
				preprompt_dictionary ) );
	}

}

LIST *get_hidden_preprompt_element_list(
					DICTIONARY *preprompt_dictionary )
{
	LIST *key_list;
	char *key;
	char *data;
	char element_name[ 128 ];
	ELEMENT_APPASERVER *element;
	LIST *element_list;
	int str_len;

	key_list = dictionary_get_key_list( preprompt_dictionary );

	if ( !list_rewind( key_list ) ) return (LIST *)0;

	element_list = list_new();

	do {
		key = list_get( key_list );

		if ( timlib_strncmp(
			key,
			QUERY_RELATION_OPERATOR_STARTING_LABEL ) == 0 )
		{
			continue;
		}

		str_len = strlen( key );

		/* ---------------------------------------------------- */
		/* Skip the suffix zero entry. The non-suffix version	*/
		/* also exists.						*/
		/* ---------------------------------------------------- */
		if ( str_len > 2 && strcmp( key + str_len - 2, "_0" ) == 0 )
		{
			continue;
		}

		data = dictionary_fetch( preprompt_dictionary, key );

		sprintf( element_name,
			 "%s%s",
			 PREPROMPT_PREFIX,
			 key );

		element = element_hidden_new_element(
				strdup( element_name ),
				data );

		list_append_pointer( element_list, element );

	} while( list_next( key_list ) );
		
	return element_list;

}

char *get_preprompt_button_control_string(
				char *application_name,
				char *cgi_directory,
				char *server_address,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name )
{
	static char control_string[ 1024 ];

	sprintf(control_string,
		"%s/post_choose_process?%s+%s+%s+%s+%s",
		appaserver_library_get_http_prompt(
			cgi_directory,
		 	server_address,
			application_get_ssl_support_yn(
				application_name ),
		       application_get_prepend_http_protocol_yn(
				application_name ) ),
		 login_name,
		 application_name,
		 session,
		 process_name,
		 role_name );
	return control_string;
}

