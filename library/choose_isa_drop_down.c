/* $APPASERVER_HOME/library/choose_isa_drop_down.c	*/
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
#include "appaserver_parameter_file.h"
#include "appaserver_error.h"
#include "environ.h"
#include "element.h"
#include "query.h"
#include "process.h"
#include "choose_isa_drop_down.h"

CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_calloc( void )
{
	CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down;

	if ( ! ( choose_isa_drop_down =
			calloc( 1, sizeof( CHOOSE_ISA_DROP_DOWN ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return choose_isa_drop_down;
}

CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_prompt_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *login_name,
			char *one2m_isa_folder_name,
			char *role_name )
{
	CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down =
		choose_isa_drop_down_calloc();

	/* Input */
	/* ----- */
	choose_isa_drop_down->login_name = login_name;
	choose_isa_drop_down->one2m_isa_folder_name = one2m_isa_folder_name;
	choose_isa_drop_down->role_name = role_name;

	/* Process */
	/* ------- */
	choose_isa_drop_down->folder =
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
			1 /* fetch_row_level_restriction */ );

	choose_isa_drop_down->role =
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	choose_isa_drop_down->security_entity =
		security_entity_new(
			login_name,
			choose_isa_drop_down->folder->non_owner_forbid,
			choose_isa_drop_down->
				role->
				override_row_restrictions );

	if ( choose_isa_drop_down->folder->populate_drop_down_process )
	{
		choose_isa_drop_down->delimited_list =
			process_delimited_list(
				process_choose_isa_commmand_line(
					choose_isa_drop_down->
						folder->
						populate_drop_down_process->
						command_line,
					security_entity_where(
						choose_isa_drop_down->
							security_entity ),
					login_name,
					role_name ) );
	}
	else
	{
		QUERY *query;

		if ( ! ( query =
				query_isa_widget_new(
					one2m_isa_folder_name,
					choose_isa_drop_down->
						folder->
						primary_key_list,
					security_entity_where(
						security_entity ) ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: query_isa_widget_new() returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		choose_isa_drop_down->delimited_list =
			query_delimited_list(
				query->select_clause,
				query->from_clause,
				query->where_clause,
				query->order_clause,
				0 /* max_rows */ );
	}

	return choose_isa_drop_down;
}

char *choose_isa_drop_down_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name )
{
	char action_string[ 1024 ];

	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s+%s+%s+%s\"",
			appaserver_library_http_prompt(
				appaserver_parameter_file_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
		"post_choose_isa_drop_down",
		login_name,
		application_name,
		session_key,
		folder_name,
		role_name );

	return strdup( action_string );
}

char *choose_isa_drop_down_title(
			char *one2m_isa_folder_name )
{
	char title[ 256 ];
	char buffer[ 128 ];

	if ( !one2m_isa_folder_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: one2m_isa_folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title,
		"%s Insert %s",
		application_title_string(),
		format_initial_capital(
			buffer,
			one2m_isa_folder_name ) );

	return strdup( title );
}
