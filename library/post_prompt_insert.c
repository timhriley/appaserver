/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_prompt_insert.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "query.h"
#include "frameset.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder_attribute.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "appaserver_error.h"
#include "prompt_insert.h"
#include "table_insert.h"
#include "pair_one2m.h"
#include "vertical_new_checkbox.h"
#include "create_table.h"
#include "post_prompt_insert.h"

POST_PROMPT_INSERT *post_prompt_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	POST_PROMPT_INSERT *post_prompt_insert;

	if ( !argc
	||   !argv
	||   !application_name
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

	post_prompt_insert = post_prompt_insert_calloc();

	post_prompt_insert->session_folder =
		/* -------------------------------------------- */
		/* Sets appaserver environment and outputs argv */
		/* Each parameter is security inspected.	*/
		/* -------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_INSERT_STATE,
			(char *)0 /* state2 */ );

	post_prompt_insert->post_prompt_insert_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_prompt_insert_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	post_prompt_insert->vertical_new_prompt =
		vertical_new_prompt_new(
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				non_prefixed_dictionary,
			VERTICAL_NEW_CHECKBOX_PREFIX,
			VERTICAL_NEW_PROMPT_ONE_HIDDEN_LABEL,
			VERTICAL_NEW_PROMPT_MANY_HIDDEN_LABEL,
			TABLE_INSERT_EXECUTABLE,
			FRAMESET_TABLE_FRAME,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name /* many_folder_name */,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				drillthru_dictionary );

	if ( post_prompt_insert->vertical_new_prompt )
	{
		return post_prompt_insert;
	}

	if ( !post_prompt_insert->post_prompt_insert_input->lookup_boolean )
	{
		post_prompt_insert->insert =
			insert_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_prompt_insert->
					post_prompt_insert_input->
					folder->
					folder_attribute_primary_key_list
					/* root_primary_key_list */,
				post_prompt_insert->
					post_prompt_insert_input->
					folder_attribute_append_isa_list,
				post_prompt_insert->
					post_prompt_insert_input->
					relation_mto1_list,
				post_prompt_insert->
					post_prompt_insert_input->
					relation_mto1_isa_list,
				post_prompt_insert->
					post_prompt_insert_input->
					dictionary_separate->
					prompt_dictionary,
				(DICTIONARY *)0 /* multi_row_dictionary */,
				post_prompt_insert->
					post_prompt_insert_input->
					dictionary_separate->
					ignore_name_list,
				post_prompt_insert->
					post_prompt_insert_input->
					post_change_process );

		post_prompt_insert->pair_one2m_post_prompt_insert =
			pair_one2m_post_prompt_insert_new(
				PAIR_ONE2M_ONE_FOLDER_KEY,
				PAIR_ONE2M_MANY_FOLDER_KEY,
				PAIR_ONE2M_UNFULFILLED_LIST_KEY,
				PAIR_ONE2M_FOLDER_DELIMITER,
				post_prompt_insert->
					post_prompt_insert_input->
					dictionary_separate->
					pair_dictionary );
	}

	if ( post_prompt_insert->
		pair_one2m_post_prompt_insert
	&&   post_prompt_insert->
		pair_one2m_post_prompt_insert->
		one_folder_name
	&&   !post_prompt_insert->insert )
	{
		post_prompt_insert->missing_primary_key_list =
			post_prompt_insert_missing_primary_key_list(
				post_prompt_insert->
					post_prompt_insert_input->
					folder->
					folder_attribute_primary_key_list,
				post_prompt_insert->
					post_prompt_insert_input->
					dictionary_separate->
					prompt_dictionary );

		if ( !list_length(
			post_prompt_insert->missing_primary_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
	"post_prompt_insert_missing_primary_key_list(%s) returned empty.",
				list_display(
				    post_prompt_insert->
					  post_prompt_insert_input->
					  folder->
					  folder_attribute_primary_key_list ) );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		post_prompt_insert->missing_display =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_prompt_insert_missing_display(
				POST_PROMPT_INSERT_MISSING_TEMPLATE,
				post_prompt_insert->missing_primary_key_list );

		return post_prompt_insert;
	}

	post_prompt_insert->query_dictionary =
		post_prompt_insert_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				prompt_dictionary );

	post_prompt_insert->dictionary_separate_send_dictionary =
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			post_prompt_insert->query_dictionary,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				drillthru_dictionary,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				ignore_dictionary,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				ignore_dictionary
				/* no_display_dictionary if lookup button */,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				pair_dictionary,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			post_prompt_insert->
				post_prompt_insert_input->
				dictionary_separate->
				prompt_dictionary
				/* non_prefixed_dictionary */ );

	post_prompt_insert->dictionary_separate_send_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		dictionary_separate_send_string(
			post_prompt_insert->
				dictionary_separate_send_dictionary );

	return post_prompt_insert;
}

