/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_drillthru.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver.h"
#include "appaserver_error.h"
#include "execute_system_string.h"
#include "prompt_insert.h"
#include "prompt_lookup.h"
#include "post_drillthru.h"

POST_DRILLTHRU *post_drillthru_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *current_folder_name,
			char *state,
			char *appaserver_data_directory,
			DICTIONARY *original_post_dictionary )
{
	POST_DRILLTHRU *post_drillthru;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !state
	||   !current_folder_name
	||   !appaserver_data_directory )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !dictionary_length( original_post_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "origin_post_dictionary is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_drillthru = post_drillthru_calloc();

	post_drillthru->base_folder_name =
		/* --------------------------------------------- */
		/* Returns component of original_post_dictionary */
		/* --------------------------------------------- */
		post_drillthru_base_folder_name(
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			DRILLTHRU_BASE_KEY,
			original_post_dictionary );

	if ( !post_drillthru->base_folder_name )
	{
		char message[ 128 ];

		sprintf(message,
			"post_drillthru_base_folder_name() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_drillthru->folder_attribute_list =
		folder_attribute_list(
			post_drillthru->base_folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			1 /* fetch_attribute */,
			1 /* cache_boolean */ );

	post_drillthru->folder_attribute_date_name_list =
		folder_attribute_date_name_list(
			post_drillthru->folder_attribute_list );

	post_drillthru->dictionary_separate_drillthru =
		dictionary_separate_drillthru_new(
			original_post_dictionary,
			application_name,
			login_name,
			post_drillthru->folder_attribute_date_name_list,
			post_drillthru->folder_attribute_list );

/* --------------------------------------------------------------------------- 
Two notes:
1) No memory gets allocated. Both the key and data still exist
   in the append_dictionary dictionary.
2) If a they share a key, then it doesn't clobber the source_destinatation.
--------------------------------------------------------------------------- */
	dictionary_append_dictionary(
		post_drillthru->
			dictionary_separate_drillthru->
			drillthru_dictionary,
		post_drillthru->
			dictionary_separate_drillthru->
			non_prefixed_dictionary );

	post_drillthru->drillthru_continue =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drillthru_continue_new(
			post_drillthru->
				dictionary_separate_drillthru->
				drillthru_dictionary /* in/out */,
			application_name,
			session_key,
			login_name,
			role_name,
			current_folder_name,
			state,
			appaserver_data_directory );

	if ( post_drillthru->drillthru_continue->drillthru_document )
	{
		return post_drillthru;
	}

	post_drillthru->dictionary_separate_send_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			(DICTIONARY *)0 /* query_dictionary */,
			DICTIONARY_SEPARATE_QUERY_PREFIX,
			post_drillthru->
				dictionary_separate_drillthru->
				drillthru_dictionary,
			DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
			(DICTIONARY *)0 /* ignore_dictionary */,
			DICTIONARY_SEPARATE_IGNORE_PREFIX,
			(DICTIONARY *)0 /* no_display_dictionary */,
			DICTIONARY_SEPARATE_NO_DISPLAY_PREFIX,
			(DICTIONARY *)0 /* pair_dictionary */,
			DICTIONARY_SEPARATE_PAIR_PREFIX,
			(DICTIONARY *)0 /* non_prefixed_dictionary */ );

	post_drillthru->dictionary_separate_send_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		dictionary_separate_send_string(
			post_drillthru->
				dictionary_separate_send_dictionary
					/* replaced_send_dictionary */ );

	post_drillthru->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename( application_name );

	if ( strcmp( state, APPASERVER_INSERT_STATE ) == 0 )
	{
		post_drillthru->system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_prompt_insert(
				PROMPT_INSERT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				post_drillthru->base_folder_name,
				post_drillthru->
					dictionary_separate_send_string,
				post_drillthru->
					appaserver_error_filename );
	}
	else
	{
		post_drillthru->system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			execute_system_string_prompt_lookup(
				PROMPT_LOOKUP_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				post_drillthru->base_folder_name,
				post_drillthru->
					dictionary_separate_send_string,
				post_drillthru->
					appaserver_error_filename );
	}

	return post_drillthru;
}

POST_DRILLTHRU *post_drillthru_calloc( void )
{
	POST_DRILLTHRU *post_drillthru;

	if ( ! ( post_drillthru = calloc( 1, sizeof ( POST_DRILLTHRU ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return post_drillthru;
}

char *post_drillthru_base_folder_name(
			char *dictionary_separate_drillthru_prefix,
			char *drillthru_base_key,
			DICTIONARY *original_post_dictionary )
{
	char key[ 128 ];

	if ( !dictionary_separate_drillthru_prefix
	||   !drillthru_base_key
	||   !dictionary_length( original_post_dictionary ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(key,
		"%s%s",
		dictionary_separate_drillthru_prefix,
		drillthru_base_key );

	return
	/* --------------------------------------- */
	/* Returns component of dictionary or null */
	/* --------------------------------------- */
	dictionary_get(
		key,
		original_post_dictionary );
}
