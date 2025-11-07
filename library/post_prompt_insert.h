/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_prompt_insert.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_PROMPT_INSERT_H
#define POST_PROMPT_INSERT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "process.h"
#include "insert.h"
#include "post_dictionary.h"
#include "pair_one2m.h"
#include "appaserver_user.h"
#include "vertical_new_prompt.h"
#include "dictionary_separate.h"
#include "appaserver_parameter.h"

#define POST_PROMPT_INSERT_EXECUTABLE	"post_prompt_insert"

#define POST_PROMPT_INSERT_MISSING_TEMPLATE \
	"<h3>Missing primary key: %s.</h3>"

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_insert_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *folder_attribute_upload_filename_list;
	POST_DICTIONARY *post_dictionary;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_POST_PROMPT_INSERT *dictionary_separate;
	boolean lookup_boolean;
	PROCESS *post_change_process;
	APPASERVER_USER *insert_appaserver_user;
	char *appaserver_error_filename;
} POST_PROMPT_INSERT_INPUT;

/* Usage */

/* Safely returns */
/* -------------- */
POST_PROMPT_INSERT_INPUT *post_prompt_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
POST_PROMPT_INSERT_INPUT *post_prompt_insert_input_calloc(
		void );

boolean post_prompt_insert_input_lookup_boolean(
		char *prompt_insert_lookup_checkbox,
		DICTIONARY *non_prefixed_dictionary );

typedef struct
{
	SESSION_FOLDER *session_folder;
	POST_PROMPT_INSERT_INPUT *post_prompt_insert_input;
	VERTICAL_NEW_PROMPT *vertical_new_prompt;
	INSERT *insert;
	PAIR_ONE2M_POST_PROMPT_INSERT *pair_one2m_post_prompt_insert;
	LIST *missing_primary_key_list;
	char *missing_display;
	DICTIONARY *query_dictionary;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
} POST_PROMPT_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_PROMPT_INSERT *post_prompt_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
POST_PROMPT_INSERT *post_prompt_insert_calloc(
		void );

/* Usage */
/* ----- */

/* Returns dictionary_small() or null */
/* ---------------------------------- */
DICTIONARY *post_prompt_insert_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *prompt_dictionary );

/* Usage */
/* ----- */
LIST *post_prompt_insert_missing_primary_key_list(
		LIST *folder_attribute_primary_key_list,
		DICTIONARY *prompt_dictionary );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_prompt_insert_missing_display(
		char *post_prompt_insert_missing_template,
		LIST *post_prompt_insert_missing_primary_key_list );

/* Usage */
/* ----- */
boolean post_prompt_insert_fatal_duplicate_boolean(
		const char *create_table_unique_suffix,
		const char *create_table_additional_suffix,
		char *appaserver_table_name,
		LIST *primary_key_list,
		char *insert_statement_error_string );

#endif
