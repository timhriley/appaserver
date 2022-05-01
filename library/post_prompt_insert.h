/* $APPASERVER_HOME/library/post_prompt_insert.h			*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST_PROMPT_INSERT_H
#define POST_PROMPT_INSERT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "insert.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

#define POST_PROMPT_INSERT_EXECUTABLE	"post_prompt_insert"

typedef struct
{
	/* Process */
	/* ------- */
	SESSION_FOLDER *session_folder;
	ROLE *role;
	FOLDER *folder;
	POST_DICTIONARY *post_dictionary;

	DICTIONARY_SEPARATE_POST_PROMPT_INSERT *
		dictionary_separate_post_prompt_insert;

	INSERT *insert;

	/* Driver */
	/* ------ */
	char *sql_error_message_list_string;
	char *edit_table_output_system_string;
	char *insert_table_output_system_string;
} POST_PROMPT_INSERT;

/* POST_PROMPT_INSERT operations */
/* ----------------------------- */

/* Usage */
/* ----- */
POST_PROMPT_INSERT *post_prompt_insert_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name );

/* Process */
/* ------- */
POST_PROMPT_INSERT *post_prompt_insert_calloc(
			void );

DICTIONARY *post_prompt_insert_query_dictionary(
			DICTIONARY *row_zero_dictionary,
			char *appaserver_relation_operator_prefix,
			char *appaserver_equal );

#endif
