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
#include "choose_isa.h"

CHOOSE_ISA *choose_isa_calloc( void )
{
	CHOOSE_ISA *choose_isa;

	if ( ! ( choose_isa =
			calloc( 1, sizeof( CHOOSE_ISA ) ) ) )
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

CHOOSE_ISA *choose_isa_prompt_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name )
{
	CHOOSE_ISA *choose_isa = choose_isa_calloc();

	/* Input */
	/* ----- */
	choose_isa->application_name = application_name;
	choose_isa->session_key = session_key;
	choose_isa->login_name = login_name;
	choose_isa->folder_name = folder_name;
	choose_isa->one2m_isa_folder_name = one2m_isa_folder_name;
	choose_isa->role_name = role_name;

	/* Process */
	/* ------- */
	choose_isa->folder =
		folder_fetch(
			one2m_isa_folder_name,
			(char *)0 /* not fetching role_folder_list */,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* --------------------------------------- */
			/* Also sets folder_primary_attribute_list */
			/* and folder_key_list			   */
			/* --------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			0 /* not fetch_row_level_restriction */,
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
							security_entity ),
					login_name,
					role_name,
					one2m_isa_folder_name ) );
	}
	else
	{
		QUERY *query;

		if ( ! ( query =
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
							security_entity ) ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: query_isa_widget_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		choose_isa->delimited_list =
			query_delimited_list(
				query->select_clause,
				query->from_clause,
				query->where_clause,
				query->order_clause,
				0 /* max_rows */ );
	}

	choose_isa->folder_menu =
		folder_menu_fetch(
			application_name,
			session_key,
			appaserver_parameter_data_directory(),
			role_name );

	if ( !choose_isa->folder_menu )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	choose_isa->action_string =
		choose_isa_action_string(
			application_name,
			login_name,
			session_key,
			folder_name,
			one2m_isa_folder_name,
			role_name );

	if ( !choose_isa->action_string )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: choose_isa_action_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	choose_isa->document =
		document_choose_isa_new(
			choose_isa->title,
			choose_isa->prompt_message,
			one2m_isa_folder_name,
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				PROMPT_FRAME /* target_frame */,
				choose_isa->folder_menu->lookup_count_list ),
			choose_isa->folder->primary_key_list,
			choose_isa->delimited_list,
			choose_isa->folder->no_initial_capital,
			choose_isa->action_string );

	return choose_isa;
}

char *choose_isa_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name )
{
	char action_string[ 1024 ];

	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s+%s+%s+%s\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		"post_choose_isa_drop_down",
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

