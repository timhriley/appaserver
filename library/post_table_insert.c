/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_table_insert.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder_attribute.h"
#include "dictionary.h"
#include "frameset.h"
#include "appaserver_error.h"
#include "table_insert.h"
#include "execute_system_string.h"
#include "vertical_new_prompt.h"
#include "table_edit.h"
#include "post_prompt_insert.h"
#include "post_table_insert.h"

POST_TABLE_INSERT *post_table_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	POST_TABLE_INSERT *post_table_insert;

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

	post_table_insert = post_table_insert_calloc();

	post_table_insert->session_folder =
		/* -------------------------------------------- */
		/* Sets appaserver environment and outputs argv.*/
		/* Each parameter is security inspected.        */
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

	post_table_insert->post_table_insert_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_table_insert_input_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name );

	if ( post_table_insert->
		post_table_insert_input->
		insert_appaserver_user )
	{
		dictionary_set(
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				prompt_dictionary,
			APPASERVER_USER_FULL_NAME,
			post_table_insert->
				post_table_insert_input->
				insert_appaserver_user->
				full_name );

		dictionary_set(
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				prompt_dictionary,
			APPASERVER_USER_STREET_ADDRESS,
			post_table_insert->
				post_table_insert_input->
				insert_appaserver_user->
				street_address );
	}

	post_table_insert->vertical_new_post =
		vertical_new_post_new(
			VERTICAL_NEW_PROMPT_MANY_HIDDEN_LABEL,
			FRAMESET_PROMPT_FRAME,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			application_name,
			session_key,
			login_name,
			role_name,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				drillthru_dictionary,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				non_prefixed_dictionary,
			post_table_insert->
				post_table_insert_input->
				appaserver_parameter->
				data_directory );

	post_table_insert->query_dictionary =
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		post_table_insert_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				prompt_dictionary,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				multi_row_dictionary,
			post_table_insert->vertical_new_post );

	post_table_insert->prompt_dictionary =
		/* ----------------------------------------------- */
		/* Returns prompt_dictionary or dictionary_small() */
		/* ----------------------------------------------- */
		post_table_insert_prompt_dictionary(
			TABLE_EDIT_ONLOAD_JAVASCRIPT_KEY,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				prompt_dictionary /* in/out */,
			post_table_insert->vertical_new_post );

	if ( post_table_insert->
		post_table_insert_input->
			relation_mto1_automatic_preselection )
	{
		post_table_insert->insert =
			insert_automatic_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_table_insert->
					post_table_insert_input->
					folder->
					folder_attribute_primary_key_list,
				post_table_insert->
					post_table_insert_input->
					folder_attribute_append_isa_list,
				post_table_insert->
					post_table_insert_input->
					relation_mto1_isa_list,
				post_table_insert->
					post_table_insert_input->
					dictionary_separate->
					prompt_dictionary,
				post_table_insert->
					post_table_insert_input->
					dictionary_separate->
					multi_row_dictionary,
				post_table_insert->
					post_table_insert_input->
					post_change_process );
	}
	else
	{
		post_table_insert->insert =
			insert_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_table_insert->
					post_table_insert_input->
					folder->
					folder_attribute_primary_key_list
					/* root_primary_key_list */,
				post_table_insert->
					post_table_insert_input->
					folder_attribute_append_isa_list,
				post_table_insert->
					post_table_insert_input->
					relation_mto1_list,
				post_table_insert->
					post_table_insert_input->
					relation_mto1_isa_list,
				post_table_insert->
					post_table_insert_input->
					dictionary_separate->
					prompt_dictionary,
				post_table_insert->
					post_table_insert_input->
					dictionary_separate->
					multi_row_dictionary
					/* in/out */,
				post_table_insert->
					post_table_insert_input->
					dictionary_separate->
					ignore_name_list,
				post_table_insert->
					post_table_insert_input->
					post_change_process );
	}

	post_table_insert->pair_one2m_post_table_insert =
		pair_one2m_post_table_insert_new(
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				pair_dictionary /* in/out */,
			PAIR_ONE2M_UNFULFILLED_LIST_KEY,
			PAIR_ONE2M_FULFILLED_LIST_KEY,
			PAIR_ONE2M_FOLDER_DELIMITER,
			folder_name /* many_folder_name */ );

	post_table_insert->dictionary_separate_send_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			post_table_insert->query_dictionary,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				drillthru_dictionary,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				ignore_dictionary,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			(DICTIONARY *)0 /* no_display_dictionary */,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			post_table_insert->
				post_table_insert_input->
				dictionary_separate->
				pair_dictionary,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			post_table_insert->prompt_dictionary
				/* non_prefixed_dictionary */ );

	post_table_insert->dictionary_separate_send_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_separate_send_string(
			post_table_insert->
				dictionary_separate_send_dictionary );

	return post_table_insert;
}