POST_PROMPT_INSERT *post_prompt_insert_calloc( void )
{
	POST_PROMPT_INSERT *post_prompt_insert;

	if ( ! ( post_prompt_insert =
			calloc( 1, sizeof ( POST_PROMPT_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_prompt_insert;
}

DICTIONARY *post_prompt_insert_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *prompt_dictionary )
{
	DICTIONARY *query_dictionary;
	LIST *key_list;
	char *key;
	char *relation_key;

	if ( !dictionary_length( prompt_dictionary ) ) return NULL;

	query_dictionary = dictionary_small();

	key_list = dictionary_key_list( prompt_dictionary );

	list_rewind( key_list );

	do {
		key = list_get( key_list );

		dictionary_set(
			query_dictionary,
			key,
			/* --------------------------------------- */
			/* Returns component of dictionary or null */
			/* --------------------------------------- */
			dictionary_get(
				key,
				prompt_dictionary ) );

		relation_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_relation_key(
				(char *)query_relation_operator_prefix,
				key );

		dictionary_set(
			query_dictionary,
			strdup( relation_key ),
			(char *)query_equal );

	} while ( list_next( key_list ) );

	return query_dictionary;
}

boolean post_prompt_insert_input_lookup_boolean(
		char *prompt_insert_lookup_checkbox,
		DICTIONARY *non_prefixed_dictionary )
{
	if ( dictionary_get(
		prompt_insert_lookup_checkbox,
		non_prefixed_dictionary ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

POST_PROMPT_INSERT_INPUT *post_prompt_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	POST_PROMPT_INSERT_INPUT *post_prompt_insert_input;

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

	post_prompt_insert_input = post_prompt_insert_input_calloc();

	post_prompt_insert_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	post_prompt_insert_input->role_attribute_exclude_insert_name_list =
		role_attribute_exclude_name_list(
			APPASERVER_INSERT_STATE,
			post_prompt_insert_input->
				role->
				role_attribute_exclude_list );

	post_prompt_insert_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			post_prompt_insert_input->
				role_attribute_exclude_insert_name_list,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	post_prompt_insert_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			post_prompt_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	post_prompt_insert_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			post_prompt_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	post_prompt_insert_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			post_prompt_insert_input->
				folder->
				folder_attribute_list,
			post_prompt_insert_input->relation_mto1_isa_list );

	post_prompt_insert_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_prompt_insert_input->folder_attribute_upload_filename_list =
		folder_attribute_upload_filename_list(
			post_prompt_insert_input->
				folder_attribute_append_isa_list );

	post_prompt_insert_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			application_name,
			post_prompt_insert_input->
				appaserver_parameter->
				upload_directory,
			post_prompt_insert_input->
				folder_attribute_append_isa_list );

	post_prompt_insert_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			post_prompt_insert_input->
				folder_attribute_append_isa_list );

	post_prompt_insert_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_prompt_insert_input->
				folder_attribute_append_isa_list );

	post_prompt_insert_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_prompt_insert_new(
			post_prompt_insert_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_prompt_insert_input->
				relation_mto1_list,
			post_prompt_insert_input->
				folder_attribute_append_isa_list,
			post_prompt_insert_input->
				folder_attribute_name_list,
			post_prompt_insert_input->
				folder_attribute_date_name_list );

	post_prompt_insert_input->lookup_boolean =
		post_prompt_insert_input_lookup_boolean(
			PROMPT_INSERT_LOOKUP_CHECKBOX,
			post_prompt_insert_input->
				dictionary_separate->
				non_prefixed_dictionary );

	if ( !post_prompt_insert_input->lookup_boolean )
	{
		if ( post_prompt_insert_input->
			folder->
			post_change_process_name )
		{
			post_prompt_insert_input->post_change_process =
				process_fetch(
					post_prompt_insert_input->
						folder->
						post_change_process_name,
					/* Not fetching javascript */
					(char *)0 /* document_root */,
					(char *)0 /* relative_source */,
					1 /* check_executable_inside */,
					appaserver_parameter_mount_point() );
		}

		post_prompt_insert_input->insert_appaserver_user =
			insert_appaserver_user(
				APPASERVER_USER_FULL_NAME,
				login_name,
				post_prompt_insert_input->
				     role_attribute_exclude_insert_name_list,
				post_prompt_insert_input->
				     folder_attribute_append_isa_list );

		if ( post_prompt_insert_input->insert_appaserver_user )
		{
			dictionary_set(
				post_prompt_insert_input->
					dictionary_separate->
					prompt_dictionary,
				APPASERVER_USER_FULL_NAME,
				post_prompt_insert_input->
					insert_appaserver_user->
					full_name );

			dictionary_set(
				post_prompt_insert_input->
					dictionary_separate->
					prompt_dictionary,
				APPASERVER_USER_STREET_ADDRESS,
				post_prompt_insert_input->
					insert_appaserver_user->
					street_address );
		}
	}

	post_prompt_insert_input->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	return post_prompt_insert_input;
}

POST_PROMPT_INSERT_INPUT *post_prompt_insert_input_calloc( void )
{
	POST_PROMPT_INSERT_INPUT *post_prompt_insert_input;

	if ( ! ( post_prompt_insert_input =
			calloc( 1,
				sizeof ( POST_PROMPT_INSERT_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_prompt_insert_input;
}

LIST *post_prompt_insert_missing_primary_key_list(
		LIST *folder_attribute_primary_key_list,
		DICTIONARY *prompt_dictionary )
{
	LIST *missing_primary_key_list = list_new();
	char *attribute_name;
	INSERT_DATUM *insert_datum;

	if ( !list_rewind( folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"folder_attribute_primary_key_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	do {
		attribute_name =
			list_get(
				folder_attribute_primary_key_list );

		insert_datum =
			insert_datum_extract(
				prompt_dictionary,
				(DICTIONARY *)0 /* multi_row_dictionary */,
				0 /* row_number */,
				attribute_name,
				1 /* primary_key_index */,
				0 /* attribute_is_number */ );

		if ( !insert_datum )
		{
			list_set(
				missing_primary_key_list,
				attribute_name );
		}

	} while ( list_next( folder_attribute_primary_key_list ) );

	return missing_primary_key_list;
}

char *post_prompt_insert_missing_display(
		char *post_prompt_insert_missing_template,
		LIST *post_prompt_insert_missing_primary_key_list )
{
	char display[ 1024 ];
	char buffer[ 512 ];

	if ( !post_prompt_insert_missing_template
	||   !list_length( post_prompt_insert_missing_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		display,
		sizeof ( display ),
		post_prompt_insert_missing_template,
		string_initial_capital(
			buffer,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			list_display_delimited_plus_space(
				post_prompt_insert_missing_primary_key_list,
				',' ) ) );

	return strdup( display );
}

boolean post_prompt_insert_fatal_duplicate_boolean(
		const char *create_table_unique_suffix,
		const char *create_table_additional_suffix,
		char *appaserver_table_name,
		LIST *primary_key_list,
		char *insert_statement_error_string )
{
	char *primary_key;

	if ( !appaserver_table_name
	||   !list_length( primary_key_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !insert_statement_error_string ) return 0;

	if ( string_exists(
		insert_statement_error_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_name(
			(char *)create_table_unique_suffix,
			appaserver_table_name ) /* substring */ ) )
	{
		return 0;
	}

	if ( string_exists(
		insert_statement_error_string,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		create_table_unique_index_name(
			(char *)0 /* create_table_unique_suffix */,
			appaserver_table_name ) /* substring */ ) )
	{
		return 0;
	}

	if ( list_rewind( primary_key_list ) )
	do {
		primary_key = list_get( primary_key_list );

		if ( string_exists(
			insert_statement_error_string,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			create_table_additional_unique_name(
				create_table_additional_suffix,
				primary_key /* attribute_name */ ) ) )
		{
			return 0;
		}

	} while ( list_next( primary_key_list ) );

	return 1;
}
