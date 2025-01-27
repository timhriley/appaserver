/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_isa.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_CHOOSE_ISA_H
#define POST_CHOOSE_ISA_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "insert.h"

#define POST_CHOOSE_ISA_EXECUTABLE	"post_choose_isa"

typedef struct
{
	DICTIONARY *post_choose_isa_query_dictionary;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
	char *execute_system_string_table_edit;
} POST_CHOOSE_ISA_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup_new(
		char *session_key,
		char *login_name,
		char *role_name,
		FOLDER *one_folder,
		char *appaserver_error_filename,
		LIST *post_choose_isa_primary_data_list );

/* Process */
/* ------- */
POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup_calloc(
		void );

typedef struct
{
	FOLDER *one_folder;
	FOLDER *folder;
	POST_DICTIONARY *post_dictionary;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_PROMPT_LOOKUP *dictionary_separate;
	LIST *role_attribute_exclude_list;
	LIST *role_attribute_exclude_name_list;
	char *insert_login_name;
	char *appaserver_error_filename;
} POST_CHOOSE_ISA_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CHOOSE_ISA_INPUT *post_choose_isa_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *one_folder_name );

/* Process */
/* ------- */
POST_CHOOSE_ISA_INPUT *post_choose_isa_input_calloc(
		void );

typedef struct
{
	POST_CHOOSE_ISA_INPUT *post_choose_isa_input;
	LIST *primary_data_list;
	boolean lookup_checkbox_checked;
	POST_CHOOSE_ISA_LOOKUP *post_choose_isa_lookup;
	DICTIONARY *query_dictionary;
	DICTIONARY *prompt_dictionary;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
	int primary_data_list_length;
	char *execute_system_string_table_insert;
	PROCESS *post_change_process;
	INSERT *insert;

	/* Set externally */
	/* -------------- */
	char *execute_system_string_table_edit;
} POST_CHOOSE_ISA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CHOOSE_ISA *post_choose_isa_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *one_folder_name );

/* Process */
/* ------- */
POST_CHOOSE_ISA *post_choose_isa_calloc(
		void );

int post_choose_isa_primary_data_list_length(
		LIST *post_choose_isa_primary_data_list );

/* Usage */
/* ----- */
LIST *post_choose_isa_primary_data_list(
		LIST *folder_attribute_primary_key_list,
		DICTIONARY *prompt_dictionary );

/* Usage */
/* ----- */
boolean post_choose_isa_lookup_checkbox_checked(
		const char *widget_lookup_checkbox_name,
		DICTIONARY *non_prefixed_dictionary );

/* Usage */
/* ----- */

/* Returns dictionary_small() */
/* -------------------------- */
DICTIONARY *post_choose_isa_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		LIST *folder_attribute_primary_key_list,
		LIST *post_choose_isa_primary_data_list );

/* Usage */
/* ----- */

/* Returns dictionary_small() */
/* -------------------------- */
DICTIONARY *post_choose_isa_prompt_dictionary(
		LIST *folder_attribute_primary_key_list,
		LIST *post_choose_isa_primary_data_list );

#endif
