/* $APPASERVER_HOME/library/edit_table.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EDIT_TABLE_H
#define EDIT_TABLE_H

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
	/* Input */
	/* ----- */
	char *application_name;
	char *login_name;
	char *session;
	char *folder_name;
	char *role_name;
	char *target_frame;
	char *state;
	POST_DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	ROLE *role;
	FOLDER *folder;
	LIST *role_folder_list;
	DICTIONARY_SEPARATE *dictionary_separate;
	boolean primary_keys_non_edit;

} EDIT_TABLE;

/* Prototypes */
/* ---------- */
EDIT_TABLE *edit_table_calloc(
			void );

EDIT_TABLE *edit_table_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			DICTIONARY *post_dictionary );

/* Returns program memory */
/* ---------------------- */
char *edit_table_state(
			LIST *role_folder_list );

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

#endif
