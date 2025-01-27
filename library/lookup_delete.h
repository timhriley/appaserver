/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_delete.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_DELETE_H
#define LOOKUP_DELETE_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "dictionary.h"
#include "dictionary_separate.h"
#include "folder_row_level_restriction.h"
#include "query.h"

#define LOOKUP_DELETE_EXECUTABLE	"post_delete_folder_block"
#define LOOKUP_DELETE_NO_ROWS_MESSAGE	"No rows selected to delete"
#define LOOKUP_DELETE_SIMULATE_MESSAGE	"Delete simulated with row count:"
#define LOOKUP_DELETE_EXECUTE_LABEL	"lookup_delete_execute_yes"
#define LOOKUP_DELETE_ROW_MAX		1000
#define LOOKUP_DELETE_MAX_EXCEED_TEMPLATE \
"<h3>Sorry, but your delete of %d rows exceeds the maximum of %d.</h3>\n"

typedef struct
{
	LIST *folder_operation_list;
	boolean folder_operation_delete_boolean;
	FOLDER *folder;
	ROLE *role;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_date_name_list;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	DICTIONARY_SEPARATE_LOOKUP_DELETE *dictionary_separate;
	DICTIONARY *query_dictionary;
	boolean execute_boolean;
} LOOKUP_DELETE_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_DELETE_INPUT *lookup_delete_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
LOOKUP_DELETE_INPUT *lookup_delete_input_calloc(
		void );

DICTIONARY *lookup_delete_input_query_dictionary(
		DICTIONARY *query_dictionary,
		DICTIONARY *non_prefixed_dictionary );

boolean lookup_delete_input_execute_boolean(
		char *lookup_delete_execute_label,
		DICTIONARY *non_prefixed_dictionary );

typedef struct
{
	LOOKUP_DELETE_INPUT *lookup_delete_input;
	QUERY_PRIMARY_KEY *query_primary_key;
	char *title_string;
	char *sub_title_string;
	int fetch_row_count;
} LOOKUP_DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_DELETE *lookup_delete_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
LOOKUP_DELETE *lookup_delete_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *lookup_delete_title_string(
		char *folder_name );

/* Returns where_string or program memory */
/* -------------------------------------- */
char *lookup_delete_sub_title_string(
		char *where_string );

int lookup_delete_fetch_row_count(
		int query_fetch_row_list_length );

/* Driver */
/* ------ */
void lookup_delete_state_two_execute(
		char *application_name,
		char *login_name,
		char *folder_name,
		char *role_name,
		boolean execute_boolean,
		LIST *query_fetch_row_list,
		char *title_string );

/* Driver */
/* ------ */
void lookup_delete_state_one_form_output(
		const char *lookup_delete_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *lookup_delete_prompt_html(
		char *folder_name,
		int lookup_delete_fetch_row_count );

/* Returns heap memory */
/* ------------------- */
char *lookup_delete_action_string(
		const char *lookup_delete_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

#endif
