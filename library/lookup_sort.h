/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/lookup_sort.h	   		   	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef LOOKUP_SORT_H
#define LOOKUP_SORT_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "folder_row_level_restriction.h"
#include "query.h"
#include "form_lookup_sort.h"

#define LOOKUP_SORT_EXECUTABLE		"post_change_sort_order"

typedef struct
{
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *dictionary_separate;
	ROLE *role;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	char *attribute_name;
} LOOKUP_SORT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SORT_INPUT *lookup_sort_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
LOOKUP_SORT_INPUT *lookup_sort_input_calloc(
		void );

/* Returns one of the first three parameters or null */
/* ------------------------------------------------- */
char *lookup_sort_input_attribute_name(
		char *attribute_name_sort_order,
		char *attribute_name_display_order,
		char *attribute_name_sequence_number,
		LIST *folder_attribute_name_list );

typedef struct
{
	LOOKUP_SORT_INPUT *lookup_sort_input;
	QUERY_PRIMARY_KEY *query_primary_key;
	int row_count;
} LOOKUP_SORT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SORT *lookup_sort_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
LOOKUP_SORT *lookup_sort_calloc(
		void );

/* Returns row_list_length */
/* ----------------------- */
int lookup_sort_row_count(
		int row_list_length );

#define LOOKUP_SORT_FORM_NO_ROWS_MESSAGE \
		"No rows selected to display"

typedef struct
{
	LOOKUP_SORT *lookup_sort;
	LIST *javascript_filename_list;
	char *action_string;
	FORM_LOOKUP_SORT *form_lookup_sort;
} LOOKUP_SORT_FORM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SORT_FORM *lookup_sort_form_new(
		char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
LOOKUP_SORT_FORM *lookup_sort_form_calloc(
		void );

LIST *lookup_sort_form_javascript_filename_list(
		void );

/* Returns heap memory */
/* ------------------- */
char *lookup_sort_form_action_string(
		char *lookup_sort_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

#define LOOKUP_SORT_EXECUTE_COMPLETE_MESSAGE \
		"Sort update completed."

typedef struct
{
	LOOKUP_SORT_INPUT *lookup_sort_input;
	QUERY_FETCH *query_fetch;
	char *appaserver_table_name;
	LIST *update_statement_list;
	char *system_string;
} LOOKUP_SORT_EXECUTE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_SORT_EXECUTE *lookup_sort_execute_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary,
		char *appaserver_error_filename );

/* Process */
/* ------- */
LOOKUP_SORT_EXECUTE *lookup_sort_execute_calloc(
		void );

/* Usage */
/* ----- */
LIST *lookup_sort_execute_update_statement_list(
		LIST *primary_key_list,
		char *lookup_sort_input_attribute_name,
		LIST *query_fetch_row_list,
		char *appaserver_table_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *lookup_sort_execute_update_statement(
		LIST *primary_key_list,
		char *lookup_sort_input_attribute_name,
		char *appaserver_table_name,
		LIST *query_row_cell_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_FETCH *lookup_sort_execute_query_fetch(
		LIST *folder_attribute_primary_key_list,
		char *attribute_name,
		DICTIONARY *original_post_dictionary );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_ROW *lookup_sort_execute_query_row(
		LIST *folder_attribute_primary_key_list,
		char *attribute_name,
		DICTIONARY *original_post_dictionary,
		int index );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_CELL *lookup_sort_execute_query_cell(
		char *attribute_name,
		DICTIONARY *original_post_dictionary,
		int index );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *lookup_sort_execute_system_string(
		char *appaserver_error_filename );

/* Usage */
/* ----- */
void lookup_sort_execute_update(
		LIST *update_statement_list,
		char *lookup_sort_execute_system_string );

#endif
