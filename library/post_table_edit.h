/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/post_table_edit.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef POST_TABLE_EDIT_H
#define POST_TABLE_EDIT_H

#include "boolean.h"
#include "list.h"
#include "session.h"
#include "role.h"
#include "folder.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "appaserver_parameter.h"
#include "operation.h"
#include "update.h"

#define POST_TABLE_EDIT_EXECUTABLE		"post_table_edit"
#define POST_TABLE_EDIT_DRILLDOWN_EXECUTABLE	"drilldown"
#define POST_TABLE_EDIT_FOLDER_LABEL		"post_table_edit_folder"
#define POST_TABLE_EDIT_FORM_EXPIRED		\
	"<h3>Your form has expired. Please regenerate it.</h3>"

typedef struct
{
	LIST *role_folder_list;
	boolean role_folder_lookup_boolean;
	boolean post_table_edit_forbid;
	boolean role_folder_update_boolean;
	char *table_edit_state;
	ROLE *role;
	LIST *exclude_attribute_name_list;
	APPASERVER_PARAMETER *appaserver_parameter;
	FOLDER *folder;
	LIST *relation_mto1_isa_list;
	PROCESS *post_change_process;
	LIST *relation_mto1_list;
	LIST *relation_one2m_recursive_list;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	char *appaserver_update_filespecification;
	DICTIONARY *file_dictionary;
	LIST *folder_operation_list;
	LIST *folder_operation_name_list;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_POST_TABLE_EDIT *dictionary_separate;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_name_list;
	LIST *folder_attribute_date_name_list;
	char *appaserver_error_filename;
} POST_TABLE_EDIT_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_TABLE_EDIT_INPUT *post_table_edit_input_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		LIST *drilldown_relation_foreign_key_list );

/* Process */
/* ------- */
POST_TABLE_EDIT_INPUT *post_table_edit_input_calloc(
		void );

typedef struct
{
	SESSION_FOLDER *session_folder;
	POST_TABLE_EDIT_INPUT *post_table_edit_input;
	UPDATE *update;
	OPERATION_ROW_LIST *operation_row_list;
	DICTIONARY *dictionary_separate_send_dictionary;
	char *dictionary_separate_send_string;
} POST_TABLE_EDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
POST_TABLE_EDIT *post_table_edit_new(
		int argc,
		char **argv,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *target_frame,
		pid_t process_id,
		LIST *drilldown_relation_foreign_key_list );

/* Process */
/* ------- */
POST_TABLE_EDIT *post_table_edit_calloc(
		void );

/* Usage */
/* ----- */
boolean post_table_edit_forbid(
		boolean role_folder_lookup_boolean );

#endif
