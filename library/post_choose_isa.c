/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_isa.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "widget.h"
#include "sql.h"
#include "frameset.h"
#include "dictionary.h"
#include "table_insert.h"
#include "prompt_insert.h"
#include "table_edit.h"
#include "execute_system_string.h"
#include "post_choose_isa.h"

POST_CHOOSE_ISA *post_choose_isa_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *one_folder_name )
{
	POST_CHOOSE_ISA *post_choose_isa;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
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

	post_choose_isa = post_choose_isa_calloc();

	post_choose_isa->post_choose_isa_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_choose_isa_input_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			one_folder_name );

	post_choose_isa->primary_data_list =
		post_choose_isa_primary_data_list(
			post_choose_isa->
				post_choose_isa_input->
				one_folder->
				folder_attribute_primary_key_list,
			post_choose_isa->
				post_choose_isa_input->
				dictionary_separate->
				prompt_dictionary );

	post_choose_isa->primary_data_list_length =
		post_choose_isa_primary_data_list_length(
			post_choose_isa->primary_data_list );

	if ( !post_choose_isa->primary_data_list_length )
	{
		post_choose_isa->skip_system_string =
			post_choose_isa_skip_system_string(
				TABLE_INSERT_EXECUTABLE,
				PROMPT_INSERT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				folder_name,
				post_choose_isa->
					post_choose_isa_input->
					folder->
					appaserver_form,
				post_choose_isa->
					post_choose_isa_input->
					appaserver_error_filename );

		return post_choose_isa;
	}

	if ( list_length( post_choose_isa->primary_data_list ) !=
	     list_length( post_choose_isa->
				post_choose_isa_input->
				one_folder->
				folder_attribute_primary_key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
	"post_choose_isa_primary_data_list(%s) returned empty or incomplete.",
			list_display_delimited(
	     			post_choose_isa->
					post_choose_isa_input->
					one_folder->
					folder_attribute_primary_key_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_choose_isa->lookup_checkbox_checked =
		post_choose_isa_lookup_checkbox_checked(
			WIDGET_LOOKUP_CHECKBOX_NAME,
			post_choose_isa->
				post_choose_isa_input->
				dictionary_separate->
				non_prefixed_dictionary );

	if ( post_choose_isa->lookup_checkbox_checked )
	{
		post_choose_isa->post_choose_isa_lookup =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			post_choose_isa_lookup_new(
				session_key,
				login_name,
				role_name,
				post_choose_isa->
					post_choose_isa_input->
					one_folder,
				post_choose_isa->
					post_choose_isa_input->
					appaserver_error_filename,
				post_choose_isa->
					primary_data_list );
			
		post_choose_isa->execute_system_string_table_edit =
			post_choose_isa->
				post_choose_isa_lookup->
				execute_system_string_table_edit;

		return post_choose_isa;
	}

	post_choose_isa->query_dictionary =
		/* -------------------------- */
		/* Returns dictionary_small() */
		/* -------------------------- */
		post_choose_isa_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			post_choose_isa->
				post_choose_isa_input->
				one_folder->
				folder_attribute_primary_key_list,
			post_choose_isa->primary_data_list );

	post_choose_isa->prompt_dictionary =
		/* -------------------------- */
		/* Returns dictionary_small() */
		/* -------------------------- */
		post_choose_isa_prompt_dictionary(
			post_choose_isa->
				post_choose_isa_input->
				folder->
				folder_attribute_primary_key_list,
			post_choose_isa->primary_data_list );

	post_choose_isa->dictionary_separate_send_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			post_choose_isa->query_dictionary,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			(DICTIONARY *)0 /* drillthru_dictionary */,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			(DICTIONARY *)0 /* ignore_dictionary */,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			(DICTIONARY *)0 /* no_display_dictionary */,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			(DICTIONARY *)0 /* pair_one2m_dictionary */,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			post_choose_isa->prompt_dictionary
				/* non_prefixed_dictionary */ );

	post_choose_isa->dictionary_separate_send_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		dictionary_separate_send_string(
			post_choose_isa->
				dictionary_separate_send_dictionary );

	if ( post_choose_isa->
		post_choose_isa_input->
		folder->
		post_change_process_name )
	{
		post_choose_isa->post_change_process =
			process_fetch(
				post_choose_isa->
					post_choose_isa_input->
					folder->
					post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
				1 /* check_executable */,
				appaserver_parameter_mount_point() );
	}

	post_choose_isa->insert =
		insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_choose_isa->
				post_choose_isa_input->
				folder->
				folder_attribute_primary_key_list,
			post_choose_isa->
				post_choose_isa_input->
				folder->
				folder_attribute_list,
			(LIST *)0 /* relation_mto1_isa_list */,
			post_choose_isa->prompt_dictionary,
			(DICTIONARY *)0 /* multi_row_dictionary */,
			(LIST *)0 /* ignore_name_list */,
			post_choose_isa->post_change_process );

	return post_choose_isa;
}

