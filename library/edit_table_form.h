/* $APPASERVER_HOME/library/edit_table_form.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef EDIT_TABLE_FORM_H
#define EDIT_TABLE_FORM_H

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

} EDIT_TABLE_FORM;

/* Prototypes */
/* ---------- */
EDIT_TABLE_FORM *edit_table_form_calloc(
			void );

EDIT_TABLE_FORM *edit_table_form_fetch(
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
char *edit_table_form_state(
			LIST *role_folder_list );

boolean edit_table_form_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

#endif
