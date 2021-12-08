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
#include "dictionary.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "menu.h"
#include "row_security.h"
#include "query.h"

/* Constants */
/* --------- */

/* Data structures */
/* --------------- */
typedef struct
{
	/* Process */
	/* ------- */
	ROLE *role;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	char *state;
	boolean menu_boolean;
	MENU *menu;
	boolean primary_keys_non_edit;
	ROW_SECURITY_EDIT_TABLE *row_security_edit_table;
	QUERY_EDIT_TABLE *query_edit_table;
	DOCUMENT_EDIT_TABLE *document_edit_table;
} EDIT_TABLE;

/* Prototypes */
/* ---------- */
EDIT_TABLE *edit_table_calloc(
			void );

EDIT_TABLE *edit_table_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *role_name,
			char *target_frame,
			boolean frameset_menu_horizontal,
			DICTIONARY *query_dictionary,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *ignore_select_attribute_name_list );

/* Returns program memory */
/* ---------------------- */
char *edit_table_state(
			LIST *role_folder_list );

int edit_table_row_insert_count(
			DICTIONARY *non_prefixed_dictionary );

int edit_table_cell_update_count(
			DICTIONARY *non_prefixed_dictionary );

/* Returns heap memory */
/* ------------------- */
char *edit_table_cell_update_folder_list_string(
			DICTIONARY *non_prefixed_dictionary );

/* Returns heap memory */
/* ------------------- */
char *edit_table_results_string(
			DICTIONARY *non_prefixed_dictionary );

boolean edit_table_content_type(
			boolean with_dynarch_menu );

boolean edit_table_primary_keys_non_edit(
			int relation_mto1_isa_list_length );

#endif