POST_CHOOSE_ISA *post_choose_isa_calloc( void )
{
	POST_CHOOSE_ISA *post_choose_isa;

	if ( ! ( post_choose_isa = calloc( 1, sizeof ( POST_CHOOSE_ISA ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_choose_isa;
}

int post_choose_isa_primary_data_list_length(
		LIST *post_choose_isa_primary_data_list )
{
	return
	list_length( post_choose_isa_primary_data_list );
}

LIST *post_choose_isa_primary_data_list(
		LIST *primary_key_list,
		DICTIONARY *prompt_dictionary )
{
	if ( !dictionary_length( prompt_dictionary ) ) return NULL;

	return
	dictionary_data_list(
		primary_key_list /* attribute_name_list */,
		prompt_dictionary );
}

boolean post_choose_isa_lookup_checkbox_checked(
		const char *widget_lookup_checkbox_name,
		DICTIONARY *non_prefixed_dictionary )
{
	return
	widget_checkbox_dictionary_checked(
		(char *)widget_lookup_checkbox_name,
		non_prefixed_dictionary );
}

POST_CHOOSE_ISA_INPUT *post_choose_isa_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *one_folder_name )
{
	POST_CHOOSE_ISA_INPUT *post_choose_isa_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
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

	post_choose_isa_input = post_choose_isa_input_calloc();

	/* Must preceed folder_name fetch due to caching */
	/* --------------------------------------------- */
	post_choose_isa_input->one_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			one_folder_name,
			/* ----------------------------------- */
			/* Set to cache all folders for a role */
			/* ----------------------------------- */
			role_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_attribute */ );

	post_choose_isa_input->folder =
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
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_attribute */ );

	post_choose_isa_input->post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	post_choose_isa_input->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_choose_isa_input->
				one_folder->
				folder_attribute_list );

	post_choose_isa_input->dictionary_separate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_prompt_lookup_new(
			post_choose_isa_input->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			(LIST *)0 /* relation_mto1_list */,
			post_choose_isa_input->
				one_folder->
				folder_attribute_name_list,
			post_choose_isa_input->
				folder_attribute_date_name_list,
			post_choose_isa_input->
				one_folder->
				folder_attribute_list );

	post_choose_isa_input->role_attribute_exclude_list =
		role_attribute_exclude_list(
			role_name );

	post_choose_isa_input->role_attribute_exclude_name_list =
		role_attribute_exclude_name_list(
			ROLE_PERMISSION_INSERT,
			post_choose_isa_input->role_attribute_exclude_list );

	post_choose_isa_input->insert_login_name =
		/* -------------------------- */
		/* Returns login_name or null */
		/* -------------------------- */
		insert_login_name(
			login_name,
			post_choose_isa_input->
				role_attribute_exclude_name_list,
			post_choose_isa_input->
				folder->
				folder_attribute_list );

	if ( post_choose_isa_input->insert_login_name )
	{
		dictionary_set(
			post_choose_isa_input->
				dictionary_separate->
				prompt_dictionary,
			ROLE_LOGIN_NAME_COLUMN,
			post_choose_isa_input->insert_login_name );
	}

	post_choose_isa_input->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	return post_choose_isa_input;
}

