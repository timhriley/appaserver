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
#include "dictionary_separate.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	ROLE_FOLDER *role_folder;
	DICTIONARY_SEPARATE *dictionary_separate;
	char *insert_table_form_state;
	boolean primary_keys_non_edit;
} INSERT_TABLE;

/* Prototypes */
/* ---------- */
INSERT_TABLE *insert_table_calloc(
			void );

/* Always succeeds */
/* --------------- */
INSERT_TABLE *insert_table_output_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			char *dictionary_string );

boolean insert_table_form_forbid(
			boolean role_folder_insert );

#endif
