/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/lookup_delete.c	   		   	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "folder_operation.h"
#include "delete.h"
#include "application.h"
#include "folder_menu.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "form_lookup_delete.h"
#include "relation_mto1.h"
#include "lookup_delete.h"

LOOKUP_DELETE *lookup_delete_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_DELETE *lookup_delete;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete = lookup_delete_calloc();

	lookup_delete->lookup_delete_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_delete_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	if ( !lookup_delete->
		lookup_delete_input->
		folder_operation_delete_boolean )
	{
		char message[ 128 ];

		sprintf(message,
		"folder_operation_delete_boolean is NOT set for %s/%s/%s.",
			login_name,
			role_name,
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete->query_primary_key =
		/* ----------------------- */
		/* Note: frees each where. */
		/* ----------------------- */
		query_primary_key_fetch(
			application_name,
			login_name,
			folder_name,
			lookup_delete->
				lookup_delete_input->
				security_entity->
				where,
			lookup_delete->
				lookup_delete_input->
				relation_mto1_list,
			lookup_delete->
				lookup_delete_input->
				relation_mto1_isa_list,
			lookup_delete->
				lookup_delete_input->
				folder_attribute_append_isa_list,
			lookup_delete->
				lookup_delete_input->
				query_dictionary,
			(char *)0 /* additional_attribute_name */ );

	if ( !lookup_delete->query_primary_key
	||   !lookup_delete->query_primary_key->query_fetch )
	{
		char message[ 128 ];

		sprintf(message,
			"query_primary_key_fetch(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_delete_title_string(
			folder_name );

	lookup_delete->sub_title_string =
		/* -------------------------------------- */
		/* Returns where_string or program memory */
		/* -------------------------------------- */
		lookup_delete_sub_title_string(
			lookup_delete->
				query_primary_key->
				query_table_edit_where->
				string );

	lookup_delete->fetch_row_count =
		lookup_delete_fetch_row_count( 
			list_length(
				lookup_delete->
					query_primary_key->
					query_fetch->
					row_list )
			/* query_fetch_row_list_length */ );

	return lookup_delete;
}

LOOKUP_DELETE *lookup_delete_calloc( void )
{
	LOOKUP_DELETE *lookup_delete;

	if ( ! ( lookup_delete = calloc( 1, sizeof ( LOOKUP_DELETE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_delete;
}

void lookup_delete_state_two_execute(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean execute_boolean,
		LIST *query_fetch_row_list,
		char *title_string )
{
	QUERY_ROW *query_row;
	DELETE *delete;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		title_string );

	if ( list_rewind( query_fetch_row_list ) )
	{
		/* Keep indent; see below. */
		/* ----------------------- */
		do {
			query_row = list_get( query_fetch_row_list );

			delete =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				delete_new(
					application_name,
					login_name,
					role_name,
					folder_name,
					query_row->cell_list,
					1 /* isa_boolean */,
					1 /* update_null_boolean */ );

			if ( !delete->
				delete_input->
				folder_operation_delete_boolean )
			{
				char message[ 128 ];

				sprintf(message,
		"folder_operation_delete_boolean is NOT set for: %s/%s/%s.",
					login_name,
					role_name,
					folder_name );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
				message );
			}

			if ( execute_boolean )
			{
				delete_execute( delete->delete_sql_list );

				printf( "<h3>%s</h3>\n",
					delete->complete_message_string );
			}
			else
			{
				delete_display( delete->delete_sql_list );

				printf( "<h3>%s</h3>\n",
					delete->message_string );
			}

		} while ( list_next( query_fetch_row_list ) );

	}
	else
	{
		printf( "<h3>Nothing selected to delete.</h3>\n" );
	}

	document_close();
}

char *lookup_delete_action_string(
		const char *lookup_delete_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	char action_string[ 1024 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
			application_ssl_support_boolean(
				application_name ) ),
		lookup_delete_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		"two" /* state */ );

	return strdup( action_string );
}

void lookup_delete_state_one_form_output(
		const char *lookup_delete_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_DELETE *lookup_delete;
	char *action_string;
	FORM_LOOKUP_DELETE *form_lookup_delete;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_delete_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			original_post_dictionary );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		lookup_delete->title_string /* process_name */ );

	printf( "<h2>Where: %s</h2>\n",
		lookup_delete->sub_title_string );

	if ( !lookup_delete->fetch_row_count )
	{
		printf(	"<h3>%s</h3>\n",
			LOOKUP_DELETE_NO_ROWS_MESSAGE );

		document_close();
		exit( 0 );
	}

	if (	lookup_delete->fetch_row_count >
		LOOKUP_DELETE_ROW_MAX )
	{
		printf(	LOOKUP_DELETE_MAX_EXCEED_TEMPLATE,
			lookup_delete->fetch_row_count,
			LOOKUP_DELETE_ROW_MAX );
		document_close();
		exit( 0 );
	}

	printf( "%s\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_delete_prompt_html(
			folder_name,
			lookup_delete->fetch_row_count ) );

	action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_delete_action_string(
			lookup_delete_executable,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	form_lookup_delete =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_lookup_delete_new(
			LOOKUP_DELETE_EXECUTE_LABEL,
			lookup_delete->
				lookup_delete_input->
				query_dictionary,
			action_string );

	printf(	"%s\n",
		form_lookup_delete->form_html );

	document_close();
}

char *lookup_delete_prompt_html(
		char *folder_name,
		int fetch_row_count )
{
	char buffer[ 128 ];
	static char html[ 256 ];
	char *row_string;

	if ( !folder_name
	||   !fetch_row_count )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( fetch_row_count == 1 )
		row_string = "row";
	else
		row_string = "rows";

	sprintf(html,
		"<h3>Delete %d %s from %s</h3>",
		fetch_row_count,
		row_string,
		string_initial_capital(
			buffer,
			folder_name ) );

	return html;
}

LOOKUP_DELETE_INPUT *lookup_delete_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary )
{
	LOOKUP_DELETE_INPUT *lookup_delete_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_delete_input = lookup_delete_input_calloc();

	lookup_delete_input->folder_operation_list =
		folder_operation_list(
			folder_name,
			role_name,
			0 /* not fetch_operation */,
			0 /* not fetch_process */ );

	lookup_delete_input->folder_operation_delete_boolean =
		folder_operation_delete_boolean(
			lookup_delete_input->folder_operation_list );

	if ( !lookup_delete_input->folder_operation_delete_boolean )
		return lookup_delete_input;

	lookup_delete_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	lookup_delete_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			0 /* not fetch_role_attribute_exclude_list */ );

	lookup_delete_input->relation_mto1_list =
		relation_mto1_list(
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			lookup_delete_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */ );

	lookup_delete_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list */,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			folder_name
				/* many_folder_name */,
			lookup_delete_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	lookup_delete_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			lookup_delete_input->
				folder->
				folder_attribute_list
					/* append_isa_list */,
			lookup_delete_input->relation_mto1_isa_list );

	lookup_delete_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			lookup_delete_input->folder_attribute_append_isa_list );

	lookup_delete_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	lookup_delete_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			lookup_delete_input->
				folder_row_level_restriction->
				non_owner_forbid ||
			lookup_delete_input->
				folder_row_level_restriction->
				non_owner_viewonly,
			lookup_delete_input->role->override_row_restrictions );

	lookup_delete_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_lookup_delete_new(
			original_post_dictionary,
			application_name,
			login_name,
			lookup_delete_input->folder_attribute_date_name_list,
			lookup_delete_input->folder_attribute_append_isa_list );

	lookup_delete_input->query_dictionary =
		lookup_delete_input_query_dictionary(
			lookup_delete_input->
				dictionary_separate->
				query_dictionary,
			lookup_delete_input->
				dictionary_separate->
				non_prefixed_dictionary );

	lookup_delete_input->execute_boolean =
		lookup_delete_input_execute_boolean(
			LOOKUP_DELETE_EXECUTE_LABEL,
			lookup_delete_input->
				dictionary_separate->
				non_prefixed_dictionary );

	return lookup_delete_input;
}

