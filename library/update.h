/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/update.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "security.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "process.h"
#include "folder_attribute.h"
#include "folder.h"
#include "query.h"
#include "folder_row_level_restriction.h"
#include "process.h"

#define UPDATE_PREUPDATE_PREFIX		"preupdate_"
#define UPDATE_FOLDER_NAME_DELIMITER	','
#define UPDATE_RESULTS_KEY		"update_results"
#define UPDATE_ROW_LIST_ERROR_KEY	"update_row_list_error"

typedef struct
{
	FOLDER_ATTRIBUTE *folder_attribute;
	char *post_datum;
	char *file_datum;
} UPDATE_ATTRIBUTE;

/* Usage */
/* ----- */
LIST *update_attribute_list(
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *folder_attribute_append_isa_list,
		int row_number );

/* Usage */
/* ----- */
UPDATE_ATTRIBUTE *update_attribute_new(
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		int row_number,
		FOLDER_ATTRIBUTE *folder_attribute );

/* Process */
/* ------- */
UPDATE_ATTRIBUTE *update_attribute_calloc(
		void );

/* Usage */
/* ----- */
UPDATE_ATTRIBUTE *update_attribute_seek(
		char *attribute_name,
		LIST *update_attribute_list );

/* Usage */
/* ----- */
LIST *update_attribute_data_list(
		LIST *update_attribute_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_attribute_list_display(
		LIST *update_attribute_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *update_attribute_display(
		char *folder_name,
		char *attribute_name,
		char *post_datum,
		char *file_datum );

typedef struct
{
	UPDATE_ATTRIBUTE *update_attribute;
	char *string;
} UPDATE_WHERE;

/* Usage */
/* ----- */
UPDATE_WHERE *update_where_new(
		char *primary_key,
		LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_WHERE *update_where_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_where_list_string(
		char *security_entity_where,
		LIST *update_where_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_where_string(
		char *primary_key,
		char *datatype_name,
		char *datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_where_list_primary_data_string(
		char sql_delimiter,
		LIST *update_where_list );

typedef struct
{
	UPDATE_ATTRIBUTE *update_attribute;
	char *set_string;
} UPDATE_CHANGED;

/* Usage */
/* ----- */
UPDATE_CHANGED *update_changed_new(
		char *attribute_name,
		LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_CHANGED *update_changed_calloc(
		void );

/* Usage */
/* ----- */
boolean update_changed_boolean(
		char *file_datum,
		char *post_datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_changed_set_string(
		char *attribute_name,
		char *datatype_name,
		char *post_data );

/* Usage */
/* ----- */
boolean update_changed_primary_key_boolean(
		LIST *changed_list );

/* Usage */
/* ----- */
UPDATE_CHANGED *update_changed_seek(
		char *attribute_name,
		LIST *changed_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_changed_display(
		UPDATE_CHANGED *update_changed );

typedef struct
{
	char *folder_name;
	LIST *changed_list;
	char *set_string;
	LIST *where_list;
	char *update_where_list_string;
	char *sql_statement_string;
} UPDATE_CHANGED_LIST;

/* Usage */
/* ----- */
UPDATE_CHANGED_LIST *update_changed_list_new(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_primary_key_list,
		LIST *folder_attribute_name_list,
		LIST *update_attribute_list,
		char *security_entity_where );

/* Process */
/* ------- */
UPDATE_CHANGED_LIST *update_changed_list_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_changed_list_set_string(
		LIST *changed_list );

/* Returns heap memory */
/* ------------------- */
char *update_changed_list_sql_statement_string(
		char *folder_table_name,
		char *update_changed_list_set_string,
		char *update_where_list_string );

/* Returns heap memory */
/* ------------------- */
char *update_changed_list_display(
		UPDATE_CHANGED_LIST *update_changed_list );

typedef struct
{
	char *folder_name;
	int row_number;
	LIST *list;
} UPDATE_ONE2M_LIST;

/* Usage */
/* ----- */
UPDATE_ONE2M_LIST *update_one2m_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *folder_name,
		char *role_name,
		int row_number,
		LIST *relation_one2m_list,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list );

/* Process */
/* ------- */
UPDATE_ONE2M_LIST *update_one2m_list_calloc(
		void );

typedef struct
{
	LIST *update_attribute_list;
	UPDATE_CHANGED_LIST *update_changed_list;
	LIST *query_cell_primary_data_list;
	char *command_line;
	LIST *relation_one2m_list;
	UPDATE_ONE2M_LIST *update_one2m_list;
} UPDATE_ONE2M_ROW;

/* Usage */
/* ----- */
UPDATE_ONE2M_ROW *update_one2m_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list,
		LIST *many_attribute_name_list,
		LIST *many_primary_key_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list,
		LIST *query_row_cell_list );

/* Process */
/* ------- */
UPDATE_ONE2M_ROW *update_one2m_row_calloc(
		void );

/* Usage */
/* ----- */
LIST *update_one2m_row_update_attribute_list(
		char *many_folder_name,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list,
		LIST *update_changed_list,
		LIST *query_row_cell_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPDATE_ATTRIBUTE *update_one2m_row_update_attribute(
		char *many_folder_name,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list,
		LIST *update_changed_list,
		QUERY_CELL *query_cell );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_one2m_row_command_line(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *many_folder_name,
		char *post_change_process_name,
		char *appaserver_error_filename,
		LIST *query_cell_primary_data_list,
		LIST *update_one2m_row_update_attribute_list );

/* Usage */
/* ----- */
int update_one2m_row_list_cell_count(
		LIST *update_one2m_row_list );

/* Usage */
/* ----- */
LIST *update_one2m_row_list_folder_name_list(
		LIST *update_one2m_row_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPDATE_CHANGED_LIST *update_one2m_row_update_changed_list(
		char *many_folder_name,
		LIST *many_primary_key_list,
		LIST *update_attribute_list );

/* Usage */
/* ----- */
void update_one2m_row_list_pipe_execute(
		LIST *update_one2m_row_list,
		FILE *appaserver_output_pipe );

/* Usage */
/* ------ */
void update_one2m_row_list_command_execute(
		LIST *update_one2m_row_list );

/* Usage */
/* ------ */
void update_one2m_row_list_display(
		LIST *update_one2m_row_list );

typedef struct
{
	int row_number;
	char *many_folder_name;
	LIST *update_one2m_row_list;
} UPDATE_ONE2M_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPDATE_ONE2M_FETCH *update_one2m_fetch_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list,
		LIST *many_attribute_name_list,
		LIST *many_primary_key_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
UPDATE_ONE2M_FETCH *update_one2m_fetch_calloc(
		void );

typedef struct
{
	char *many_folder_name;
	LIST *query_cell_list;
	char *select_string;
	char *query_cell_where_string;
	char *query_system_string;
	QUERY_FETCH *query_fetch;
	UPDATE_ONE2M_FETCH *update_one2m_fetch;
} UPDATE_ONE2M;

/* Usage */
/* ----- */
UPDATE_ONE2M *update_one2m_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		char *many_folder_name,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list,
		LIST *many_attribute_name_list,
		LIST *many_primary_key_list,
		PROCESS *many_post_change_process,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		UPDATE_CHANGED_LIST *update_changed_list );

/* Process */
/* ------- */
UPDATE_ONE2M *update_one2m_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *update_one2m_select_string(
		LIST *many_attribute_name_list,
		char delimiter );

/* Usage */
/* ----- */
LIST *update_one2m_query_cell_list(
		LIST *update_where_list,
		LIST *relation_translate_list,
		LIST *many_folder_attribute_list );

/* Usage */
/* ----- */
LIST *update_one2m_changed_list(
		char *many_folder_name,
		LIST *update_changed_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */
LIST *update_one2m_where_list(
		char *many_folder_name,
		LIST *update_where_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */

/* Returns component of foreign_key_list */
/* ------------------------------------- */
char *update_one2m_changed_attribute_name(
		int primary_key_index,
		LIST *foreign_key_list );

/* Usage */
/* ----- */
void update_one2m_list_sql_statement_execute(
		LIST *update_one2m_list,
		FILE *appaserver_output_pipe );

/* Usage */
/* ----- */
void update_one2m_list_command_line_execute(
		LIST *update_one2m_list );

/* Usage */
/* ----- */
void update_one2m_list_display(
		LIST *update_one2m_list );

/* Usage */
/* ----- */
int update_one2m_list_cell_count(
		LIST *update_one2m_list );

/* Usage */
/* ----- */
LIST *update_one2m_list_folder_name_list(
		LIST *update_one2m_list );

typedef struct
{
	RELATION_MTO1 *relation_mto1_isa;
	LIST *update_attribute_list;
	UPDATE_CHANGED_LIST *update_changed_list;
	char *update_where_list_primary_data_string;
	char *update_command_line;
	LIST *relation_one2m_list;
	UPDATE_ONE2M_LIST *update_one2m_list;
} UPDATE_MTO1_ISA;

/* Usage */
/* ----- */
LIST *update_mto1_isa_list(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		LIST *relation_mto1_isa_list,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		boolean update_changed_primary_key_boolean,
		LIST *update_attribute_list );

/* Usage */
/* ----- */
UPDATE_MTO1_ISA *update_mto1_isa_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		int row_number,
		RELATION_MTO1 *relation_mto1_isa,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		boolean update_changed_primary_key_boolean,
		LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_MTO1_ISA *update_mto1_isa_calloc(
		void );

/* Usage */
/* ----- */
LIST *update_mto1_isa_update_attribute_list(
		LIST *one_folder_attribute_list,
		LIST *relation_translate_list,
		LIST *many_update_attribute_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
UPDATE_ATTRIBUTE *update_mto1_isa_update_attribute(
		LIST *one_folder_attribute_list,
		LIST *relation_translate_list,
		char *many_attribute_name,
		char *post_datum,
		char *file_datum,
		UPDATE_ATTRIBUTE *many_update_attribute );

/* Usage */
/* ----- */
int update_mto1_isa_list_cell_count(
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
int update_mto1_isa_cell_count(
		UPDATE_MTO1_ISA *update_mto1_isa );

/* Usage */
/* ----- */
LIST *update_mto1_isa_list_folder_name_list(
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
LIST *update_mto1_isa_folder_name_list(
		UPDATE_MTO1_ISA *update_mto1_isa );

/* Usage */
/* ----- */
void update_mto1_isa_list_sql_statement_execute(
		LIST *update_mto1_isa_list,
		FILE *appaserver_output_pipe );

/* Usage */
/* ----- */
void update_mto1_isa_sql_statement_execute(
		UPDATE_MTO1_ISA *update_mto1_isa,
		FILE *appaserver_output_pipe );

/* Usage */
/* ----- */
void update_mto1_isa_list_command_line_execute(
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
void update_mto1_isa_command_line_execute(
		UPDATE_MTO1_ISA *update_mto1_isa );

/* Usage */
/* ----- */
void update_mto1_isa_list_display(
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
void update_mto1_isa_display(
		UPDATE_MTO1_ISA *update_mto1_isa );

typedef struct
{
	char *folder_name;
	LIST *folder_attribute_primary_key_list;
	LIST *folder_attribute_name_list;
	UPDATE_CHANGED_LIST *update_changed_list;
	boolean update_changed_primary_key_boolean;
	char *update_where_list_primary_data_string;
	char *update_command_line;
} UPDATE_ROOT;

/* Usage */
/* ----- */
UPDATE_ROOT *update_root_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		LIST *update_attribute_list );

/* Process */
/* ------- */
UPDATE_ROOT *update_root_calloc(
			void );

int update_root_cell_count(
		int update_changed_list_length );

/* Returns char *update_error_string */
/* --------------------------------- */
char *update_root_execute(
		const char *sql_executable,
		char *update_sql_statement_string,
		char *appaserver_error_filename );

typedef struct
{
	int row_number;
	LIST *update_attribute_list;
	UPDATE_ROOT *update_root;
	UPDATE_ONE2M_LIST *update_one2m_list;
	LIST *update_mto1_isa_list;
} UPDATE_ROW;

/* Usage */
/* ----- */
UPDATE_ROW *update_row_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		int row_number );

/* Process */
/* ------- */
UPDATE_ROW *update_row_calloc(
		void );

/* Usage */
/* ----- */
int update_row_cell_count(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
LIST *update_row_folder_name_list(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */

/* Returns update_error_string or null */
/* ----------------------------------- */
char *update_row_execute(
		const char *sql_executable,
		char *application_name,
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list,
		char *appaserver_error_filename );

/* Usage */
/* ----- */
void update_row_command_line_execute(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list );

/* Usage */
/* ----- */
void update_row_display(
		UPDATE_ROOT *update_root,
		UPDATE_ONE2M_LIST *update_one2m_list,
		LIST *update_mto1_isa_list );

typedef struct
{
	int dictionary_highest_index;
	LIST *list;
	int cell_count;
	LIST *folder_name_list;
	char *folder_name_list_string;
} UPDATE_ROW_LIST;

/* Usage */
/* ----- */

/* Returns null if no updates */
/* -------------------------- */
UPDATE_ROW_LIST *update_row_list_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process,
		SECURITY_ENTITY *security_entity,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point );

/* Process */
/* ------- */
UPDATE_ROW_LIST *update_row_list_calloc(
		void );

int update_row_list_cell_count(
		LIST *update_row_list );

LIST *update_row_list_folder_name_list(
		LIST *update_row_list );

/* Returns heap memory or "" */
/* ------------------------- */
char *update_row_list_folder_name_list_string(
		const char update_folder_name_delimiter,
		LIST *update_row_list_folder_name_list );

/* Usage */
/* ----- */

/* Returns error_row_list_error_string or null */
/* ------------------------------------------- */
char *update_row_list_execute(
		const char *sql_executable,
		char *application_name,
		UPDATE_ROW_LIST *update_row_list,
		char *appaserver_error_filename );

/* Usage */
/* ----- */

/* Execute any post_change_process */
/* ------------------------------- */
void update_row_list_command_line_execute(
		UPDATE_ROW_LIST *update_row_list );

/* Usage */
/* ----- */
void update_row_list_zap_root(
		UPDATE_ROW_LIST *update_row_list );

/* Usage */
/* ----- */
void update_row_list_display(
		UPDATE_ROW_LIST *update_row_list );

typedef struct
{
	SECURITY_ENTITY *security_entity;
	char *appaserver_error_filename;
	char *appaserver_parameter_mount_point;
	UPDATE_ROW_LIST *update_row_list;
	char *results_string;
	char *error_string;
} UPDATE;

/* Usage */
/* ----- */
UPDATE *update_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		boolean override_row_restrictions,
		DICTIONARY *multi_row_dictionary,
		DICTIONARY *file_dictionary,
		FOLDER_ROW_LEVEL_RESTRICTION *
			folder_row_level_restriction,
		LIST *relation_one2m_list,
		LIST *relation_mto1_isa_list,
		LIST *folder_attribute_append_isa_list,
		PROCESS *post_change_process );

/* Process */
/* ------- */
UPDATE *update_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *update_results_string(
		int cell_count,
		char *update_row_list_folder_name_list_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_command_line(
		const char *appaserver_update_state,
		const char *update_preupdate_prefix,
		char *post_change_process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *post_change_process_name,
		char *appaserver_error_filename,
		char *update_where_list_primary_data_string,
		LIST *update_attribute_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *update_system_string(
		const char *sql_executable,
		char *appaserver_error_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *update_sql_statement_string(
		LIST *update_column_name_list,
		LIST *update_data_list,
		LIST *primary_key_list,
		LIST *primary_data_list,
		LIST *folder_attribute_list,
		char *table_name );

/* Usage */
/* ----- */
void update_statement_execute(
		const char *sql_executable,
		char *application_name,
		char *update_statement );

#endif
