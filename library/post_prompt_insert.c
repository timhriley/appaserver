/* $APPASERVER_HOME/library/post_prompt_insert.c			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "folder_attribute.h"
#include "role_operation.h"
#include "dictionary.h"
#include "dictionary_separate.h"
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
	POST_PROMPT_INSERT *post_prompt_insert = post_prompt_insert_calloc();


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
			APPASERVER_INSERT_STATE );

	post_prompt_insert->role =
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	post_prompt_insert->folder =
		folder_fetch(
			folder_name,
			(char *)0 /* role_name */,
			role_exclude_update_attribute_name_list(
				post_prompt_insert->
					role->
					attribute_exclude_list ),
			/* Also sets primary_key_list */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			1 /* fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	post_prompt_insert->post_dictionary =
		post_dictionary_stdin_new(
			(char *)0 /* upload_directory */,
			(char *)0 /* session_key */ );

	post_prompt_insert->dictionary_separate_post_prompt_insert =
		dictionary_separate_post_prompt_insert_new(
			post_prompt_insert->
				post_dictionary->
				original_post_dictionary,
			application_name,
			login_name,
			folder_attribute_name_list(
				post_prompt_insert->
					folder->
					folder_attribute_append_isa_list ),
			folder_attribute_date_name_list(
				post_prompt_insert->
					folder->
					folder_attribute_append_isa_list ),
			post_prompt_insert->
				folder->
				folder_attribute_append_isa_list );

	post_prompt_insert->insert =
		insert_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			post_prompt_insert->folder->primary_key_list,
			post_prompt_insert->folder->folder_attribute_name_list,
			post_prompt_insert->
				dictionary_separate_post_prompt_insert->
				row_zero_dictionary,
			(DICTIONARY *)0 /* multi_row_dictionary */,
			post_prompt_insert->
				dictionary_separate_post_prompt_insert->
				ignore_name_list,
			(post_prompt_insert->
				folder->
				post_change_process)
					? post_prompt_insert->
						folder->
						post_change_process->
						process_name
					: (char *)0,
			(post_prompt_insert->
				folder->
				post_change_process)
					? post_prompt_insert->
						folder->
						post_change_process->
						command_line
					: (char *)0 );

	if ( post_prompt_insert->insert )
	{
		post_prompt_insert->
			dictionary_separate_post_prompt_insert->
			query_dictionary =
				post_prompt_insert_query_dictionary(
				     post_prompt_insert->
					dictionary_separate_post_prompt_insert->
					row_zero_dictionary,
				     APPASERVER_RELATION_OPERATOR_PREFIX,
				     APPASERVER_EQUAL );
	}

	return post_prompt_insert;
}

POST_PROMPT_INSERT *post_prompt_insert_calloc( void )
{
	POST_PROMPT_INSERT *post_prompt_insert;

	if ( ! ( post_prompt_insert =
			calloc( 1, sizeof( POST_PROMPT_INSERT ) ) ) )
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
			DICTIONARY *row_zero_dictionary,
			char *appaserver_relation_operator_prefix,
			char *appaserver_equal )
{
	DICTIONARY *query_dictionary;
	LIST *key_list;
	char *key;
	char query_key[ 128 ];

	if ( !dictionary_length( row_zero_dictionary ) )
		return (DICTIONARY *)0;

	query_dictionary = dictionary_small();

	key_list = dictionary_key_list( row_zero_dictionary );

	list_rewind( key_list );

	do {
		key = list_get( key_list );

		dictionary_set(
			query_dictionary,
			key,
			dictionary_get(
				key,
				row_zero_dictionary ) );

		sprintf(query_key,
			"%s%s",
			appaserver_relation_operator_prefix,
			key );

		dictionary_set(
			query_dictionary,
			strdup( query_key ),
			appaserver_equal );

	} while ( list_next( key_list ) );

	return query_dictionary;
}

