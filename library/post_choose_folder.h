/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_folder.h			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef POST_CHOOSE_FOLDER_H
#define POST_CHOOSE_FOLDER_H

#include "boolean.h"
#include "list.h"
#include "folder.h"
#include "relation.h"
#include "drillthru.h"

#define POST_CHOOSE_FOLDER_EXECUTABLE	"post_choose_folder"

typedef struct
{
	DRILLTHRU_START *drillthru_start;
	FOLDER *folder;
	boolean fetch_relation_mto1_isa;
	LIST *relation_mto1_isa_list;
	LIST *relation_one2m_pair_list;
	char *form_name;
	boolean isa_drop_down_boolean;
	boolean prompt_insert_boolean;
	boolean table_insert_boolean;
	boolean prompt_lookup_boolean;
	boolean table_edit_boolean;
	char *appaserver_error_filename;
	char *system_string;
} POST_CHOOSE_FOLDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_CHOOSE_FOLDER *post_choose_folder_new(
		/* ----------------------------------- */
		/* See session_folder_integrity_exit() */
		/* ----------------------------------- */
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *appaserver_data_directory );

/* Process */
/* ------- */
POST_CHOOSE_FOLDER *post_choose_folder_calloc(
		void );

/* Returns drillthru_start_current_folder_name or folder_name */
/* ---------------------------------------------------------- */
char *post_choose_folder_name(
		char *folder_name,
		char *drillthru_start_current_folder_name );

boolean post_choose_folder_fetch_relation_mto1_isa(
		const char *appaserver_insert_state,
		char *state );

/* Returns either const parameter or appaserver_form */
/* ------------------------------------------------- */
char *post_choose_folder_form_name(
		const char *form_appaserver_prompt,
		const char *form_appaserver_table,
		int relation_pair_one2m_list_length,
		char *appaserver_form );

boolean post_choose_folder_isa_drop_down_boolean(
		int relation_mto1_isa_list_length );

boolean post_choose_folder_prompt_insert_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_prompt,
		char *state,
		char *post_choose_folder_form_name );

boolean post_choose_folder_table_insert_boolean(
		const char *appaserver_insert_state,
		const char *form_appaserver_table,
		char *state,
		char *post_choose_folder_form_name );

boolean post_choose_folder_prompt_lookup_boolean(
		/* -------- */
		/* Not this */
		/* -------- */
		const char *appaserver_insert_state,
		const char *form_appaserver_prompt,
		char *state,
		char *post_choose_folder_form_name );

boolean post_choose_folder_table_edit_boolean(
		/* -------- */
		/* Not this */
		/* -------- */
		const char *appaserver_insert_state,
		const char *form_appaserver_table,
		char *state,
		char *post_choose_folder_form_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *post_choose_folder_system_string(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean isa_drop_down_boolean,
		boolean prompt_insert_boolean,
		boolean table_insert_boolean,
		boolean prompt_lookup_boolean,
		boolean table_edit_boolean,
		char *appaserver_error_filename,
		RELATION_MTO1 *relation_mto1_isa_first );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *post_choose_folder_action_string(
		char *post_choose_folder_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *state,
		char *folder_name );

#endif
