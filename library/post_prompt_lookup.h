/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_prompt_lookup.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_PROMPT_LOOKUP_H
#define POST_PROMPT_LOOKUP_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "dictionary_separate.h"
#include "drillthru.h"
#include "query.h"

#define POST_PROMPT_LOOKUP_EXECUTABLE		"post_prompt_lookup"
#define POST_PROMPT_LOOKUP_MISSING_MESSAGE	\
	"This query might drain too many resources. Please include:"

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_lookup_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_lookup_required_name_list;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *dictionary_separate;
	DRILLTHRU_STATUS *drillthru_status;
	char *selection;
	char *appaserver_error_filename;
} POST_PROMPT_LOOKUP_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input_new(
		char *form_radio_list_name,
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input_calloc(
		void );

/* Returns component of non_prefixed_dictionary */
/* -------------------------------------------- */
char *post_prompt_lookup_input_selection(
		char *form_radio_list_name,
		DICTIONARY *non_prefixed_dictionary );

typedef struct
{
	SESSION_FOLDER *session_folder;
	POST_PROMPT_LOOKUP_INPUT *post_prompt_lookup_input;
	QUERY_DROP_DOWN_LIST *query_drop_down_list;
	QUERY_ATTRIBUTE_LIST *query_attribute_list;
	LIST *include_name_list;
	LIST *missing_required_list;
	char *missing_html;
	LIST *relation_one2m_pair_list;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
	char *execute_system_string;
} POST_PROMPT_LOOKUP;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_PROMPT_LOOKUP *post_prompt_lookup_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *original_post_dictionary );

/* Process */
/* ------- */
POST_PROMPT_LOOKUP *post_prompt_lookup_calloc(
		void );

LIST *post_prompt_lookup_missing_required_list(
		LIST *folder_attribute_lookup_required_name_list,
		LIST *post_prompt_lookup_include_name_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *post_prompt_lookup_execute_system_string(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *non_prefixed_dictionary,
		char *appaserver_error_filename,
		char *post_prompt_lookup_input_selection,
		char *dictionary_separate_send_string );

/* Usage */
/* ----- */
LIST *post_prompt_lookup_include_name_list(
		QUERY_DROP_DOWN_LIST *query_drop_down_list,
		QUERY_ATTRIBUTE_LIST *query_attribute_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_prompt_lookup_missing_html(
		char *post_prompt_lookup_missing_message,
		char *folder_name,
		LIST *post_prompt_lookup_missing_required_list );

#endif
