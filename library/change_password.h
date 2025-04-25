/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/change_password.h				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef CHANGE_PASSWORD_H
#define CHANGE_PASSWORD_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "appaserver.h"
#include "dictionary.h"

typedef struct
{
	SESSION_PROCESS *session_process;
	DICTIONARY *query_dictionary;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
	char *execute_system_string_table_edit;
} CHANGE_PASSWORD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHANGE_PASSWORD *change_password_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name );

/* Process */
/* ------- */
CHANGE_PASSWORD *change_password_calloc(
		void );

/* Usage */
/* ----- */
DICTIONARY *change_password_query_dictionary(
		const char *appaserver_user_login_name,
		const char *query_relation_operator_prefix,
		const char *query_equal,
		char *login_name );

#endif
