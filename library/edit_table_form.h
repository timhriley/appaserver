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
#include "dictionary_appaserver.h"

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
	char *insert_update_key;
	char *target_frame;
	DICTIONARY *post_dictionary;

	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	ROLE_FOLDER *role_folder;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char *edit_table_form_state;
	boolean primary_keys_non_edit;

} EDIT_TABLE_FORM;

/* Prototypes */
/* ---------- */
EDIT_TABLE_FORM *edit_table_form_calloc(
			void );

EDIT_TABLE_FORM *edit_table_form_new(
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
			ROLE_FOLDER *role_folder );

boolean edit_table_form_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

#endif