POST_CHOOSE_ISA_INPUT *post_choose_isa_input_calloc( void )
{
	POST_CHOOSE_ISA_INPUT *post_choose_isa_input;

	if ( ! ( post_choose_isa_input =
			calloc( 1,
				sizeof ( POST_CHOOSE_ISA_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_choose_isa_input;
}

DICTIONARY *post_choose_isa_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *primary_key_list,
		LIST *primary_data_list )
{
	DICTIONARY *query_dictionary;
	char *primary_key;
	char *primary_data;
	char *relation_key;

	if ( !list_rewind( primary_key_list )
	||   !list_rewind( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"cannot align key=[%s] with data=[%s].",
			list_display( primary_key_list ),
			list_display( primary_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_dictionary = dictionary_small();

	do {
		primary_key = list_get( primary_key_list );
		primary_data = list_get( primary_data_list );

		dictionary_set(
			query_dictionary,
			primary_key,
			primary_data );

		relation_key =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_relation_key(
				(char *)query_relation_operator_prefix,
				primary_key );

		dictionary_set(
			query_dictionary,
			strdup( relation_key ),
			(char *)query_equal );

		list_next( primary_data_list );

	} while ( list_next( primary_key_list ) );

	return query_dictionary;
}

POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup_new(
		char *session_key,
		char *login_name,
		char *role_name,
		FOLDER *one_folder,
		char *appaserver_error_filename,
		LIST *primary_data_list )
{
	POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup;

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !one_folder
	||   !appaserver_error_filename
	||   !list_length( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_choose_isa_lookup = post_choose_isa_lookup_calloc();

	post_choose_isa_lookup->post_choose_isa_query_dictionary =
		/* -------------------------- */
		/* Returns dictionary_small() */
		/* -------------------------- */
		post_choose_isa_query_dictionary(
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			one_folder->folder_attribute_primary_key_list,
			primary_data_list );

	post_choose_isa_lookup->dictionary_separate_send_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			post_choose_isa_lookup->
				post_choose_isa_query_dictionary,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			(DICTIONARY *)0 /* drillthru_dictionary */,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			(DICTIONARY *)0 /* ignore_dictionary */,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			(DICTIONARY *)0 /* no_display_dictionary */,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			(DICTIONARY *)0 /* pair_one2m_dictionary */,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			(DICTIONARY *)0 /* non_prefixed_dictionary */ );

	post_choose_isa_lookup->dictionary_separate_send_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		dictionary_separate_send_string(
			post_choose_isa_lookup->
				dictionary_separate_send_dictionary );

	post_choose_isa_lookup->execute_system_string_table_edit =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_table_edit(
			TABLE_EDIT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			one_folder->folder_name,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			post_choose_isa_lookup->
				dictionary_separate_send_string,
			appaserver_error_filename );

	return post_choose_isa_lookup;
}

POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup_calloc( void )
{
	POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup;

	if ( ! ( post_choose_isa_lookup =
			calloc( 1,
				sizeof ( POST_CHOOSE_ISA_LOOKUP ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_choose_isa_lookup;
}

DICTIONARY *post_choose_isa_prompt_dictionary(
		LIST *primary_key_list,
		LIST *primary_data_list )
{
	DICTIONARY *prompt_dictionary;
	char *primary_key;
	char *primary_data;

	if ( !list_rewind( primary_key_list )
	||   !list_rewind( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		char message[ 128 ];

		sprintf(message,
		"cannot align key=[%s] with data=[%s].",
			list_display( primary_key_list ),
			list_display( primary_data_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	prompt_dictionary = dictionary_small();

	do {
		primary_key = list_get( primary_key_list );
		primary_data = list_get( primary_data_list );

		dictionary_set(
			prompt_dictionary,
			primary_key,
			primary_data );

		list_next( primary_data_list );

	} while ( list_next( primary_key_list ) );

	return prompt_dictionary;
}

char *post_choose_isa_skip_system_string(
		const char *table_insert_executable,
		const char *prompt_insert_executable,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *folder_appaserver_form,
		char *appaserver_error_filename )
{
	char *skip_system_string;

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !folder_appaserver_form
	||   !appaserver_error_filename )
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

	if ( strcmp(
		FORM_APPASERVER_TABLE,
		folder_appaserver_form ) == 0 )
	{
		skip_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_table_insert(
				table_insert_executable,
				session_key,
				login_name,
				role_name,
				folder_name,
				FRAMESET_PROMPT_FRAME /* target_frame */,
				(char *)0 /* results_string */,
				(char *)0 /* error_string */,
				(char *)0 /* dictionary_separate_send_string */,
				appaserver_error_filename );
	}
	else
	{
		skip_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_prompt_insert(
				prompt_insert_executable,
				session_key,
				login_name,
				role_name,
				folder_name,
				(char *)0 /* dictionary_separate_send_string */,
				appaserver_error_filename );
	}

	return skip_system_string;
}

