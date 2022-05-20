/* $APPASERVER_HOME/library/post_table_insert.h			*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef POST_TABLE_INSERT_H
#define POST_TABLE_INSERT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "insert.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

#define POST_TABLE_INSERT_EXECUTABLE	"post_table_insert"

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

	boolean lookup_boolean;
	INSERT *insert;

	/* Driver */
	/* ------ */
	char *sql_error_message_list_string;
	char *edit_table_output_system_string;
	char *insert_table_output_system_string;
} POST_TABLE_INSERT; 

/* Usage */
/* ----- */
POST_TABLE_INSERT *post_table_insert_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame );

/* Process */
/* ------- */
POST_TABLE_INSERT *post_table_insert_calloc(
			void );

#endif
