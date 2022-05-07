/* $APPASERVER_HOME/library/table_insert.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TABLE_INSERT_H
#define TABLE_INSERT_H

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
#define TABLE_INSERT_OUTPUT_EXECUTABLE	"output_table_insert"

typedef struct
{
	ROLE *role;
	FOLDER *folder;
	LIST *role_folder_list;
	DICTIONARY_SEPARATE_TABLE_INSERT *dictionary_separate_table_insert;
	boolean forbid;
} TABLE_INSERT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
TABLE_INSERT *table_insert_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *message,
			boolean menu_horizontal_boolean,
			DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
boolean table_insert_forbid(
			boolean role_folder_insert );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *table_insert_output_system_string(
			char *table_insert_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */
TABLE_INSERT *table_insert_calloc(
			void );
#endif
