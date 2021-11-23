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
#include "query.h"
#include "row_security.h"

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
	QUERY *query;
	char *state;
	boolean with_dynarch_menu;
	int row_insert_count;
	int cell_update_count;
	char *cell_update_folder_list_string;
	char *results_string;
	boolean content_type;
	boolean primary_keys_non_edit;
	ROW_SECURITY *row_security;
	DOCUMENT *document;
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

boolean edit_table_with_dynarch_menu(
			char *target_frame );

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
