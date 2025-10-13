/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_spreadsheet.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver.h"
#include "application.h"
#include "document.h"
#include "relation_mto1.h"
#include "folder_attribute.h"
#include "appaserver_error.h"
#include "lookup_spreadsheet.h"

LOOKUP_SPREADSHEET *lookup_spreadsheet_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_SPREADSHEET *lookup_spreadsheet;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	lookup_spreadsheet = lookup_spreadsheet_calloc();

	lookup_spreadsheet->lookup_spreadsheet_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_spreadsheet_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_string,
			data_directory );

	lookup_spreadsheet->query_spreadsheet =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_spreadsheet_new(
			application_name,
			login_name,
			folder_name,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				security_entity->
				where,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				dictionary_separate->
				non_prefixed_dictionary
					/* query_dictionary */,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				dictionary_separate->
				no_display_name_list,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				role_attribute_exclude_lookup_name_list,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				folder_attribute_append_isa_list,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				folder->
				folder_attribute_primary_key_list,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				relation_mto1_list,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				relation_mto1_isa_list  );

	lookup_spreadsheet->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_spreadsheet_title_string(
			folder_name );

	lookup_spreadsheet->sub_title_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		lookup_spreadsheet_sub_title_string(
			lookup_spreadsheet->
				query_spreadsheet->
				query_table_edit_where->
				string );

	return lookup_spreadsheet;
}

LOOKUP_SPREADSHEET *lookup_spreadsheet_calloc( void )
{
	LOOKUP_SPREADSHEET *lookup_spreadsheet;

	if ( ! ( lookup_spreadsheet =
			calloc( 1,
				sizeof ( LOOKUP_SPREADSHEET ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_spreadsheet;
}

char *lookup_spreadsheet_title_string( char *folder_name )
{
	static char title_string[ 128 ];
	char destination[ 64 ];

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
		"Spreadsheet for %s",
		string_initial_capital(
			destination,
			folder_name ) );

	return title_string;
}

char *lookup_spreadsheet_sub_title_string(
		char *query_table_edit_where_string )
{
	char title_string[ STRING_WHERE_BUFFER ];

	if ( !query_table_edit_where_string )
	{
		query_table_edit_where_string = "Entire table";
	}

	if (	strlen( query_table_edit_where_string ) + 7 >=
		STRING_WHERE_BUFFER )
	{
		*title_string = '\0';
	}
	else
	{
		sprintf(title_string,
			"Where: %s",
			query_table_edit_where_string );
	}

	return strdup( title_string );
}

void lookup_spreadsheet_output(
		char *application_name,
		LOOKUP_SPREADSHEET *lookup_spreadsheet )
{
	unsigned long row_count;
	char *prompt_message;
	char *anchor_html;

	if ( !application_name
	||   !lookup_spreadsheet )
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
		lookup_spreadsheet->title_string );

	printf( "<h2>%s</h2>\n",
		lookup_spreadsheet->sub_title_string );

	row_count =
		query_spreadsheet_output(
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				folder_attribute_append_isa_list,
			lookup_spreadsheet->
				query_spreadsheet->
				destination_enum,
			lookup_spreadsheet->
				query_spreadsheet->
				query_select_name_list,
			lookup_spreadsheet->
				query_spreadsheet->
				query_system_string,
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				output_filename );

	prompt_message =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		lookup_spreadsheet_prompt_message(
			row_count );

	anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			lookup_spreadsheet->
				lookup_spreadsheet_input->
				prompt_filename,
			"_new" /* target_window */,
			prompt_message );

	printf( "%s\n%s\n%s\n",
		anchor_html,
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_body_close_tag(),
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		document_close_tag() );
}

char *lookup_spreadsheet_prompt_message( unsigned long row_count )
{
	static char prompt_message[ 128 ];

	sprintf(prompt_message,
		"Link to file with %ld rows.",
		row_count );

	return prompt_message;
}

LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory )
{
	LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	lookup_spreadsheet_input = lookup_spreadsheet_input_calloc();

	lookup_spreadsheet_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_role_attribute_exclude_list */ );

	lookup_spreadsheet_input->role_attribute_exclude_lookup_name_list =
		role_attribute_exclude_lookup_name_list(
			APPASERVER_LOOKUP_STATE,
			APPASERVER_UPDATE_STATE,
			lookup_spreadsheet_input->
				role->
				role_attribute_exclude_list );

	lookup_spreadsheet_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			lookup_spreadsheet_input->
				role_attribute_exclude_lookup_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	lookup_spreadsheet_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			lookup_spreadsheet_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	lookup_spreadsheet_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			lookup_spreadsheet_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_mto1_list */ );

	lookup_spreadsheet_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			lookup_spreadsheet_input->
				folder->
				folder_attribute_list,
			lookup_spreadsheet_input->
				relation_mto1_isa_list );

	lookup_spreadsheet_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			lookup_spreadsheet_input->
				folder_attribute_append_isa_list );

	lookup_spreadsheet_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			lookup_spreadsheet_input->
				folder_attribute_append_isa_list );

	lookup_spreadsheet_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	lookup_spreadsheet_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			lookup_spreadsheet_input->
				folder_row_level_restriction->
				non_owner_forbid,
			lookup_spreadsheet_input->
				role->
				override_row_restrictions );

	lookup_spreadsheet_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_string_new(
			DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
			DICTIONARY_ELEMENT_DELIMITER,
			dictionary_string );

	lookup_spreadsheet_input->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			application_server_address(),
			data_directory,
			"spreadsheet" /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"csv" /* extension */ );

	lookup_spreadsheet_input->output_filename =
		lookup_spreadsheet_input->
			appaserver_link->
			appaserver_link_output->
			filename;

	lookup_spreadsheet_input->prompt_filename =
		lookup_spreadsheet_input->
			appaserver_link->
			appaserver_link_prompt->
			filename;

	lookup_spreadsheet_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_lookup_new(
			lookup_spreadsheet_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			lookup_spreadsheet_input->
				relation_mto1_list,
			lookup_spreadsheet_input->
				folder_attribute_name_list,
			lookup_spreadsheet_input->
				folder_attribute_date_name_list,
			lookup_spreadsheet_input->
				folder_attribute_append_isa_list );

	return lookup_spreadsheet_input;
}

LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input_calloc( void )
{
	LOOKUP_SPREADSHEET_INPUT *lookup_spreadsheet_input;

	if ( ! ( lookup_spreadsheet_input =
			calloc( 1,
				sizeof ( LOOKUP_SPREADSHEET_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return lookup_spreadsheet_input;
}

