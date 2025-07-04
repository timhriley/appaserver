/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/choose_isa.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "query.h"
#include "widget.h"
#include "process.h"
#include "frameset.h"
#include "post_choose_isa.h"
#include "choose_isa.h"

CHOOSE_ISA *choose_isa_calloc( void )
{
	CHOOSE_ISA *choose_isa;

	if ( ! ( choose_isa = calloc( 1, sizeof ( CHOOSE_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return choose_isa;
}

CHOOSE_ISA *choose_isa_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name,
		boolean application_menu_horizontal_boolean )
{
	CHOOSE_ISA *choose_isa;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !insert_folder_name
	||   !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	choose_isa = choose_isa_calloc();

	choose_isa->choose_isa_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		choose_isa_input_new(
			application_name,
			login_name,
			session_key,
			role_name,
			one_folder_name,
			application_menu_horizontal_boolean );

	if ( choose_isa->choose_isa_input->populate_drop_down_process )
	{
		char *command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			choose_isa_command_line(
				choose_isa->
					choose_isa_input->
					populate_drop_down_process->
					command_line
					/* process_command_line */,
				session_key,
				login_name,
				role_name,
				insert_folder_name,
				choose_isa->
					choose_isa_input->
					security_entity->
					where,
				appaserver_error_filename(
					application_name ) );

		choose_isa->delimited_list =
			list_pipe_fetch(
				command_line );
	}
	else
	{
		choose_isa->query_choose_isa =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			query_choose_isa_new(
				application_name,
				login_name,
				one_folder_name,
				choose_isa->
					choose_isa_input->
					one_folder->
					folder_attribute_primary_list,
				choose_isa->
					choose_isa_input->
					security_entity->
					where );

		choose_isa->delimited_list =
			choose_isa->
				query_choose_isa->
					query_row_delimited_list;
	}

	choose_isa->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_isa_title_string( insert_folder_name );

	choose_isa->sub_sub_title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_isa_sub_sub_title_string(
			insert_folder_name,
			one_folder_name );

	choose_isa->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string( application_name ),
			choose_isa->title_string,
			(char *)0 /* sub_title_string */,
			choose_isa->sub_sub_title_string,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */,
			(choose_isa->choose_isa_input->menu)
				? choose_isa->choose_isa_input->menu->html
				: (char *)0,
			document_head_menu_setup_string(
				(choose_isa->choose_isa_input->menu)
					? 1
					: 0 /* menu_boolean */ ),
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	choose_isa->prompt_message =
		choose_isa_prompt_message(
			WIDGET_DROP_DOWN_DASH_DELIMITER,
			choose_isa->
				choose_isa_input->
				one_folder->
				folder_attribute_primary_key_list );

	choose_isa->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_isa_action_string(
			POST_CHOOSE_ISA_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			insert_folder_name,
			one_folder_name );

	choose_isa->form_choose_isa =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_choose_isa_new(
			WIDGET_LOOKUP_CHECKBOX_NAME,
			choose_isa->prompt_message,
			choose_isa->
				choose_isa_input->
				one_folder->
				folder_attribute_primary_key_list,
			choose_isa->delimited_list,
			choose_isa->
				choose_isa_input->
				one_folder->
				no_initial_capital,
			choose_isa->action_string,
			FRAMESET_PROMPT_FRAME /* target_frame */ );

	choose_isa->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			choose_isa->document->html,
			choose_isa->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			choose_isa->document->document_body->html,
			choose_isa->form_choose_isa->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			document_close_tag() );

	free( choose_isa->form_choose_isa->html );

	return choose_isa;
}

char *choose_isa_action_string(
		const char *post_choose_isa_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name )
{
	char action_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !insert_folder_name
	||   !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(action_string,
		"%s/%s?%s+%s+%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			application_ssl_support_boolean(
				application_name ) ),
		post_choose_isa_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		insert_folder_name,
		one_folder_name );

	return strdup( action_string );
}

char *choose_isa_title_string( char *insert_folder_name )
{
	static char title_string[ 128 ];
	char buffer[ 64 ];

	if ( !insert_folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: insert_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_string,
		"Insert %s",
		string_initial_capital(
			buffer,
			insert_folder_name ) );

	return title_string;
}

char *choose_isa_sub_sub_title_string(
		char *insert_folder_name,
		char *one_folder_name )
{
	static char sub_sub_title_string[ 128 ];
	char buffer1[ 64 ];
	char buffer2[ 64 ];

	if ( !insert_folder_name
	||   !one_folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(sub_sub_title_string,
		"Is this %s an existing %s? If no, keep as Select.",
		string_initial_capital(
			buffer1,
			insert_folder_name ),
		string_initial_capital(
			buffer2,
			one_folder_name ) );

	return sub_sub_title_string;
}

char *choose_isa_prompt_message(
		char *widget_drop_down_dash_delimiter,
		LIST *primary_key_list )
{
	char prompt_message[ 512 ];
	char buffer[ 256 ];

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(prompt_message,
		"Choose %s",
		string_initial_capital(
			buffer,
			list_display_string_delimited(
				primary_key_list,
				widget_drop_down_dash_delimiter ) ) );

	return strdup( prompt_message );
}

char *choose_isa_command_line(
		char *process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *security_entity_where,
		char *appaserver_error_filename )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	query_drop_down_process_command_line(
		(char *)0 /* application_name */,
		session_key,
		login_name,
		role_name,
		(char *)0 /* state */,
		insert_folder_name /* many_folder_name */,
		(char *)0 /* populate_drop_down_process_name */,
		(DICTIONARY *)0 /* dictionary */,
		security_entity_where /* where_string */,
		process_command_line,
		appaserver_error_filename );
}

CHOOSE_ISA_INPUT *choose_isa_input_new(
		char *application_name,
		char *login_name,
		char *session_key,
		char *role_name,
		char *one_folder_name,
		boolean application_menu_horizontal_boolean )
{
	CHOOSE_ISA_INPUT *choose_isa_input;

	if ( !application_name
	||   !login_name
	||   !session_key
	||   !role_name
	||   !one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	choose_isa_input = choose_isa_input_calloc();

	choose_isa_input->one_folder =
		folder_fetch(
			one_folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			0 /* not cache_boolean */ );

	if ( choose_isa_input->
		one_folder->
		populate_drop_down_process_name )
	{
		choose_isa_input->populate_drop_down_process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_fetch(
				choose_isa_input->
					one_folder->
					populate_drop_down_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source_directory */,
				1 /* check_executable_inside_filesystem */,
				appaserver_parameter_mount_point() );
	}

	choose_isa_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	choose_isa_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			one_folder_name );

	choose_isa_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			choose_isa_input->
				folder_row_level_restriction->
				non_owner_forbid,
			choose_isa_input->
				role->
				override_row_restrictions );

	if ( application_menu_horizontal_boolean )
	{
		choose_isa_input->folder_menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			folder_menu_fetch(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory(),
				0 /* not folder_menu_remove_boolean */ );

		choose_isa_input->menu =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				1 /* application_menu_horizontal_boolean */,
				choose_isa_input->folder_menu->count_list );
	}

	return choose_isa_input;
}

CHOOSE_ISA_INPUT *choose_isa_input_calloc( void )
{
	CHOOSE_ISA_INPUT *choose_isa_input;

	if ( ! ( choose_isa_input = calloc( 1, sizeof ( CHOOSE_ISA_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return choose_isa_input;
}

