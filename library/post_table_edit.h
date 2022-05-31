/* $APPASERVER_HOME/library/post_table_edit.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST_TABLE_EDIT_H
#define POST_TABLE_EDIT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "operation.h"
#include "update.h"

#define POST_TABLE_EDIT_EXECUTABLE	"post_table_edit"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *target_frame;
	char *detail_base_folder_name;

	/* Process */
	/* ------- */
	SESSION_FOLDER *session_folder;
	ROLE *role;
	FOLDER *folder;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_POST_TABLE_EDIT *dictionary_separate;
	char *table_edit_spool_filename;
	DICTIONARY *file_dictionary;
	UPDATE *update;
	OPERATION_ROW_LIST *operation_row_list;
} POST_TABLE_EDIT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
POST_TABLE_EDIT *post_table_edit_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name );

/* Process */
/* ------- */
POST_TABLE_EDIT *post_table_edit_calloc(
			void );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *post_table_edit_action_string(
			char *post_table_edit_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name );

#endif
