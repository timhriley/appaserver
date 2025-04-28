/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/delete.h		 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org 	*/
/* -------------------------------------------------------------------- */

#ifndef DELETE_H
#define DELETE_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "folder.h"
#include "folder_row_level_restriction.h"
#include "role.h"
#include "security.h"
#include "query.h"

#define DELETE_MESSAGE			\
	"Delete: delete row count=%u, update cell count=%u"

#define DELETE_COMPLETE_MESSAGE		\
	"Delete complete: delete row count=%u, update cell count=%u"

typedef struct
{
	char *sql_statement;
	char *command_line;
} DELETE_ONE2M_UPDATE_CELL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell_new(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *appaserver_error_filename,
		char *folder_table_name,
		PROCESS *post_change_process,
		LIST *query_row_cell_list,
		char *query_cell_where_string,
		QUERY_CELL *set_null_query_cell );

/* Process */
/* ------- */
DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell_calloc(
			void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_update_cell_sql_statement(
			char *folder_table_name,
			char *set_null_attribute_name,
			char *query_cell_where_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_update_cell_command_line(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *appaserver_error_filename,
		char *query_cell_attribute_name,
		char *query_cell_select_datum,
		LIST *query_row_cell_list,
		char *post_change_process_command_line );

typedef struct
{
	LIST *delete_one2m_update_cell_list;
} DELETE_ONE2M_UPDATE_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row_new(
		char *appaserver_error_filename,
		char *folder_table_name,
		LIST *set_null_query_cell_list,
		PROCESS *post_change_process,
		LIST *query_row_cell_list );

/* Process */
/* ------- */
DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row_calloc(
		void );

typedef struct
{
	char *folder_name;
	LIST *delete_one2m_update_row_list;
} DELETE_ONE2M_UPDATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ONE2M_UPDATE *delete_one2m_update_new(
		char *appaserver_error_filename,
		char *folder_name,
		char *folder_table_name,
		LIST *set_null_query_cell_list,
		PROCESS *post_change_process,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
DELETE_ONE2M_UPDATE *delete_one2m_update_calloc(
		void );

typedef struct
{
	LIST *delete_one2m_row_list;
} DELETE_ONE2M_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ONE2M_FETCH *delete_one2m_fetch_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		char *many_folder_table_name,
		PROCESS *post_change_process,
		LIST *query_fetch_row_list,
		LIST *relation_one2m_list );

/* Process */
/* ------- */
DELETE_ONE2M_FETCH *delete_one2m_fetch_calloc(
		void );

typedef struct
{
	LIST *list;
} DELETE_ONE2M_LIST;

/* Usage */
/* ----- */
DELETE_ONE2M_LIST *delete_one2m_list_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		LIST *relation_one2m_list );

/* Process */
/* ------- */
DELETE_ONE2M_LIST *delete_one2m_list_calloc(
		void );

typedef struct
{
	char *delete_where;
	char *delete_sql_statement;
	char *pre_command_line;
	char *command_line;
	DELETE_ONE2M_LIST *delete_one2m_list;
} DELETE_ONE2M_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ONE2M_ROW *delete_one2m_row_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		char *many_folder_table_name,
		PROCESS *post_change_process,
		LIST *query_row_cell_list,
		LIST *relation_one2m_list );

/* Process */
/* ------- */
DELETE_ONE2M_ROW *delete_one2m_row_calloc(
		void );

typedef struct
{
	PROCESS *post_change_process;
	char *many_folder_name;
	char *table_name;
	LIST *query_cell_list;
	char *select_string;
	char *query_cell_where_string;
	char *query_system_string;
	QUERY_FETCH *query_fetch;
	DELETE_ONE2M_UPDATE *delete_one2m_update;
	LIST *relation_one2m_list;
	DELETE_ONE2M_FETCH *delete_one2m_fetch;
} DELETE_ONE2M;

/* Usage */
/* ----- */
DELETE_ONE2M *delete_one2m_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *one_folder_query_cell_list,
		char *many_folder_name,
		LIST *many_folder_attribute_primary_list,
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list,
		boolean foreign_key_none_primary,
		PROCESS *post_change_process );

/* Process */
/* ------- */
DELETE_ONE2M *delete_one2m_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_table_name(
		char *application_name,
		char *many_folder_name );

/* Returns heap memory */
/* ------------------- */
char *delete_one2m_select_string(
		LIST *many_folder_primary_key_list,
		char delimiter );

/* Usage */
/* ----- */
LIST *delete_one2m_query_fetch_list(
		LIST *delete_one2m_list );

typedef struct
{
	char *one_folder_name;
	LIST *query_cell_list;
	char *delete_where;
	char *delete_sql_statement;
	PROCESS *post_change_process;
	char *pre_command_line;
	char *command_line;
	LIST *relation_one2m_list;
	DELETE_ONE2M_LIST *delete_one2m_list;
} DELETE_MTO1_ISA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_MTO1_ISA *delete_mto1_isa_new(
		char *application_name,
		char *role_name,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		char *one_folder_name,
		LIST *one_folder_attribute_primary_key_list,
		char *post_change_process_name );

/* Process */
/* ------- */
DELETE_MTO1_ISA *delete_mto1_isa_calloc(
		void );

typedef struct
{
	LIST *list;
} DELETE_MTO1_ISA_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_MTO1_ISA_LIST *delete_mto1_isa_list_new(
		char *application_name,
		char *role_name,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		LIST *relation_mto1_isa_list );