POST_TABLE_INSERT *post_table_insert_calloc( void )
{
	POST_TABLE_INSERT *post_table_insert;

	if ( ! ( post_table_insert =
			calloc( 1, sizeof ( POST_TABLE_INSERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_table_insert;
}

POST_TABLE_INSERT_INPUT *post_table_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	POST_TABLE_INSERT_INPUT *post_table_insert_input;

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

	post_table_insert_input = post_table_insert_input_calloc();

	post_table_insert_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	post_table_insert_input->role_attribute_exclude_insert_name_list =
		role_attribute_exclude_insert_name_list(
			APPASERVER_INSERT_STATE,
			post_table_insert_input->
				role->
				role_attribute_exclude_list );

	post_table_insert_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			post_table_insert_input->
				role_attribute_exclude_insert_name_list,
			/* -------------------------------------- */
			/* Sets folder_attribute_primary_list	  */
			/* Sets folder_attribute_primary_key_list */
			/* Sets folder_attribute_name_list	  */
			/* -------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	post_table_insert_input->relation_mto1_list =
		relation_mto1_list(
			folder_name
				/* many_folder_name */,
			post_table_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_folder_primary_key_list */ );

	post_table_insert_input->relation_mto1_automatic_preselection =
		relation_mto1_automatic_preselection(
			post_table_insert_input->
				relation_mto1_list );

	post_table_insert_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			post_table_insert_input->
				folder->
				folder_attribute_primary_key_list
				/* many_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	post_table_insert_input->folder_attribute_append_isa_list =
		folder_attribute_append_isa_list(
			post_table_insert_input->
				folder->
				folder_attribute_list,
			post_table_insert_input->
				relation_mto1_isa_list );

	post_table_insert_input->folder_attribute_name_list =
		folder_attribute_name_list(
			(char *)0 /* folder_name */,
			post_table_insert_input->
				folder_attribute_append_isa_list );

	post_table_insert_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_table_insert_input->
				folder_attribute_append_isa_list );

	post_table_insert_input->folder_attribute_upload_filename_list =
		folder_attribute_upload_filename_list(
			post_table_insert_input->
				folder_attribute_append_isa_list );

	post_table_insert_input->appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	post_table_insert_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			application_name,
			post_table_insert_input->
				appaserver_parameter->
				upload_directory,
			post_table_insert_input->
				folder_attribute_upload_filename_list );

	post_table_insert_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_post_table_insert_new(
			post_table_insert_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			post_table_insert_input->
				relation_mto1_list,
			post_table_insert_input->
				folder_attribute_name_list,
			post_table_insert_input->
				folder_attribute_date_name_list,
			post_table_insert_input->
				folder_attribute_append_isa_list );

	if ( post_table_insert_input->folder->post_change_process_name )
	{
		post_table_insert_input->post_change_process =
			process_fetch(
				post_table_insert_input->
					folder->
					post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
				1 /* check_executable_inside_filesystem */,
				post_table_insert_input->
					appaserver_parameter->
					mount_point );
	}

	post_table_insert_input->insert_appaserver_user =
		insert_appaserver_user(
			APPASERVER_USER_FULL_NAME,
			login_name,
			post_table_insert_input->
				role_attribute_exclude_insert_name_list,
			post_table_insert_input->
				folder_attribute_append_isa_list );

	return post_table_insert_input;
}

POST_TABLE_INSERT_INPUT *post_table_insert_input_calloc( void )
{
	POST_TABLE_INSERT_INPUT *post_table_insert_input;

	if ( ! ( post_table_insert_input =
			calloc( 1, sizeof ( POST_TABLE_INSERT_INPUT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_table_insert_input;
}

DICTIONARY *post_table_insert_prompt_dictionary(
		const char *onload_javascript_key,
		DICTIONARY *prompt_dictionary,
		VERTICAL_NEW_POST *vertical_new_post )
{
	if ( !prompt_dictionary ) prompt_dictionary = dictionary_small();

	if ( vertical_new_post )
	{
		dictionary_set(
			prompt_dictionary,
			(char *)onload_javascript_key,
			vertical_new_post->onload_javascript_string );
	}

	return prompt_dictionary;
}

DICTIONARY *post_table_insert_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		VERTICAL_NEW_POST *vertical_new_post )
{
	if ( !vertical_new_post )
	{
		return
		post_prompt_insert_query_dictionary(
			query_relation_operator_prefix,
			query_equal,
			prompt_dictionary );
	}

	return
	post_prompt_insert_query_dictionary(
		query_relation_operator_prefix,
		query_equal,
		/* ---------------------------------- */
		/* Returns dictionary_small() or null */
		/* ---------------------------------- */
		dictionary_remove_index(
			multi_row_dictionary ) );
}