LOOKUP_DELETE_INPUT *lookup_delete_input_calloc( void )
{
	LOOKUP_DELETE_INPUT *lookup_delete_input;

	if ( ! ( lookup_delete_input =
			calloc( 1,
				sizeof ( LOOKUP_DELETE_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_delete_input;
}

char *lookup_delete_title_string( char *folder_name )
{
	static char title_string[ 128 ];

	if ( !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "folder_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(title_string,
		"delete from %s",
		folder_name );

	return
	string_initial_capital(
		title_string,
		title_string );
}

char *lookup_delete_sub_title_string( char *where_string )
{
	if ( !where_string ) where_string = "Entire Table";

	return where_string;
}

boolean lookup_delete_input_execute_boolean(
		char *lookup_delete_execute_label,
		DICTIONARY *non_prefixed_dictionary )
{
	char *get;

	get =
		dictionary_get(
			lookup_delete_execute_label,
			non_prefixed_dictionary );

	return ( get && *get == 'y' );
}

DICTIONARY *lookup_delete_input_query_dictionary(
		DICTIONARY *query_dictionary,
		DICTIONARY *non_prefixed_dictionary )
{
	if ( dictionary_length( query_dictionary ) )
		return query_dictionary;
	else
		return non_prefixed_dictionary;
}

int lookup_delete_fetch_row_count( int query_fetch_row_list_length )
{
	return query_fetch_row_list_length;
}

