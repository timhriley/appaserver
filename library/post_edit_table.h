/* $APPASERVER_HOME/library/post_edit_table.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST_EDIT_TABLE_H
#define POST_EDIT_TABLE_H

#include "boolean.h"
#include "list.h"

#define POST_EDIT_TABLE_EXECUTABLE	"post_edit_table"

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *target_frame;
	char *detail_base_folder_name;

	/* Process */
	/* ------- */
	SESSION_FOLDER *session_folder;
} POST_EDIT_TABLE;

/* POST_EDIT_TABLE operations */
/* -------------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
POST_EDIT_TABLE *post_edit_table_new(
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame,
			char *detail_base_folder_name );

/* Process */
/* ------- */
POST_EDIT_TABLE *post_edit_table_calloc(
			void );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *post_edit_table_action_string(
			char *post_edit_table_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *detail_base_folder_name );

#endif
