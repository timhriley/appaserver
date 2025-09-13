/* -------------------------------------------------------------------- */
/* APPASERVER_HOME/library/vertical_new_prompt.c			*/
/* -------------------------------------------------------------------- */
/* This controls the vertical new checkboxes on the prompt insert form.	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "dictionary_separate.h"
#include "execute_system_string.h"
#include "vertical_new_prompt.h"

VERTICAL_NEW_PROMPT *vertical_new_prompt_new(
		DICTIONARY *non_prefixed_dictionary,
		const char *vertical_new_checkbox_prefix,
		const char *vertical_new_prompt_one_hidden_label,
		const char *vertical_new_prompt_many_hidden_label,
		const char *table_insert_executable,
		const char *frameset_table_frame,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		DICTIONARY *drillthru_dictionary )
{
	VERTICAL_NEW_PROMPT *vertical_new_prompt;
	DICTIONARY *dictionary;
	char *send_string;

	if ( !application_name
	||   !login_name
	||   !session_key
	||   !role_name
	||   !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	vertical_new_prompt = vertical_new_prompt_calloc();

	if ( ! ( vertical_new_prompt->one_folder_name =
		/* ---------------------------------------------------- */
		/* Returns component of non_prefixed_dictionary or null */
		/* ---------------------------------------------------- */
			vertical_new_prompt_one_folder_name(
				vertical_new_checkbox_prefix,
				non_prefixed_dictionary ) ) )
	{
		free( vertical_new_prompt );
		return NULL;
	}

	vertical_new_prompt->many_folder_name = many_folder_name;

	dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		vertical_new_prompt_dictionary(
			vertical_new_prompt_one_hidden_label,
			vertical_new_prompt_many_hidden_label,
			many_folder_name,
			vertical_new_prompt->one_folder_name );

	send_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_separate_send_string(
			dictionary_separate_send_dictionary(
				(DICTIONARY *)0 /* sort_dictionary */,
				DICTIONARY_SEPARATE_SORT_PREFIX,
				(DICTIONARY *)0 /* query_dictionary */,
				DICTIONARY_SEPARATE_QUERY_PREFIX,
				drillthru_dictionary,
				DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
				(DICTIONARY *)0 /* ignore_dictionary */,
				DICTIONARY_SEPARATE_IGNORE_PREFIX,
				(DICTIONARY *)0 /* no_display_dictionary */,
				DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
				(DICTIONARY *)0 /* pair_dictionary */,
				DICTIONARY_SEPARATE_PAIR_PREFIX,
				dictionary /* non_prefixed_dictionary */ ) );

	vertical_new_prompt->execute_system_string_table_insert =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_table_insert(
			table_insert_executable,
			session_key,
			login_name,
			role_name,
			vertical_new_prompt->
				one_folder_name /* folder_name */,
			(char *)frameset_table_frame /* target_frame */,
			(char *)0 /* result_string */,
			(char *)0 /* error_string */,
			send_string /* dictionary_separate_send_string */,
			appaserver_error_filename( application_name ) );

	return vertical_new_prompt;
}

VERTICAL_NEW_PROMPT *vertical_new_prompt_calloc( void )
{
	VERTICAL_NEW_PROMPT *vertical_new_prompt;

	if ( ! ( vertical_new_prompt =
		    calloc( 1, sizeof ( VERTICAL_NEW_PROMPT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return vertical_new_prompt;
}

char *vertical_new_prompt_one_folder_name(
		const char *vertical_new_prefix,
		DICTIONARY *non_prefixed_dictionary )
{
	LIST *prefix_key_list;
	char *first;
	char *one_folder_name = {0};

	prefix_key_list =
		dictionary_prefix_key_list(
			vertical_new_prefix,
			non_prefixed_dictionary );


	first = list_first( prefix_key_list );

	if ( first )
	{
		one_folder_name = first + strlen( vertical_new_prefix );
	}

	return one_folder_name;
}

DICTIONARY *vertical_new_prompt_dictionary(
		const char *vertical_new_prompt_one_hidden_label,
		const char *vertical_new_prompt_many_hidden_label,
		char *many_folder_name,
		char *vertical_new_prompt_one_folder_name )
{
	DICTIONARY *dictionary = dictionary_small();

	if ( !many_folder_name
	||   !vertical_new_prompt_one_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	dictionary_set(
		dictionary,
		(char *)vertical_new_prompt_one_hidden_label,
		vertical_new_prompt_one_folder_name );

	dictionary_set(
		dictionary,
		(char *)vertical_new_prompt_many_hidden_label,
		many_folder_name );

	return dictionary;
}
