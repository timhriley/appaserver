/* $APPASERVER_HOME/library/insert_table_form.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INSERT_TABLE_FORM_H
#define INSERT_TABLE_FORM_H

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
	char *insert_table_form_state;
	boolean primary_keys_non_edit;

} INSERT_TABLE_FORM;

/* Prototypes */
/* ---------- */
INSERT_TABLE_FORM *insert_table_form_calloc(
			void );

INSERT_TABLE_FORM *insert_table_form_fetch(
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *target_frame,
			DICTIONARY *post_dictionary );

boolean insert_table_form_forbid(
			boolean role_folder_insert );

#endif
