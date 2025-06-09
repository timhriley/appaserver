/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_table_insert.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_TABLE_INSERT_H
#define POST_TABLE_INSERT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "insert.h"
#include "process.h"
#include "post_dictionary.h"
#include "appaserver_parameter.h"
#include "appaserver_user.h"
#include "pair_one2m.h"
#include "dictionary_separate.h"
#include "vertical_new_post.h"

#define POST_TABLE_INSERT_EXECUTABLE	"post_table_insert"
#define POST_TABLE_INSERT_PROCESS_NAME	\
					"table_insert"

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_insert_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	RELATION_MTO1 *relation_mto1_automatic_preselection;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_upload_filename_list;
	APPASERVER_PARAMETER *appaserver_parameter;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_POST_TABLE_INSERT *dictionary_separate;
	PROCESS *post_change_process;
	APPASERVER_USER *insert_appaserver_user;
} POST_TABLE_INSERT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_TABLE_INSERT_INPUT *post_table_insert_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
POST_TABLE_INSERT_INPUT *post_table_insert_input_calloc(
		void );

/* Returns prompt_dictionary or dictionary_small() */
/* ----------------------------------------------- */
DICTIONARY *post_table_insert_prompt_dictionary(
		const char *table_edit_onload_javascript_key,
		DICTIONARY *dictionary_separate_prompt_dictionary /* in/out */,
		VERTICAL_NEW_POST *vertical_new_post );

typedef struct
{
	SESSION_FOLDER *session_folder;
	POST_TABLE_INSERT_INPUT *post_table_insert_input;
	VERTICAL_NEW_POST *vertical_new_post;
	DICTIONARY *query_dictionary;
	DICTIONARY *prompt_dictionary;
	INSERT *insert;
	PAIR_ONE2M_POST_TABLE_INSERT *pair_one2m_post_table_insert;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
} POST_TABLE_INSERT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_TABLE_INSERT *post_table_insert_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
POST_TABLE_INSERT *post_table_insert_calloc(
		void );

/* Usage */
/* ----- */
DICTIONARY *post_table_insert_query_dictionary(
		const char *query_relation_operator_prefix,
		const char *query_equal,
		DICTIONARY *prompt_dictionary,
		DICTIONARY *multi_row_dictionary,
		VERTICAL_NEW_POST *vertical_new_post );

#endif
