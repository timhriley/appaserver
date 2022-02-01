/* $APPASERVER_HOME/library/choose_isa.c		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "element.h"
#include "query.h"
#include "process.h"
#include "frameset.h"
#include "choose_isa.h"

CHOOSE_ISA *choose_isa_calloc( void )
{
	CHOOSE_ISA *choose_isa;

	if ( ! ( choose_isa = calloc( 1, sizeof( CHOOSE_ISA ) ) ) )
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
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name,
			boolean menu_boolean )
{
	CHOOSE_ISA *choose_isa = choose_isa_calloc();

	/* Process */
	/* ------- */
	choose_isa->folder =
		folder_fetch(
			one2m_isa_folder_name,
			(char *)0 /* not fetching role_folder_list */,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* --------------------------------------- */
			/* Also sets folder_attribute_primary_list */
			/* and primary_key_list			   */
			/* --------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	choose_isa->role =
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	choose_isa->security_entity =
		security_entity_new(
			login_name,
			choose_isa->folder->non_owner_forbid,
			choose_isa->
				role->
				override_row_restrictions );

	if ( choose_isa->folder->populate_drop_down_process )
	{
		choose_isa->delimited_list =
			list_pipe_fetch(
				process_choose_isa_command_line(
					choose_isa->
						folder->
						populate_drop_down_process->
						command_line,
					application_name,
					security_entity_where(
						choose_isa->
							security_entity,
						choose_isa->
							folder->
							folder_attribute_list ),
					login_name,
					role_name,
					one2m_isa_folder_name ) );
	}
	else
	{
		choose_isa->query_isa_widget =
			query_isa_widget_new(
				one2m_isa_folder_name,
				choose_isa->
					folder->
					primary_key_list,
				choose_isa->
					folder->
					folder_attribute_primary_list,
				login_name,
				security_entity_where(
					choose_isa->
						security_entity,
					choose_isa->
						folder->
						folder_attribute_list ) );

		if ( !choose_isa->query_isa_widget )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: query_isa_widget_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		choose_isa->delimited_list =
			choose_isa->
				query_isa_widget->
					delimited_list;
	}

	choose_isa->prompt_message =
		choose_isa_prompt_message(
			choose_isa->folder->primary_key_list );

	if ( menu_boolean )
	{
		choose_isa->folder_menu =
			folder_menu_fetch(
				application_name,
				session_key,
				appaserver_parameter_data_directory(),
				role_name );

		if ( !choose_isa->folder_menu )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				role_name );
			exit( 1 );
		}

		choose_isa->menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				FRAMESET_PROMPT_FRAME,
				choose_isa->folder_menu->lookup_count_list );

		if ( !choose_isa->menu )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				login_name );
			exit( 1 );
		}

	}

	choose_isa->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_isa_post_action_string(
			application_name,
			login_name,
			session_key,
			folder_name,
			one2m_isa_folder_name,
			role_name );

	if ( !choose_isa->post_action_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: choose_isa_post_action_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	choose_isa->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			choose_isa->title_string,
			choose_isa->subtitle_html,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			menu_boolean,
			choose_isa->menu,
			document_head_menu_setup_string( menu_boolean ),
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );


	choose_isa->form_choose_isa =
		form_choose_isa_new(
			choose_isa->prompt_message,
			choose_isa->folder->primary_key_list,
			choose_isa->delimited_list,
			choose_isa->folder->no_initial_capital,
			choose_isa->post_action_string );

	choose_isa->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_isa_html(
			choose_isa->document->html,
			choose_isa->document->document_head->html,
			document_head_close_html(),
			choose_isa->document->document_body->html,
			choose_isa->form_choose_isa->html,
			document_body_close_html(),
			document_close_html() );

	return choose_isa;
}

char *choose_isa_post_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name )
{
	char action_string[ 1024 ];

	if ( !application_name
	||   !login_name
	||   !session_key
	||   !folder_name
	||   !one2m_isa_folder_name
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		"post_choose_isa",
		application_name,
		login_name,
		session_key,
		folder_name,
		one2m_isa_folder_name,
		role_name );

	return strdup( action_string );
}

char *choose_isa_title_string(
			char *folder_name )
{
	char title_string[ 256 ];
	char buffer[ 128 ];

	if ( !folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title_string,
		"Insert %s",
		string_initial_capital(
			buffer,
			folder_name ) );

	return strdup( title_string );
}

char *choose_isa_subtitle_html(
			char *folder_name,
			char *one2m_isa_folder_name )
{
	char subtitle_html[ 256 ];
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !folder_name
	||   !one2m_isa_folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(subtitle_html,
		"<h3> Is this %s an existing %s?</h3>",
		string_initial_capital(
			buffer1,
			folder_name ),
		string_initial_capital(
			buffer2,
			one2m_isa_folder_name ) );

	return strdup( subtitle_html );
}

char *choose_isa_prompt_message( LIST *primary_key_list )
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
				ELEMENT_LONG_DASH_DELIMITER ) ) );

	return strdup( prompt_message );
}

char *choose_isa_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *form_choose_isa_html,
			char *document_body_close_html,
			char *document_close_html )
{
	char html[ STRING_ONE_MEG ];

	if ( !document_html
	||   !document_head_html
	||   !document_head_close_html
	||   !document_body_html
	||   !form_choose_isa_html
	||   !document_body_close_html
	||   !document_close_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"%s\n%s\n%s\n%s\n%s\n%s\n%s",
		document_html,
		document_head_html,
		document_head_close_html,
		document_body_html,
		form_choose_isa_html,
		document_body_close_html,
		document_close_html );

	return strdup( html );
}

