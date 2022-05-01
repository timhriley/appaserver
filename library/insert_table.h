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
	DICTIONARY_SEPARATE_INSERT_TABLE *dictionary_separate_insert_table;
	boolean forbid;
} INSERT_TABLE;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
INSERT_TABLE *insert_table_new(
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
boolean insert_table_forbid(
			boolean role_folder_insert );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *insert_table_output_system_string(
			char *insert_table_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *target_frame,
			char *dictionary_separate_send_string,
			char *appaserver_error_filename );

/* Private */
/* ------- */
INSERT_TABLE *insert_table_calloc(
			void );
#endif
