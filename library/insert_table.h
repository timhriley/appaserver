/* $APPASERVER_HOME/library/insert_table.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_TABLE_H
#define INSERT_TABLE_H

/* Includes */
/* -------- */
#include "dictionary.h"
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define INSERT_TABLE_OUTPUT_EXECUTABLE	"output_insert_table"

typedef struct
{
	ROLE *role;
	FOLDER *folder;
	LIST *role_folder_list;
	DICTIONARY_SEPARATE_INSERT_TABLE *dictionary_separate;
	boolean forbid;
} INSERT_TABLE;

/* INSERT_TABLE operations */
/* ----------------------- */

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
INSERT_TABLE *insert_table_output_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *message,
			POST_DICTIONARY *post_dictionary );

/* Process */
/* ------- */
boolean insert_table_forbid(
			boolean role_folder_insert );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *insert_table_output_system_string(
			char *insert_table_output_executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */
INSERT_TABLE *insert_table_calloc(
			void );
#endif
