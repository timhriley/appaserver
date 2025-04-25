/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/change_password.c				   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	   */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "dictionary_separate.h"
#include "table_edit.h"
#include "frameset.h"
#include "query.h"
#include "execute_system_string.h"
#include "change_password.h"

CHANGE_PASSWORD *change_password_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name )
{
	CHANGE_PASSWORD *change_password;

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	change_password = change_password_calloc();

	/* ---------------------------------------------------- */
	/* Sets appaserver environment and outputs argv.	*/
	/* Each parameter is security inspected.		*/
	/* Any error will exit( 1 ).				*/
	/* ---------------------------------------------------- */
	change_password->session_process =
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	change_password->query_dictionary =
		change_password_query_dictionary(
			APPASERVER_USER_LOGIN_NAME,
			QUERY_RELATION_OPERATOR_PREFIX,
			QUERY_EQUAL,
			login_name );

	change_password->dictionary_separate_send_dictionary =
		dictionary_separate_send_dictionary(
			(DICTIONARY *)0 /* sort_dictionary */,
			DICTIONARY_SEPARATE_SORT_PREFIX,
			change_password->query_dictionary,
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

	change_password->dictionary_separate_send_string =
		dictionary_separate_send_string(
			change_password->
				dictionary_separate_send_dictionary );

	change_password->execute_system_string_table_edit =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_table_edit(
			TABLE_EDIT_EXECUTABLE,
			session_key,
			login_name,
			role_name,
			APPASERVER_USER_TABLE,
			FRAMESET_PROMPT_FRAME /* target_frame */,
			(char *)0 /* results_string */,
			(char *)0 /* error_string */,
			change_password->dictionary_separate_send_string,
			appaserver_error_filename(
				application_name ) );

	return change_password;
}

CHANGE_PASSWORD *change_password_calloc( void )
{
	CHANGE_PASSWORD *change_password;

	if ( ! ( change_password = calloc( 1, sizeof ( CHANGE_PASSWORD ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return change_password;
}

DICTIONARY *change_password_query_dictionary(
		const char *appaserver_user_login_name,
		const char *query_relation_operator_prefix,
		const char *query_equal,
		char *login_name )
{
	DICTIONARY *query_dictionary;
	char *relation_key;

	if ( !login_name )
	{
		char message[ 128 ];

		sprintf(message, "login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	query_dictionary = dictionary_small();

	dictionary_set(
		query_dictionary,
		(char *)appaserver_user_login_name,
		login_name );

	relation_key =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		query_relation_key(
			(char *)query_relation_operator_prefix,
			(char *)appaserver_user_login_name );

	dictionary_set(
		query_dictionary,
		strdup( relation_key ),
		(char *)query_equal );

	return query_dictionary;
}