/* Process */
/* ------- */
DELETE_MTO1_ISA_LIST *delete_mto1_isa_list_calloc(
		void );

typedef struct
{
	char *delete_where;
	char *delete_sql_statement;
	char *delete_pre_command_line;
	char *delete_command_line;
} DELETE_ROOT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_ROOT *delete_root_new(
		char *application_name,
		char *folder_name,
		LIST *primary_query_cell_list,
		PROCESS *post_change_process,
		char *security_entity_where,
		char *appaserver_error_filename );

/* Process */
/* ------- */
DELETE_ROOT *delete_root_calloc(
		void );

typedef struct
{
	LIST *folder_operation_list;
	boolean folder_operation_delete_boolean;
	char *appaserver_error_filename;
	char *appaserver_parameter_mount_point;
	ROLE *role;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	FOLDER *folder;
	LIST *relation_one2m_list;
	LIST *relation_mto1_isa_list;
} DELETE_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_INPUT *delete_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */
DELETE_INPUT *delete_input_calloc(
		void );

typedef struct
{
	char *statement;
	char *pre_command_line;
	char *command_line;
} DELETE_SQL;

/* Usage */
/* ----- */
LIST *delete_sql_list(
		DELETE_ROOT *delete_root,
		DELETE_ONE2M_LIST *delete_one2m_list,
		DELETE_MTO1_ISA_LIST *delete_mto1_isa_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE_SQL *delete_sql_new(
		char *statement,
		char *pre_command_line,
		char *command_line );

/* Process */
/* ------- */
DELETE_SQL *delete_sql_calloc(
		void );

/* Usage */
/* ----- */
boolean delete_sql_getset(
		LIST *delete_sql_list /* in/out */,
		DELETE_SQL *candidate_delete_sql );

/* Usage */
/* ----- */
boolean delete_sql_duplicate_boolean(
		DELETE_SQL *delete_sql,
		DELETE_SQL *candidate_delete_sql );

/* Usage */
/* ----- */
void delete_sql_free(
		DELETE_SQL *delete_sql );

/* Usage */
/* ----- */
void delete_sql_one2m(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_list );

/* Usage */
/* ----- */
void delete_sql_one2m_delete_update(
		LIST *delete_sql_list /* in/out */,
		DELETE_ONE2M_FETCH *delete_one2m_fetch,
		DELETE_ONE2M_UPDATE *delete_one2m_update );

/* Usage */
/* ----- */
void delete_sql_one2m_delete(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_row_list );

/* Usage */
/* ----- */
void delete_sql_one2m_update(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_update_row_list );

/* Usage */
/* ----- */
void delete_sql_update_cell(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_update_cell_list );

/* Usage */
/* ----- */
void delete_sql_mto1_isa(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_mto1_isa_list );

typedef struct
{
	DELETE_INPUT *delete_input;
	DELETE_ROOT *delete_root;
	DELETE_ONE2M_LIST *delete_one2m_list;
	DELETE_MTO1_ISA_LIST *delete_mto1_isa_list;
	LIST *delete_sql_list;
	unsigned int row_count;
	unsigned int update_cell_count;
	char *message_string;
	char *complete_message_string;
} DELETE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DELETE *delete_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *primary_query_cell_list,
		boolean isa_boolean,
		boolean update_null_boolean );

/* Process */
/* ------- */
DELETE *delete_calloc(
		void );

unsigned int delete_row_count(
		LIST *delete_sql_list );

unsigned int delete_update_cell_count(
		LIST *delete_sql_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *delete_where(
		LIST *primary_query_cell_list,
		char *security_entity_where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_sql_statement(
		char *folder_table_name,
		char *delete_where );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_command_line(
		char *delete_state,
		LIST *primary_query_cell_list,
		char *post_change_process_command_line,
		char *appaserver_error_filename );

/* Usage */
/* ----- */
LIST *delete_mto1_isa_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *one_folder_attribute_primary_key_list );

/* Usage */
/* ----- */
LIST *delete_one2m_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *relation_foreign_key_list );

/* Usage */
/* ----- */
LIST *delete_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *key_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_message_string(
		const char *message,
		unsigned int delete_row_count,
		unsigned int delete_update_cell_count );

/* Usage */
/* ----- */
LIST *delete_sql_statement_list(
		DELETE_ROOT *delete_root,
		DELETE_ONE2M_LIST *delete_one2m_list,
		DELETE_MTO1_ISA_LIST *delete_mto1_isa_list );

/* Usage */
/* ----- */
LIST *delete_pre_command_list(
		DELETE_ROOT *delete_root,
		DELETE_ONE2M_LIST *delete_one2m_list,
		DELETE_MTO1_ISA_LIST *delete_mto1_isa_list );

/* Usage */
/* ----- */
LIST *delete_command_list(
		DELETE_ROOT *delete_root,
		DELETE_ONE2M_LIST *delete_one2m_list,
		DELETE_MTO1_ISA_LIST *delete_mto1_isa_list );

/* Driver */
/* ------ */
void delete_display(
		LIST *delete_sql_list );

/* Driver */
/* ------ */
void delete_execute(
		LIST *delete_sql_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *delete_execute_system_string(
		const char *sql_executable );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *delete_key_list_data_string_sql(
		const char sql_delimiter,
		char *table_name,
		LIST *key_list,
		char *data_string );

#endif
